// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiodbus.h>
#include <gudev/gudev.h>

#include "daemon.h"
#include "helpers.h"

struct _GPIODBusDaemon {
	GObject parent;
	GDBusConnection *con;
	GUdevClient *udev;
	GDBusObjectManagerServer *chip_manager;
	GDBusObjectManagerServer *request_manager;
	GHashTable *chips;
	GHashTable *requests;
	GTree *req_id_root;
};

G_DEFINE_TYPE(GPIODBusDaemon, gpiodbus_daemon, G_TYPE_OBJECT);

typedef struct {
	GPIODChip *chip;
	GPIODBusChip *dbus_chip;
	GPIODBusDaemon *daemon;
	GDBusObjectManagerServer *line_manager;
	GHashTable *lines;
} GPIODBusDaemonChipData;

typedef struct {
	GPIODLineRequest *request;
	GPIODBusRequest *dbus_request;
	gint id;
	GPIODBusDaemonChipData *chip_data;
} GPIODBusDaemonRequestData;

typedef struct {
	GPIODBusLine *dbus_line;
	GPIODBusDaemonChipData *chip_data;
	GPIODBusDaemonRequestData *req_data;
} GPIODBusDaemonLineData;

static const gchar* const gpiodbus_daemon_udev_subsystems[] = { "gpio", NULL };

static void gpiodbus_daemon_dispose(GObject *obj)
{
	GPIODBusDaemon *self = GPIODBUS_DAEMON(obj);

	g_debug("disposing of the GPIO daemon");

	g_clear_pointer(&self->requests, g_hash_table_unref);
	g_clear_pointer(&self->chips, g_hash_table_unref);
	g_clear_pointer(&self->req_id_root, g_tree_destroy);
	g_clear_object(&self->con);

	G_OBJECT_CLASS(gpiodbus_daemon_parent_class)->dispose(obj);
}

static void gpiodbus_daemon_finalize(GObject *obj)
{
	GPIODBusDaemon *self = GPIODBUS_DAEMON(obj);

	g_debug("finalizing GPIO daemon");

	g_clear_object(&self->request_manager);
	g_clear_object(&self->chip_manager);
	g_clear_object(&self->udev);

	G_OBJECT_CLASS(gpiodbus_daemon_parent_class)->finalize(obj);
}

static void gpiodbus_daemon_class_init(GPIODBusDaemonClass *daemon_class)
{
	GObjectClass *class = G_OBJECT_CLASS(daemon_class);

	class->dispose = gpiodbus_daemon_dispose;
	class->finalize = gpiodbus_daemon_finalize;
}

static gboolean
gpiodbus_remove_request_if_chip_matches(gpointer key G_GNUC_UNUSED,
					gpointer value, gpointer user_data)
{
	GPIODBusDaemonChipData *chip_data = user_data;
	GPIODBusDaemonRequestData *req_data = value;

	return req_data->chip_data == chip_data;
}

static void gpiodbus_daemon_chip_data_free(gpointer data)
{
	GPIODBusDaemonChipData *chip_data = data;
	const gchar *obj_path;

	obj_path = g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(chip_data->dbus_chip));

	g_debug("unexporting object for GPIO chip: '%s'", obj_path);

	g_hash_table_foreach_remove(chip_data->daemon->requests,
				    gpiodbus_remove_request_if_chip_matches,
				    chip_data);

	g_dbus_object_manager_server_unexport(chip_data->daemon->chip_manager,
					      obj_path);

	g_hash_table_unref(chip_data->lines);
	g_object_unref(chip_data->line_manager);
	g_object_unref(chip_data->chip);
	g_object_unref(chip_data->dbus_chip);
	g_free(chip_data);
}

static void gpiodbus_daemon_line_data_free(gpointer data)
{
	GPIODBusDaemonLineData *line_data = data;
	const gchar *obj_path;

	obj_path = g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(line_data->dbus_line));

	g_debug("unexporting object for GPIO line: '%s'",
		obj_path);

	g_dbus_object_manager_server_unexport(
				line_data->chip_data->line_manager, obj_path);

	g_object_unref(line_data->dbus_line);
	g_free(line_data);
}

static void gpiodbus_lines_set_managed(GPIODBusDaemonRequestData *req_data,
				       gboolean managed)
{
	g_autoptr(GDBusObject) obj = NULL;
	const gchar *const *line_paths;
	GPIODBusLine *line;
	const gchar *path;
	guint i;

	line_paths = gpiodbus_request_get_line_paths(req_data->dbus_request);

	for (path = line_paths[0], i = 0; path; path = line_paths[++i]) {
		obj = g_dbus_object_manager_get_object(
			G_DBUS_OBJECT_MANAGER(
				req_data->chip_data->line_manager), path);
		line = gpiodbus_object_peek_line(GPIODBUS_OBJECT(obj));

		g_debug("Setting line %u on chip object '%s' to '%s'",
			gpiodbus_line_get_offset(line),
			g_dbus_interface_skeleton_get_object_path(
				G_DBUS_INTERFACE_SKELETON(
					req_data->chip_data->dbus_chip)),
			managed ? "managed" : "unmanaged");

		gpiodbus_line_set_managed(line, managed);
		gpiodbus_line_set_request_path(line,
			managed ? g_dbus_interface_skeleton_get_object_path(
				G_DBUS_INTERFACE_SKELETON(
					req_data->dbus_request)) : NULL);

	}
}

static void gpiodbus_daemon_request_data_free(gpointer data)
{
	GPIODBusDaemonRequestData *req_data = data;
	const gchar *obj_path;

	obj_path = g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(req_data->dbus_request));

	g_debug("unexporting object for GPIO request: '%s'", obj_path);

	g_dbus_object_manager_server_unexport(
		req_data->chip_data->daemon->request_manager, obj_path);

	gpiodbus_lines_set_managed(req_data, FALSE);
	gpiodbus_id_free(req_data->chip_data->daemon->req_id_root,
			 req_data->id);
	g_object_unref(req_data->request);
	g_object_unref(req_data->dbus_request);
	g_free(req_data);
}

static void gpiodbus_daemon_init(GPIODBusDaemon *self)
{
	g_debug("initializing GPIO DBus daemon");

	self->con = NULL;
	self->udev = g_udev_client_new(gpiodbus_daemon_udev_subsystems);
	self->chip_manager =
			g_dbus_object_manager_server_new("/io/gpiod1/chips");
	self->request_manager =
			g_dbus_object_manager_server_new("/io/gpiod1/requests");
	self->chips = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
					    gpiodbus_daemon_chip_data_free);
	self->requests = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
					gpiodbus_daemon_request_data_free);
	self->req_id_root = g_tree_new_full(gpiodbus_id_cmp, NULL,
					    g_free, NULL);
}

GPIODBusDaemon *gpiodbus_daemon_new(void)
{
	return GPIODBUS_DAEMON(g_object_new(GPIODBUS_DAEMON_TYPE, NULL));
}

static void gpiodbus_daemon_on_info_event(GPIODChip *chip G_GNUC_UNUSED,
					  GPIODInfoEvent *event,
					  gpointer data)
{
	GPIODBusDaemonChipData *chip_data = data;
	g_autoptr(GPIODLineInfo) info = NULL;
	GPIODBusDaemonLineData *line_data;
	guint offset;

	info = g_gpiod_info_event_get_line_info(event);
	offset = g_gpiod_line_info_get_offset(info);

	g_debug("line info event received for offset %u on chip '%s'",
		offset,
		g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(chip_data->dbus_chip)));

	line_data = g_hash_table_lookup(chip_data->lines,
					GINT_TO_POINTER(offset));
	if (!line_data)
		g_error("failed to retrieve line data - programming bug?");

	gpiodbus_line_set_props(line_data->dbus_line, info);
}

static void gpiodbus_daemon_export_line(GPIODBusDaemon *self,
					GPIODBusDaemonChipData *chip_data,
					GPIODLineInfo *info)
{
	g_autofree GPIODBusDaemonLineData *line_data = NULL;
	g_autoptr(GPIODBusObjectSkeleton) skeleton = NULL;
	g_autoptr(GPIODBusLine) dbus_line = NULL;
	g_autofree gchar *obj_path = NULL;
	const gchar *obj_prefix;
	guint line_offset;
	gboolean ret;

	obj_prefix = g_dbus_object_manager_get_object_path(
				G_DBUS_OBJECT_MANAGER(chip_data->line_manager));
	line_offset = g_gpiod_line_info_get_offset(info);
	dbus_line = gpiodbus_line_skeleton_new();
	obj_path = g_strdup_printf("%s/line%u", obj_prefix, line_offset);

	gpiodbus_line_set_props(dbus_line, info);

	skeleton = gpiodbus_object_skeleton_new(obj_path);
	gpiodbus_object_skeleton_set_line(skeleton, GPIODBUS_LINE(dbus_line));

	g_debug("exporting object for GPIO line: '%s'", obj_path);

	g_dbus_object_manager_server_export(chip_data->line_manager,
					    G_DBUS_OBJECT_SKELETON(skeleton));
	g_dbus_object_manager_server_set_connection(chip_data->line_manager,
						    self->con);

	line_data = g_malloc0(sizeof(*line_data));
	line_data->dbus_line = g_steal_pointer(&dbus_line);
	line_data->chip_data = chip_data;

	ret = g_hash_table_insert(chip_data->lines,
				  GUINT_TO_POINTER(line_offset),
				  g_steal_pointer(&line_data));
	/* It's a programming bug if the line is already in the hashmap. */
	g_assert(ret);
}

static gboolean gpiodbus_daemon_export_lines(GPIODBusDaemon *self,
					     GPIODBusDaemonChipData *chip_data)
{
	g_autoptr(GPIODChipInfo) chip_info = NULL;
	GPIODChip *chip = chip_data->chip;
	g_autoptr(GError) err = NULL;
	guint i, num_lines;
	gint j;

	chip_info = g_gpiod_chip_get_info(chip, &err);
	if (!chip_info) {
		g_critical("failed to read chip info: %s", err->message);
		return FALSE;
	}

	num_lines = g_gpiod_chip_info_get_num_lines(chip_info);

	g_signal_connect(chip, "info-event",
			 G_CALLBACK(gpiodbus_daemon_on_info_event), chip_data);

	for (i = 0; i < num_lines; i++) {
		g_autoptr(GPIODLineInfo) linfo = NULL;

		linfo = g_gpiod_chip_watch_line_info(chip, i, &err);
		if (!linfo) {
			g_critical("failed to setup a line-info watch: %s",
				   err->message);
			for (j = i; j >= 0; j--)
				g_gpiod_chip_unwatch_line_info(chip, i, NULL);
			return FALSE;
		}

		gpiodbus_daemon_export_line(self, chip_data, linfo);
	}

	return TRUE;
}

static gboolean
gpiodbus_daemon_handle_release_lines(GPIODBusRequest *request,
				     GDBusMethodInvocation *invocation,
				     gpointer user_data)
{
	GPIODBusDaemonRequestData *req_data = user_data;
	g_autofree gchar *obj_path = NULL;
	gboolean ret;

	obj_path = g_strdup(g_dbus_interface_skeleton_get_object_path(
					G_DBUS_INTERFACE_SKELETON(request)));

	g_debug("release call received on request '%s'", obj_path);

	ret = g_hash_table_remove(req_data->chip_data->daemon->requests,
				  obj_path);
	/* It's a programming bug if the request was not in the hashmap. */
	if (!ret)
		g_warning("request '%s' is not registered - logic error?",
			  obj_path);

	g_dbus_method_invocation_return_value(invocation, NULL);

	return G_SOURCE_CONTINUE;
}

static gboolean
gpiodbus_daemon_handle_reconfigure_lines(GPIODBusRequest *request,
					 GDBusMethodInvocation *invocation,
					 GVariant *arg_line_cfg,
					 gpointer user_data)
{
	GPIODBusDaemonRequestData *req_data = user_data;
	g_autoptr(GPIODLineConfig) line_cfg = NULL;
	g_autofree gchar *line_cfg_str = NULL;
	g_autoptr(GError) err = NULL;
	const gchar *obj_path;
	gboolean ret;

	obj_path = g_dbus_interface_skeleton_get_object_path(
					G_DBUS_INTERFACE_SKELETON(request));
	line_cfg_str = g_variant_print(arg_line_cfg, FALSE);

	g_debug("reconfigure call received on request '%s', line config: %s",
		obj_path, line_cfg_str);

	line_cfg = gpiodbus_line_config_from_variant(arg_line_cfg);
	if (!line_cfg) {
		g_critical("failed to convert method call arguments '%s' to line config",
			   line_cfg_str);
		g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR,
						      G_DBUS_ERROR_INVALID_ARGS,
						      "Invalid line configuration");
		goto out;
	}

	ret = g_gpiod_line_request_reconfigure_lines(req_data->request,
						     line_cfg, &err);
	if (!ret) {
		g_critical("failed to reconfigure GPIO lines on request '%s': %s",
			   obj_path, err->message);
		g_dbus_method_invocation_return_dbus_error(invocation,
						"io.gpiod1.ReconfigureFailed",
						err->message);
		goto out;
	}

	g_dbus_method_invocation_return_value(invocation, NULL);

out:
	return G_SOURCE_CONTINUE;
}

static gboolean
gpiodbus_daemon_handle_get_values(GPIODBusRequest *request,
				  GDBusMethodInvocation *invocation,
				  GVariant *arg_offsets, gpointer user_data)
{
	GPIODBusDaemonRequestData *req_data = user_data;
	g_autoptr(GVariant) out_values = NULL;
	g_autofree gchar *offsets_str = NULL;
	g_autoptr(GVariant) response = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GArray) values = NULL;
	g_autoptr(GError) err = NULL;
	GVariantBuilder builder;
	const gchar *obj_path;
	GVariantIter iter;
	gsize num_offsets;
	guint offset, i;
	gboolean ret;

	obj_path = g_dbus_interface_skeleton_get_object_path(
					G_DBUS_INTERFACE_SKELETON(request));
	offsets_str = g_variant_print(arg_offsets, FALSE);
	num_offsets = g_variant_n_children(arg_offsets);

	g_debug("get-values call received on request '%s' for offsets: %s",
		obj_path, offsets_str);

	if (num_offsets == 0) {
		ret = g_gpiod_line_request_get_values(req_data->request,
						      &values, &err);
	} else {
		offsets = g_array_sized_new(FALSE, TRUE, sizeof(offset),
					    num_offsets);
		g_variant_iter_init(&iter, arg_offsets);
		while (g_variant_iter_next(&iter, "u", &offset))
			g_array_append_val(offsets, offset);

		ret = g_gpiod_line_request_get_values_subset(req_data->request,
							     offsets, &values,
							     &err);
	}
	if (!ret) {
		g_critical("failed to get GPIO line values on request '%s': %s",
			   obj_path, err->message);
		g_dbus_method_invocation_return_dbus_error(invocation,
						"io.gpiod1.GetValuesFailed",
						err->message);
		goto out;
	}

	g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);
	for (i = 0; i < values->len; i++)
		g_variant_builder_add(&builder, "i",
				      g_array_index(values, gint, i));
	out_values = g_variant_ref_sink(g_variant_builder_end(&builder));

	g_variant_builder_init(&builder, G_VARIANT_TYPE_TUPLE);
	g_variant_builder_add_value(&builder, out_values);
	response = g_variant_ref_sink(g_variant_builder_end(&builder));

	g_dbus_method_invocation_return_value(invocation, response);

out:
	return G_SOURCE_CONTINUE;
}

static void
gpiodbus_daemon_on_edge_event(GPIODLineRequest *request G_GNUC_UNUSED,
			      GPIODEdgeEvent *event, gpointer user_data)
{
	GPIODBusDaemonRequestData *req_data = user_data;
	GPIODBusDaemonLineData *line_data;
	gulong line_seqno, global_seqno;
	GPIODEdgeEventType edge;
	guint64 timestamp;
	guint offset;
	gint val;

	edge = g_gpiod_edge_event_get_event_type(event);
	offset = g_gpiod_edge_event_get_line_offset(event);
	timestamp = g_gpiod_edge_event_get_timestamp_ns(event);
	global_seqno = g_gpiod_edge_event_get_global_seqno(event);
	line_seqno = g_gpiod_edge_event_get_line_seqno(event);

	val = edge == G_GPIOD_EDGE_EVENT_RISING_EDGE ? 1 : 0;

	g_debug("%s edge event received for offset %u on request '%s'",
		val ? "rising" : "falling", offset,
		g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(req_data->dbus_request)));

	line_data = g_hash_table_lookup(req_data->chip_data->lines,
					GINT_TO_POINTER(offset));
	if (!line_data)
		g_error("failed to retrieve line data - programming bug?");

	gpiodbus_line_emit_edge_event(line_data->dbus_line,
				      g_variant_new("(ittt)", val, timestamp,
						    global_seqno, line_seqno));
}

static void
gpiodbus_daemon_export_request(GPIODBusDaemon *self, GPIODLineRequest *request,
			       GPIODBusDaemonChipData *chip_data, gint id)
{
	g_autofree GPIODBusDaemonRequestData *req_data = NULL;
	g_autoptr(GPIODBusObjectSkeleton) skeleton = NULL;
	g_autoptr(GPIODBusRequest) dbus_req = NULL;
	g_autofree gchar *obj_path = NULL;
	gboolean ret;

	dbus_req = gpiodbus_request_skeleton_new();
	obj_path = g_strdup_printf("/io/gpiod1/requests/request%d", id);

	gpiodbus_request_set_props(dbus_req, request, chip_data->dbus_chip,
				G_DBUS_OBJECT_MANAGER(chip_data->line_manager));

	skeleton = gpiodbus_object_skeleton_new(obj_path);
	gpiodbus_object_skeleton_set_request(skeleton,
					     GPIODBUS_REQUEST(dbus_req));

	g_debug("exporting object for GPIO request: '%s'", obj_path);

	g_dbus_object_manager_server_export(self->request_manager,
					    G_DBUS_OBJECT_SKELETON(skeleton));

	req_data = g_malloc0(sizeof(*req_data));
	req_data->chip_data = chip_data;
	req_data->dbus_request = g_steal_pointer(&dbus_req);
	req_data->id = id;
	req_data->request = g_object_ref(request);

	g_signal_connect(req_data->dbus_request, "handle-release",
			 G_CALLBACK(gpiodbus_daemon_handle_release_lines),
			 req_data);
	g_signal_connect(req_data->dbus_request, "handle-reconfigure-lines",
			 G_CALLBACK(gpiodbus_daemon_handle_reconfigure_lines),
			 req_data);
	g_signal_connect(req_data->dbus_request, "handle-get-values",
			 G_CALLBACK(gpiodbus_daemon_handle_get_values),
			 req_data);
	g_signal_connect(req_data->request, "edge-event",
			 G_CALLBACK(gpiodbus_daemon_on_edge_event), req_data);

	gpiodbus_lines_set_managed(req_data, TRUE);

	ret = g_hash_table_insert(self->requests, g_steal_pointer(&obj_path),
				  g_steal_pointer(&req_data));
	/* It's a programming bug if the request is already in the hashmap. */
	g_assert(ret);
}

static gboolean
gpiodbus_daemon_handle_request_lines(GPIODBusChip *chip,
				     GDBusMethodInvocation *invocation,
				     GVariant *arg_line_cfg,
				     GVariant *arg_req_cfg,
				     gpointer user_data)
{
	GPIODBusDaemonChipData *chip_data = user_data;
	g_autoptr(GPIODRequestConfig) req_cfg = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineConfig) line_cfg = NULL;
	g_autofree gchar *line_cfg_str = NULL;
	g_autofree gchar *req_cfg_str = NULL;
	g_autofree gchar *response = NULL;
	g_autoptr(GError) err = NULL;
	const gchar *obj_path;
	guint id;

	obj_path = g_dbus_interface_skeleton_get_object_path(
			G_DBUS_INTERFACE_SKELETON(chip));
	line_cfg_str = g_variant_print(arg_line_cfg, FALSE);
	req_cfg_str = g_variant_print(arg_req_cfg, FALSE);

	g_debug("line request received on chip '%s', line config: %s, request_config: %s",
		obj_path, line_cfg_str, req_cfg_str);

	line_cfg = gpiodbus_line_config_from_variant(arg_line_cfg);
	if (!line_cfg) {
		g_critical("failed to convert method call arguments '%s' to line config",
			   line_cfg_str);
		g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR,
						      G_DBUS_ERROR_INVALID_ARGS,
						      "Invalid line configuration");
		goto out;
	}

	req_cfg = gpiodbus_request_config_from_variant(arg_req_cfg);
	if (!req_cfg) {
		g_critical("failed to convert method call arguments '%s' to request config",
			   req_cfg_str);
		g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR,
						      G_DBUS_ERROR_INVALID_ARGS,
						      "Invalid request configuration");
		goto out;
	}

	request = g_gpiod_chip_request_lines(chip_data->chip, req_cfg, line_cfg,
					     &err);
	if (err) {
		g_critical("failed to request GPIO lines on chip '%s': %s",
			   obj_path, err->message);
		g_dbus_method_invocation_return_dbus_error(invocation,
				"io.gpiod1.RequestFailed", err->message);
		goto out;
	}

	g_debug("line request succeeded on chip '%s'", obj_path);

	id = gpiodbus_id_alloc(chip_data->daemon->req_id_root);
	gpiodbus_daemon_export_request(chip_data->daemon, request,
				       chip_data, id);

	response = g_strdup_printf("/io/gpiod1/requests/request%d", id);
	g_dbus_method_invocation_return_value(invocation,
					      g_variant_new("(o)", response));

out:
	return G_SOURCE_CONTINUE;
}

static void gpiodbus_daemon_export_chip(GPIODBusDaemon *self, GUdevDevice *dev)
{
	g_autofree GPIODBusDaemonChipData *chip_data = NULL;
	g_autoptr(GDBusObjectManagerServer) manager = NULL;
	g_autoptr(GPIODBusObjectSkeleton) skeleton = NULL;
	const gchar *devname, *devpath, *obj_prefix;
	g_autoptr(GPIODBusChip) dbus_chip = NULL;
	g_autoptr(GHashTable) lines = NULL;
	g_autofree gchar *obj_path = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GError) err = NULL;
	gboolean ret;

	devname = g_udev_device_get_name(dev);
	devpath = g_udev_device_get_device_file(dev);
	obj_prefix = g_dbus_object_manager_get_object_path(
				G_DBUS_OBJECT_MANAGER(self->chip_manager));

	chip = g_gpiod_chip_new(devpath, &err);
	if (!chip) {
		g_critical("failed to open GPIO chip %s: %s",
			   devpath, err->message);
		return;
	}

	dbus_chip = gpiodbus_chip_skeleton_new();
	obj_path = g_strdup_printf("%s/%s", obj_prefix, devname);

	ret = gpiodbus_chip_set_props(dbus_chip, chip, &err);
	if (!ret) {
		g_critical("failed to set chip properties: %s", err->message);
		return;
	}

	skeleton = gpiodbus_object_skeleton_new(obj_path);
	gpiodbus_object_skeleton_set_chip(skeleton, GPIODBUS_CHIP(dbus_chip));

	g_debug("exporting object for GPIO chip: '%s'", obj_path);

	g_dbus_object_manager_server_export(self->chip_manager,
					    G_DBUS_OBJECT_SKELETON(skeleton));

	lines = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL,
				      gpiodbus_daemon_line_data_free);
	manager = g_dbus_object_manager_server_new(obj_path);

	chip_data = g_malloc0(sizeof(*chip_data));
	chip_data->daemon = self;
	chip_data->chip = g_steal_pointer(&chip);
	chip_data->dbus_chip = g_steal_pointer(&dbus_chip);
	chip_data->lines = g_steal_pointer(&lines);
	chip_data->line_manager = g_steal_pointer(&manager);

	ret = gpiodbus_daemon_export_lines(self, chip_data);
	if (!ret) {
		g_dbus_object_manager_server_unexport(self->chip_manager,
						      obj_path);
		return;
	}

	g_signal_connect(chip_data->dbus_chip, "handle-request-lines",
			 G_CALLBACK(gpiodbus_daemon_handle_request_lines),
			 chip_data);

	ret = g_hash_table_insert(self->chips, g_strdup(devname),
				  g_steal_pointer(&chip_data));
	/* It's a programming bug if the chip is already in the hashmap. */
	g_assert(ret);
}

static void gpiodbus_daemon_unexport_chip(GPIODBusDaemon *self,
					  GUdevDevice *dev)
{
	const gchar *name = g_udev_device_get_name(dev);
	gboolean ret;

	ret = g_hash_table_remove(self->chips, name);
	/* It's a programming bug if the chip was not in the hashmap. */
	if (!ret)
		g_warning("chip '%s' is not registered - exporting failed?",
			  name);
}

/*
 * We can get two uevents per action per gpiochip. One is for the new-style
 * character device, the other for legacy sysfs devices. We are only concerned
 * with the former, which we can tell from the latter by the presence of
 * the device file.
 */
static gboolean gpiodbus_daemon_is_gpiochip_device(GUdevDevice *dev)
{
	return g_udev_device_get_device_file(dev) != NULL;
}

static void gpiodbus_daemon_on_uevent(GUdevClient *udev G_GNUC_UNUSED,
				      const gchar *action, GUdevDevice *dev,
				      gpointer data)
{
	GPIODBusDaemon *self = data;

	if (!gpiodbus_daemon_is_gpiochip_device(dev))
		return;

	g_debug("uevent: %s action on %s device",
		action, g_udev_device_get_name(dev));

	if (g_strcmp0(action, "bind") == 0)
		gpiodbus_daemon_export_chip(self, dev);
	else if (g_strcmp0(action, "unbind") == 0)
		gpiodbus_daemon_unexport_chip(self, dev);
}

static void gpiodbus_daemon_process_chip_dev(gpointer data, gpointer user_data)
{
	GPIODBusDaemon *daemon = user_data;
	GUdevDevice *dev = data;

	if (gpiodbus_daemon_is_gpiochip_device(dev))
		gpiodbus_daemon_export_chip(daemon, dev);
}

void gpiodbus_daemon_start(GPIODBusDaemon *self, GDBusConnection *con)
{
	g_autolist(GUdevDevice) devs = NULL;

	g_assert(self);
	g_assert(!self->con); /* Don't allow to call this twice. */

	self->con = g_object_ref(con);

	/* Subscribe for GPIO uevents. */
	g_signal_connect(self->udev, "uevent",
			 G_CALLBACK(gpiodbus_daemon_on_uevent), self);

	devs = g_udev_client_query_by_subsystem(self->udev, "gpio");
	g_list_foreach(devs, gpiodbus_daemon_process_chip_dev, self);

	g_dbus_object_manager_server_set_connection(self->chip_manager,
						    self->con);
	g_dbus_object_manager_server_set_connection(self->request_manager,
						    self->con);

	g_debug("GPIO daemon now listening");
}
