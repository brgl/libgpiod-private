// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODChip {
	GObject parent_instance;
	GString *path;
	GError *construct_err;
	struct gpiod_chip *handle;
	GSource *info_event_src;
	guint info_event_src_id;
};

enum {
	G_GPIOD_CHIP_PROP_PATH = 1,
	G_GPIOD_CHIP_PROP_HANDLE,
};

enum {
	G_GPIOD_CHIP_SIGNAL_INFO_EVENT,
	G_GPIOD_CHIP_SIGNAL_LAST,
};

static guint signals[G_GPIOD_CHIP_SIGNAL_LAST];

static void g_string_free_complete(GString *str)
{
	g_string_free(str, TRUE);
}

static gboolean g_gpiod_chip_on_info_event(GIOChannel *source G_GNUC_UNUSED,
					   GIOCondition condition G_GNUC_UNUSED,
					   gpointer data)
{
	g_autoptr(GPIODInfoEvent) event = NULL;
	struct gpiod_info_event *event_handle;
	GPIODChip *self = data;

	event_handle = gpiod_chip_read_info_event(self->handle);
	if (!event_handle)
		return TRUE;

	event = G_GPIOD_INFO_EVENT_OBJ(g_object_new(G_GPIOD_INFO_EVENT_TYPE,
						    "handle", event_handle,
						    NULL));

	g_signal_emit(self,
		      signals[G_GPIOD_CHIP_SIGNAL_INFO_EVENT],
		      0,
		      event);

	return TRUE;
}

static gboolean
g_gpiod_chip_initable_init(GInitable *initable,
			   GCancellable *cancellable G_GNUC_UNUSED,
			   GError **err)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(initable);

	if (self->construct_err) {
		g_propagate_error(err, self->construct_err);
		self->construct_err = NULL;
		return FALSE;
	}

	return TRUE;
}

static void g_gpiod_chip_initable_iface_init(GInitableIface *iface)
{
	iface->init = g_gpiod_chip_initable_init;
}

G_DEFINE_TYPE_WITH_CODE(GPIODChip, g_gpiod_chip, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(
				G_TYPE_INITABLE,
				g_gpiod_chip_initable_iface_init));

static void g_gpiod_chip_constructed(GObject *obj)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(obj);
	g_autoptr(GIOChannel) channel = NULL;

	g_assert(!self->handle);
	g_assert(self->path);

	self->handle = gpiod_chip_open(self->path->str);
	if (!self->handle) {
		g_gpiod_set_error_from_errno(&self->construct_err,
					     "unable to open GPIO chip '%s'",
					     self->path->str);
		return;
	}

	channel = g_io_channel_unix_new(gpiod_chip_get_fd(self->handle));
	self->info_event_src = g_io_create_watch(channel, G_IO_IN);
	g_source_set_callback(self->info_event_src,
			      G_SOURCE_FUNC(g_gpiod_chip_on_info_event),
			      self, NULL);
	self->info_event_src_id = g_source_attach(self->info_event_src, NULL);

	G_OBJECT_CLASS(g_gpiod_chip_parent_class)->constructed(obj);
}

static void g_gpiod_chip_get_property(GObject *obj, guint prop_id,
				      GValue *val, GParamSpec *pspec)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_CHIP_PROP_PATH:
		g_value_set_static_string(val, self->path->str);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_chip_set_property(GObject *obj, guint prop_id,
				      const GValue *val, GParamSpec *pspec)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_CHIP_PROP_PATH:
		self->path = g_string_new(g_value_get_string(val));
		break;
	case G_GPIOD_CHIP_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

void g_gpiod_chip_close(GPIODChip *self)
{
	g_clear_pointer(&self->info_event_src, g_source_unref);
	g_clear_pointer(&self->handle, gpiod_chip_close);
}

static void g_gpiod_chip_dispose(GObject *obj)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(obj);

	if (self->info_event_src_id)
		g_source_remove(self->info_event_src_id);

	g_gpiod_chip_close(self);

	G_OBJECT_CLASS(g_gpiod_chip_parent_class)->dispose(obj);
}

static void g_gpiod_chip_finalize(GObject *obj)
{
	GPIODChip *self = G_GPIOD_CHIP_OBJ(obj);

	g_clear_error(&self->construct_err);
	g_clear_pointer(&self->path, g_string_free_complete);

	G_OBJECT_CLASS(g_gpiod_chip_parent_class)->finalize(obj);
}

static void g_gpiod_chip_class_init(GPIODChipClass *chip_class)
{
	GObjectClass *class = G_OBJECT_CLASS(chip_class);

	class->constructed = g_gpiod_chip_constructed;
	class->get_property = g_gpiod_chip_get_property;
	class->set_property = g_gpiod_chip_set_property;
	class->dispose = g_gpiod_chip_dispose;
	class->finalize = g_gpiod_chip_finalize;

	g_object_class_install_property(class, G_GPIOD_CHIP_PROP_PATH,
		g_param_spec_string("path", "Path",
			"Path to the GPIO chip device used to create this chip.",
			NULL, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

	g_object_class_install_property(class, G_GPIOD_CHIP_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"Open GPIO chip handle as returned by gpiod_chip_open().",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	signals[G_GPIOD_CHIP_SIGNAL_INFO_EVENT] =
				g_signal_new("info-event",
					     G_TYPE_FROM_CLASS(chip_class),
					     G_SIGNAL_RUN_LAST,
					     0,
					     NULL,
					     NULL,
					     g_cclosure_marshal_generic,
					     G_TYPE_NONE,
					     1,
					     G_GPIOD_INFO_EVENT_TYPE);
}

static void g_gpiod_chip_init(GPIODChip *self)
{
	self->path = NULL;
	self->construct_err = NULL;
	self->handle = NULL;
	self->info_event_src = NULL;
	self->info_event_src_id = 0;
}

GPIODChip *g_gpiod_chip_new(const gchar *path, GError **err)
{
	return G_GPIOD_CHIP_OBJ(g_initable_new(G_GPIOD_CHIP_TYPE, NULL, err,
					       "path", path, NULL));
}

gboolean g_gpiod_chip_is_closed(GPIODChip *self)
{
	return !self->handle;
}

const gchar *g_gpiod_chip_get_path(GPIODChip *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "path");
}

static void set_err_chip_closed(GError **err)
{
	g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_CHIP_CLOSED,
		    "Chip was closed and cannot be used");
}

GPIODChipInfo *g_gpiod_chip_get_info(GPIODChip *self, GError **err)
{
	struct gpiod_chip_info *info;

	g_assert(self);

	if (g_gpiod_chip_is_closed(self)) {
		set_err_chip_closed(err);
		return NULL;
	}

	info = gpiod_chip_get_info(self->handle);
	if (!info) {
		g_gpiod_set_error_from_errno(err,
			"unable to retrieve GPIO chip information");
		return NULL;
	}

	return G_GPIOD_CHIP_INFO_OBJ(g_object_new(G_GPIOD_CHIP_INFO_TYPE,
						  "handle", info, NULL));
}

static GPIODLineInfo *
g_gpiod_chip_do_get_line_info(GPIODChip *self, guint offset, GError **err,
			struct gpiod_line_info *(*func)(struct gpiod_chip *,
							unsigned int),
			const gchar *err_action)
{
	struct gpiod_line_info *info;

	g_assert(self);

	if (g_gpiod_chip_is_closed(self)) {
		set_err_chip_closed(err);
		return NULL;
	}

	info = func(self->handle, offset);
	if (!info) {
		g_gpiod_set_error_from_errno(err, "unable to %s for offset %u",
					     err_action, offset);
		return NULL;
	}

	return G_GPIOD_LINE_INFO_OBJ(g_object_new(G_GPIOD_LINE_INFO_TYPE,
						  "handle", info, NULL));
		
}

GPIODLineInfo *
g_gpiod_chip_get_line_info(GPIODChip *self, guint offset, GError **err)
{
	return g_gpiod_chip_do_get_line_info(self, offset, err,
					     gpiod_chip_get_line_info,
					     "retrieve GPIO line-info");
}

GPIODLineInfo *
g_gpiod_chip_watch_line_info(GPIODChip *self, guint offset, GError **err)
{
	return g_gpiod_chip_do_get_line_info(self, offset, err,
					     gpiod_chip_watch_line_info,
					     "setup a line-info watch");
}

gboolean
g_gpiod_chip_unwatch_line_info(GPIODChip *self, guint offset, GError **err)
{
	int ret;

	g_assert(self);

	if (g_gpiod_chip_is_closed(self)) {
		set_err_chip_closed(err);
		return FALSE;
	}

	ret = gpiod_chip_unwatch_line_info(self->handle, offset);
	if (ret) {
		g_gpiod_set_error_from_errno(err,
			    "unable to unwatch line-info events for offset %u",
			    offset);
		return FALSE;
	}

	return TRUE;
}

gboolean
g_gpiod_chip_get_line_offset_from_name(GPIODChip *self, const gchar *name,
				       guint *offset, GError **err)
{
	gint ret;

	g_assert(self);

	if (g_gpiod_chip_is_closed(self)) {
		set_err_chip_closed(err);
		return FALSE;
	}

	if (!name) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "name must not be NULL");
		return FALSE;
	}

	ret = gpiod_chip_get_line_offset_from_name(self->handle, name);
	if (ret < 0) {
		if (errno != ENOENT)
			g_gpiod_set_error_from_errno(err,
				    "failed to map line name to offset");
		else
			errno = 0;

		return FALSE;
	}

	if (offset)
		*offset = ret;

	return TRUE;
}

GPIODLineRequest *g_gpiod_chip_request_lines(GPIODChip *self,
					     GPIODRequestConfig *req_cfg,
					     GPIODLineConfig *line_cfg,
					     GError **err)
{
	struct gpiod_request_config *req_cfg_handle;
	struct gpiod_line_config *line_cfg_handle;
	struct gpiod_line_request *req;

	g_assert(self);

	if (g_gpiod_chip_is_closed(self)) {
		set_err_chip_closed(err);
		return NULL;
	}

	if (!line_cfg) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "line-config is required for request");
		return NULL;
	}

	req_cfg_handle = req_cfg ?
		g_gpiod_get_prop_pointer(G_OBJECT(req_cfg), "handle") : NULL;
	line_cfg_handle = g_gpiod_get_prop_pointer(G_OBJECT(line_cfg),
						   "handle");

	req = gpiod_chip_request_lines(self->handle,
				       req_cfg_handle, line_cfg_handle);
	if (!req) {
		g_gpiod_set_error_from_errno(err,
				"failed to request GPIO lines");
		return NULL;
	}

	return G_GPIOD_LINE_REQUEST_OBJ(g_object_new(G_GPIOD_LINE_REQUEST_TYPE,
						     "handle", req, NULL));
}
