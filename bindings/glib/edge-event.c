// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODEdgeEvent {
	GObject parent_instance;
	struct gpiod_edge_event *handle;
};

enum {
	G_GPIOD_EDGE_EVENT_PROP_HANDLE = 1,
	G_GPIOD_EDGE_EVENT_PROP_EVENT_TYPE,
	G_GPIOD_EDGE_EVENT_PROP_TIMESTAMP_NS,
	G_GPIOD_EDGE_EVENT_PROP_LINE_OFFSET,
	G_GPIOD_EDGE_EVENT_PROP_GLOBAL_SEQNO,
	G_GPIOD_EDGE_EVENT_PROP_LINE_SEQNO,
};

G_DEFINE_TYPE(GPIODEdgeEvent, g_gpiod_edge_event, G_TYPE_OBJECT);

static void g_gpiod_edge_event_get_property(GObject *obj, guint prop_id,
					    GValue *val, GParamSpec *pspec)
{
	GPIODEdgeEvent *self = G_GPIOD_EDGE_EVENT_OBJ(obj);
	GPIODEdgeEventType type;

	switch (prop_id) {
	case G_GPIOD_EDGE_EVENT_PROP_EVENT_TYPE:
		type = g_gpiod_edge_event_type_from_library(
				gpiod_edge_event_get_event_type(self->handle));
		g_value_set_enum(val, type);
		break;
	case G_GPIOD_EDGE_EVENT_PROP_TIMESTAMP_NS:
		g_value_set_uint64(val,
			gpiod_edge_event_get_timestamp_ns(self->handle));
		break;
	case G_GPIOD_EDGE_EVENT_PROP_LINE_OFFSET:
		g_value_set_uint(val,
			gpiod_edge_event_get_line_offset(self->handle));
		break;
	case G_GPIOD_EDGE_EVENT_PROP_GLOBAL_SEQNO:
		g_value_set_ulong(val,
			gpiod_edge_event_get_global_seqno(self->handle));
		break;
	case G_GPIOD_EDGE_EVENT_PROP_LINE_SEQNO:
		g_value_set_ulong(val,
			gpiod_edge_event_get_line_seqno(self->handle));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_edge_event_set_property(GObject *obj, guint prop_id,
					    const GValue *val,
					    GParamSpec *pspec)
{
	GPIODEdgeEvent *self = G_GPIOD_EDGE_EVENT_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_EDGE_EVENT_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_edge_event_finalize(GObject *obj)
{
	GPIODEdgeEvent *self = G_GPIOD_EDGE_EVENT_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_edge_event_free);

	G_OBJECT_CLASS(g_gpiod_edge_event_parent_class)->finalize(obj);
}

static void g_gpiod_edge_event_class_init(GPIODEdgeEventClass *edge_event_class)
{
	GObjectClass *class = G_OBJECT_CLASS(edge_event_class);

	class->set_property = g_gpiod_edge_event_set_property;
	class->get_property = g_gpiod_edge_event_get_property;
	class->finalize = g_gpiod_edge_event_finalize;

	g_object_class_install_property(class, G_GPIOD_EDGE_EVENT_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO info event object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	g_object_class_install_property(class,
					G_GPIOD_EDGE_EVENT_PROP_EVENT_TYPE,
		g_param_spec_enum("event-type", "Event Type",
			"Type of the edge event.",
			G_GPIOD_EDGE_EVENT_TYPE_TYPE,
			G_GPIOD_EDGE_EVENT_RISING_EDGE,
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_EDGE_EVENT_PROP_TIMESTAMP_NS,
		g_param_spec_uint64("timestamp-ns",
			"Timestamp (in nanoseconds)",
			"Timestamp of the edge event expressed in nanoseconds.",
			0, G_MAXUINT64, 0, G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_EDGE_EVENT_PROP_LINE_OFFSET,
		g_param_spec_uint("line-offset", "Line Offset",
			"Offset of the line on which this event was registered.",
			0, G_MAXUINT, 0, G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_EDGE_EVENT_PROP_GLOBAL_SEQNO,
		g_param_spec_ulong("global-seqno", "Global Sequence Number",
			"Global sequence number of this event",
			0, G_MAXULONG, 0, G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_EDGE_EVENT_PROP_LINE_SEQNO,
		g_param_spec_ulong("line-seqno", "Line Sequence Number",
			"Event sequence number specific to the line.",
			0, G_MAXULONG, 0, G_PARAM_READABLE));
}

static void g_gpiod_edge_event_init(GPIODEdgeEvent *self)
{
	self->handle = NULL;
}

GPIODEdgeEventType g_gpiod_edge_event_get_event_type(GPIODEdgeEvent *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "event-type");
}

guint64 g_gpiod_edge_event_get_timestamp_ns(GPIODEdgeEvent *self)
{
	return g_gpiod_get_prop_uint64(G_OBJECT(self), "timestamp-ns");
}

guint g_gpiod_edge_event_get_line_offset(GPIODEdgeEvent *self)
{
	return g_gpiod_get_prop_uint(G_OBJECT(self), "line-offset");
}

gulong g_gpiod_edge_event_get_global_seqno(GPIODEdgeEvent *self)
{
	return g_gpiod_get_prop_ulong(G_OBJECT(self), "global-seqno");
}

gulong g_gpiod_edge_event_get_line_seqno(GPIODEdgeEvent *self)
{
	return g_gpiod_get_prop_ulong(G_OBJECT(self), "line-seqno");
}
