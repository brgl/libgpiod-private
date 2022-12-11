// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

static const gsize event_buf_size = 64;

struct _GPIODLineRequest {
	GObject parent_instance;
	struct gpiod_line_request *handle;
	struct gpiod_edge_event_buffer *event_buf;
	GSource *edge_event_src;
	guint edge_event_src_id;
	enum gpiod_line_value *val_buf;
};

enum {
	G_GPIOD_LINE_REQUEST_PROP_HANDLE = 1,
	G_GPIOD_LINE_REQUEST_PROP_CHIP_NAME,
	G_GPIOD_LINE_REQUEST_PROP_REQUESTED_OFFSETS,
};

enum {
	G_GPIOD_LINE_REQUEST_SIGNAL_EDGE_EVENT,
	G_GPIOD_LINE_REQUEST_SIGNAL_LAST,
};

static guint signals[G_GPIOD_LINE_REQUEST_SIGNAL_LAST];

G_DEFINE_TYPE(GPIODLineRequest, g_gpiod_line_request, G_TYPE_OBJECT);

static gboolean
g_gpiod_line_request_on_edge_event(GIOChannel *source G_GNUC_UNUSED,
				   GIOCondition condition G_GNUC_UNUSED,
				   gpointer data)
{
	struct gpiod_edge_event *event_handle, *event_copy;
	GPIODLineRequest *self = data;
	gint ret, i;

	ret = gpiod_line_request_read_edge_events(self->handle,
						  self->event_buf,
						  event_buf_size);
	if (ret < 0)
		return TRUE;

	for (i = 0; i < ret; i++) {
		g_autoptr(GPIODEdgeEvent) event = NULL;

		event_handle = gpiod_edge_event_buffer_get_event(
						self->event_buf, i);
		event_copy = gpiod_edge_event_copy(event_handle);
		if (!event_copy)
			g_error("failed to copy the edge event");

		event = G_GPIOD_EDGE_EVENT_OBJ(
				g_object_new(G_GPIOD_EDGE_EVENT_TYPE,
					     "handle", event_copy, NULL));

		g_signal_emit(self,
			      signals[G_GPIOD_LINE_REQUEST_SIGNAL_EDGE_EVENT],
			      0,
			      event);
	}

	return TRUE;
}

static void g_gpiod_line_request_constructed(GObject *obj)
{
	GPIODLineRequest *self = G_GPIOD_LINE_REQUEST_OBJ(obj);
	g_autoptr(GIOChannel) channel = NULL;
	gsize num_lines;

	self->event_buf = gpiod_edge_event_buffer_new(event_buf_size);
	if (!self->event_buf)
		g_error("failed to allocate the edge event buffer");

	channel = g_io_channel_unix_new(
			gpiod_line_request_get_fd(self->handle));
	self->edge_event_src = g_io_create_watch(channel, G_IO_IN);
	g_source_set_callback(self->edge_event_src,
			      G_SOURCE_FUNC(g_gpiod_line_request_on_edge_event),
			      self, NULL);
	self->edge_event_src_id = g_source_attach(self->edge_event_src, NULL);

	num_lines = gpiod_line_request_get_num_requested_lines(self->handle);
	self->val_buf = g_malloc0(sizeof(enum gpiod_line_value) * num_lines);

	G_OBJECT_CLASS(g_gpiod_line_request_parent_class)->constructed(obj);
}

static void g_gpiod_line_request_get_property(GObject *obj, guint prop_id,
						GValue *val, GParamSpec *pspec)
{
	GPIODLineRequest *self = G_GPIOD_LINE_REQUEST_OBJ(obj);
	g_autofree guint *offsets = NULL;
	gsize num_offsets;
	GArray *boxed;

	switch (prop_id) {
	case G_GPIOD_LINE_REQUEST_PROP_CHIP_NAME:
		g_value_set_static_string(val,
			gpiod_line_request_get_chip_name(self->handle));
		break;
	case G_GPIOD_LINE_REQUEST_PROP_REQUESTED_OFFSETS:
		boxed = g_array_new(FALSE, TRUE, sizeof(guint));

		if (!g_gpiod_line_request_is_released(self)) {
			num_offsets =
				gpiod_line_request_get_num_requested_lines(
								self->handle);
			offsets = g_malloc0(num_offsets * sizeof(guint));
			gpiod_line_request_get_requested_offsets(self->handle,
								 offsets,
								 num_offsets);
			g_array_append_vals(boxed, offsets, num_offsets);
		}

		g_value_set_boxed(val, boxed);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_request_set_property(GObject *obj, guint prop_id,
					       const GValue *val, GParamSpec *pspec)
{
	GPIODLineRequest *self = G_GPIOD_LINE_REQUEST_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_LINE_REQUEST_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_request_dispose(GObject *obj)
{
	GPIODLineRequest *self = G_GPIOD_LINE_REQUEST_OBJ(obj);

	if (self->edge_event_src_id)
		g_source_remove(self->edge_event_src_id);

	G_OBJECT_CLASS(g_gpiod_line_request_parent_class)->dispose(obj);
}

static void g_gpiod_line_request_finalize(GObject *obj)
{
	GPIODLineRequest *self = G_GPIOD_LINE_REQUEST_OBJ(obj);

	g_gpiod_line_request_release(self);
	g_clear_pointer(&self->event_buf, gpiod_edge_event_buffer_free);
	g_clear_pointer(&self->val_buf, g_free);

	G_OBJECT_CLASS(g_gpiod_line_request_parent_class)->finalize(obj);
}

static void g_gpiod_line_request_class_init(GPIODLineRequestClass *line_request_class)
{
	GObjectClass *class = G_OBJECT_CLASS(line_request_class);

	class->constructed = g_gpiod_line_request_constructed;
	class->set_property = g_gpiod_line_request_set_property;
	class->get_property = g_gpiod_line_request_get_property;
	class->dispose = g_gpiod_line_request_dispose;
	class->finalize = g_gpiod_line_request_finalize;

	g_object_class_install_property(class, G_GPIOD_LINE_REQUEST_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO line request object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_REQUEST_PROP_CHIP_NAME,
		g_param_spec_string("chip-name", "Chip Name",
			"Name of the GPIO chip this request was made on.",
			NULL, G_PARAM_READABLE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_REQUEST_PROP_REQUESTED_OFFSETS,
		g_param_spec_boxed("requested-offsets", "Requested offsets",
			"Array of requested offsets.",
			G_TYPE_ARRAY,
			G_PARAM_READABLE));

	signals[G_GPIOD_LINE_REQUEST_SIGNAL_EDGE_EVENT] =
			g_signal_new("edge-event",
				     G_TYPE_FROM_CLASS(line_request_class),
				     G_SIGNAL_RUN_LAST,
				     0,
				     NULL,
				     NULL,
				     g_cclosure_marshal_generic,
				     G_TYPE_NONE,
				     1,
				     G_GPIOD_EDGE_EVENT_TYPE);
}

static void g_gpiod_line_request_init(GPIODLineRequest *self)
{
	self->handle = NULL;
	self->event_buf = NULL;
	self->edge_event_src = NULL;
}

void g_gpiod_line_request_release(GPIODLineRequest *self)
{
	g_assert(self);

	g_clear_pointer(&self->edge_event_src, g_source_unref);
	g_clear_pointer(&self->handle, gpiod_line_request_release);
}

gboolean g_gpiod_line_request_is_released(GPIODLineRequest *self)
{
	g_assert(self);

	return !self->handle;
}

static void set_err_request_released(GError **err)
{
	g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_REQUEST_RELEASED,
		    "line request was released and cannot be used");
}

const gchar *g_gpiod_line_request_get_chip_name(GPIODLineRequest *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "chip-name");
}

GArray *g_gpiod_line_request_get_requested_offsets(GPIODLineRequest *self)
{
	return g_gpiod_get_prop_boxed_array(G_OBJECT(self),
					    "requested-offsets");
}

gboolean g_gpiod_line_request_reconfigure_lines(GPIODLineRequest *self,
						GPIODLineConfig *config,
						GError **err)
{
	struct gpiod_line_config *config_handle;
	gint ret;

	g_assert(self);

	if (g_gpiod_line_request_is_released(self)) {
		set_err_request_released(err);
		return FALSE;
	}

	if (!config) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "line-config is required to reconfigure lines");
		return FALSE;
	}

	config_handle = g_gpiod_get_prop_pointer(G_OBJECT(config), "handle");

	ret = gpiod_line_request_reconfigure_lines(self->handle, config_handle);
	if (ret) {
		g_gpiod_set_error_from_errno(err,
					     "failed to reconfigure lines");
		return FALSE;
	}

	return TRUE;
}

gboolean
g_gpiod_line_request_get_value(GPIODLineRequest *self, guint offset,
			       GPIODLineValue *value, GError **err)
{
	enum gpiod_line_value val;

	g_assert(self);

	if (g_gpiod_line_request_is_released(self)) {
		set_err_request_released(err);
		return FALSE;
	}

	val = gpiod_line_request_get_value(self->handle, offset);
	if (val == GPIOD_LINE_VALUE_ERROR) {
		g_gpiod_set_error_from_errno(err,
			    "failed to get line value for offset %u", offset);
		return FALSE;
	}

	*value = g_gpiod_line_value_from_library(val);
	return TRUE;
}

gboolean g_gpiod_line_request_get_values_subset(GPIODLineRequest *self,
						const GArray *offsets,
						GArray **values,
						GError **err)
{
	guint i;
	int ret;

	g_assert(self);

	if (g_gpiod_line_request_is_released(self)) {
		set_err_request_released(err);
		return FALSE;
	}

	if (!offsets || !values) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "offsets and values must not be NULL");
		return FALSE;
	}

	ret = gpiod_line_request_get_values_subset(self->handle, offsets->len,
					(const unsigned int *)offsets->data,
					self->val_buf);
	if (ret) {
		g_gpiod_set_error_from_errno(err, "failed to read line values");
		return FALSE;
	}

	if (!(*values)) {
		*values = g_array_sized_new(FALSE, TRUE,
					    sizeof(GPIODLineValue),
					    offsets->len);
	}

	g_array_set_size(*values, offsets->len);

	for (i = 0; i < offsets->len; i++) {
		GPIODLineValue *val = &g_array_index(*values, GPIODLineValue, i);
		*val = g_gpiod_line_value_from_library(self->val_buf[i]);
	}

	return TRUE;
}

gboolean g_gpiod_line_request_get_values(GPIODLineRequest *self,
					 GArray **values, GError **err)
{
	g_autoptr(GArray) offsets = NULL;

	offsets = g_gpiod_line_request_get_requested_offsets(self);

	return g_gpiod_line_request_get_values_subset(self, offsets,
						      values, err);
}

gboolean g_gpiod_line_request_set_value(GPIODLineRequest *self, guint offset,
					GPIODLineValue value, GError **err)
{
	int ret;

	g_assert(self);

	if (g_gpiod_line_request_is_released(self)) {
		set_err_request_released(err);
		return FALSE;
	}

	ret = gpiod_line_request_set_value(self->handle, offset,
				g_gpiod_line_value_to_library(value));
	if (ret) {
		g_gpiod_set_error_from_errno(err,
			"failed to set line value for offset: %u", offset);
		return FALSE;
	}

	return TRUE;
}

gboolean g_gpiod_line_request_set_values_subset(GPIODLineRequest *self,
						const GArray *offsets,
						const GArray *values,
						GError **err)
{
	guint i;
	int ret;

	g_assert(self);

	if (g_gpiod_line_request_is_released(self)) {
		set_err_request_released(err);
		return FALSE;
	}

	if (!offsets || !values) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "offsets and values must not be NULL");
		return FALSE;
	}

	if (offsets->len != values->len) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "offsets and values must have the sme size");
		return FALSE;
	}

	for (i = 0; i < values->len; i++)
		self->val_buf[i] = g_gpiod_line_value_to_library(
					g_array_index(values,
						      GPIODLineValue, i));

	ret = gpiod_line_request_set_values_subset(self->handle,
						offsets->len,
						(unsigned int *)offsets->data,
						self->val_buf);
	if (ret) {
		g_gpiod_set_error_from_errno(err, "failed to set line values");
		return FALSE;
	}

	return TRUE;
}

gboolean g_gpiod_line_request_set_values(GPIODLineRequest *self,
					 GArray *values, GError **err)
{
	g_autoptr(GArray) offsets = NULL;

	offsets = g_gpiod_line_request_get_requested_offsets(self);

	return g_gpiod_line_request_set_values_subset(self, offsets,
						      values, err);
}
