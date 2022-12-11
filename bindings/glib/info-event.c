// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODInfoEvent {
	GObject parent_instance;
	struct gpiod_info_event *handle;
	GPIODLineInfo *info;
};

enum {
	G_GPIOD_INFO_EVENT_PROP_HANDLE = 1,
	G_GPIOD_INFO_EVENT_PROP_EVENT_TYPE,
	G_GPIOD_INFO_EVENT_PROP_TIMESTAMP,
	G_GPIOD_INFO_EVENT_PROP_LINE_INFO,
};

G_DEFINE_TYPE(GPIODInfoEvent, g_gpiod_info_event, G_TYPE_OBJECT);

static void g_gpiod_info_event_get_property(GObject *obj, guint prop_id,
					    GValue *val, GParamSpec *pspec)
{
	GPIODInfoEvent *self = G_GPIOD_INFO_EVENT_OBJ(obj);
	struct gpiod_line_info *info, *cpy;
	GPIODInfoEventType type;

	switch (prop_id) {
	case G_GPIOD_INFO_EVENT_PROP_EVENT_TYPE:
		type = g_gpiod_info_event_type_from_library(
				gpiod_info_event_get_event_type(self->handle));
		g_value_set_enum(val, type);
		break;
	case G_GPIOD_INFO_EVENT_PROP_TIMESTAMP:
		g_value_set_uint64(val,
			gpiod_info_event_get_timestamp_ns(self->handle));
		break;
	case G_GPIOD_INFO_EVENT_PROP_LINE_INFO:
		if (!self->info) {
			info = gpiod_info_event_get_line_info(self->handle);
			cpy = gpiod_line_info_copy(info);
			if (!cpy)
				g_error("Failed to allocate memory for line-info object");

			self->info = G_GPIOD_LINE_INFO_OBJ(
				g_object_new(G_GPIOD_LINE_INFO_TYPE,
					"handle", cpy, NULL));
		}

		g_value_set_object(val, g_object_ref(self->info));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_info_event_set_property(GObject *obj, guint prop_id,
					    const GValue *val,
					    GParamSpec *pspec)
{
	GPIODInfoEvent *self = G_GPIOD_INFO_EVENT_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_INFO_EVENT_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_info_event_dispose(GObject *obj)
{
	GPIODInfoEvent *self = G_GPIOD_INFO_EVENT_OBJ(obj);

	g_clear_object(&self->info);

	G_OBJECT_CLASS(g_gpiod_info_event_parent_class)->dispose(obj);
}

static void g_gpiod_info_event_finalize(GObject *obj)
{
	GPIODInfoEvent *self = G_GPIOD_INFO_EVENT_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_info_event_free);

	G_OBJECT_CLASS(g_gpiod_info_event_parent_class)->finalize(obj);
}

static void g_gpiod_info_event_class_init(GPIODInfoEventClass *info_event_class)
{
	GObjectClass *class = G_OBJECT_CLASS(info_event_class);

	class->set_property = g_gpiod_info_event_set_property;
	class->get_property = g_gpiod_info_event_get_property;
	class->dispose = g_gpiod_info_event_dispose;
	class->finalize = g_gpiod_info_event_finalize;

	g_object_class_install_property(class, G_GPIOD_INFO_EVENT_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO info event object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	g_object_class_install_property(class,
					G_GPIOD_INFO_EVENT_PROP_EVENT_TYPE,
		g_param_spec_enum("event-type", "Event Type",
			"Type of the info event.",
			G_GPIOD_INFO_EVENT_TYPE_TYPE,
			G_GPIOD_INFO_EVENT_LINE_REQUESTED,
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_INFO_EVENT_PROP_TIMESTAMP,
		g_param_spec_uint64("timestamp-ns",
			"Timestamp (in nanoseconds)",
			"Timestamp of the info event expressed in nanoseconds.",
			0, G_MAXUINT64, 0, G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_INFO_EVENT_PROP_LINE_INFO,
		g_param_spec_object("line-info", "Line Info",
			"New line-info snapshot associated with this info event.",
			G_GPIOD_LINE_INFO_TYPE, G_PARAM_READABLE));
}

static void g_gpiod_info_event_init(GPIODInfoEvent *self)
{
	self->handle = NULL;
	self->info = NULL;
}

GPIODInfoEventType g_gpiod_info_event_get_event_type(GPIODInfoEvent *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "event-type");
}

guint64 g_gpiod_info_event_get_timestamp_ns(GPIODInfoEvent *self)
{
	return g_gpiod_get_prop_uint64(G_OBJECT(self), "timestamp-ns");
}

GPIODLineInfo *g_gpiod_info_event_get_line_info(GPIODInfoEvent *self)
{
	return G_GPIOD_LINE_INFO_OBJ(
			g_gpiod_get_prop_object(G_OBJECT(self), "line-info"));
}
