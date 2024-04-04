// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODLineInfo {
	GObject parent_instance;
	struct gpiod_line_info *handle;
};

enum {
	G_GPIOD_LINE_INFO_PROP_HANDLE = 1,
	G_GPIOD_LINE_INFO_PROP_OFFSET,
	G_GPIOD_LINE_INFO_PROP_NAME,
	G_GPIOD_LINE_INFO_PROP_USED,
	G_GPIOD_LINE_INFO_PROP_CONSUMER,
	G_GPIOD_LINE_INFO_PROP_DIRECTION,
	G_GPIOD_LINE_INFO_PROP_EDGE_DETECTION,
	G_GPIOD_LINE_INFO_PROP_BIAS,
	G_GPIOD_LINE_INFO_PROP_DRIVE,
	G_GPIOD_LINE_INFO_PROP_ACTIVE_LOW,
	G_GPIOD_LINE_INFO_PROP_DEBOUNCED,
	G_GPIOD_LINE_INFO_PROP_DEBOUNCE_PERIOD,
	G_GPIOD_LINE_INFO_PROP_EVENT_CLOCK,
};

G_DEFINE_TYPE(GPIODLineInfo, g_gpiod_line_info, G_TYPE_OBJECT);

static void g_gpiod_line_info_get_property(GObject *obj, guint prop_id,
					   GValue *val, GParamSpec *pspec)
{
	GPIODLineInfo *self = G_GPIOD_LINE_INFO_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_LINE_INFO_PROP_OFFSET:
		g_value_set_uint(val, gpiod_line_info_get_offset(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_NAME:
		g_value_set_static_string(val,
			gpiod_line_info_get_name(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_USED:
		g_value_set_boolean(val, gpiod_line_info_is_used(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_CONSUMER:
		g_value_set_static_string(val,
			gpiod_line_info_get_consumer(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_DIRECTION:
		g_value_set_enum(val,
			g_gpiod_line_direction_from_library(
				gpiod_line_info_get_direction(self->handle),
				FALSE));
		break;
	case G_GPIOD_LINE_INFO_PROP_EDGE_DETECTION:
		g_value_set_enum(val,
			g_gpiod_line_edge_from_library(
				gpiod_line_info_get_edge_detection(
					self->handle)));
		break;
	case G_GPIOD_LINE_INFO_PROP_BIAS:
		g_value_set_enum(val,
			g_gpiod_line_bias_from_library(
				gpiod_line_info_get_bias(self->handle),
				FALSE));
		break;
	case G_GPIOD_LINE_INFO_PROP_DRIVE:
		g_value_set_enum(val,
			g_gpiod_line_drive_from_library(
				gpiod_line_info_get_drive(self->handle)));
		break;
	case G_GPIOD_LINE_INFO_PROP_ACTIVE_LOW:
		g_value_set_boolean(val,
			gpiod_line_info_is_active_low(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_DEBOUNCED:
		g_value_set_boolean(val,
			gpiod_line_info_is_debounced(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_DEBOUNCE_PERIOD:
		g_value_set_int64(val,
			gpiod_line_info_get_debounce_period_us(self->handle));
		break;
	case G_GPIOD_LINE_INFO_PROP_EVENT_CLOCK:
		g_value_set_enum(val,
			g_gpiod_line_clock_from_library(
				gpiod_line_info_get_event_clock(self->handle)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_info_set_property(GObject *obj, guint prop_id,
					   const GValue *val, GParamSpec *pspec)
{
	GPIODLineInfo *self = G_GPIOD_LINE_INFO_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_LINE_INFO_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_info_finalize(GObject *obj)
{
	GPIODLineInfo *self = G_GPIOD_LINE_INFO_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_line_info_free);

	G_OBJECT_CLASS(g_gpiod_line_info_parent_class)->finalize(obj);
}

static void g_gpiod_line_info_class_init(GPIODLineInfoClass *line_info_class)
{
	GObjectClass *class = G_OBJECT_CLASS(line_info_class);

	class->set_property = g_gpiod_line_info_set_property;
	class->get_property = g_gpiod_line_info_get_property;
	class->finalize = g_gpiod_line_info_finalize;

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO line information object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_OFFSET,
		g_param_spec_uint("offset", "Offset",
			"Offset of the GPIO line.",
			0, G_MAXUINT, 0, G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_NAME,
		g_param_spec_string("name", "Name",
			"Name of the GPIO line, if named.",
			NULL, G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_USED,
		g_param_spec_boolean("used", "Is Used",
			"Indicates whether the GPIO line is requested for exclusive usage",
			FALSE, G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_CONSUMER,
		g_param_spec_string("consumer", "Consumer",
			"Name of the consumer of the GPIO line, if requested.",
			NULL, G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_DIRECTION,
		g_param_spec_enum("direction", "Direction",
			"Direction of the GPIO line.",
			G_GPIOD_LINE_DIRECTION_TYPE,
			G_GPIOD_LINE_DIRECTION_INPUT,
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_INFO_PROP_EDGE_DETECTION,
		g_param_spec_enum("edge-detection", "Edge Detection",
			"Edge detection setting of the GPIO line.",
			G_GPIOD_LINE_EDGE_TYPE,
			G_GPIOD_LINE_EDGE_NONE,
			G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_BIAS,
		g_param_spec_enum("bias", "Bias",
			"Bias setting of the GPIO line.",
			G_GPIOD_LINE_BIAS_TYPE,
			G_GPIOD_LINE_BIAS_UNKNOWN,
			G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_DRIVE,
		g_param_spec_enum("drive", "Drive",
			"Drive setting of the GPIO line.",
			G_GPIOD_LINE_DRIVE_TYPE,
			G_GPIOD_LINE_DRIVE_PUSH_PULL,
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_INFO_PROP_ACTIVE_LOW,
		g_param_spec_boolean("active-low", "Is Active-Low",
			"Indicates whether the signal of the line is inverted.",
			FALSE, G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_LINE_INFO_PROP_DEBOUNCED,
		g_param_spec_boolean("debounced", "Is Debounced",
			"Indicates whether the line is debounced (by hardware or by the kernel software debouncer).",
			FALSE, G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_INFO_PROP_DEBOUNCE_PERIOD,
		g_param_spec_int64("debounce-period-us",
			"Debounce Period (in microseconds)",
			"Debounce period of the line (expressed in microseconds).",
			0, G_MAXINT64, 0,
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_INFO_PROP_EVENT_CLOCK,
		g_param_spec_enum("event-clock", "Event Clock",
			"Event clock used to timestamp the edge events of the line.",
			G_GPIOD_LINE_CLOCK_TYPE,
			G_GPIOD_LINE_CLOCK_MONOTONIC,
			G_PARAM_READABLE));
}

static void g_gpiod_line_info_init(GPIODLineInfo *self)
{
	self->handle = NULL;
}

guint g_gpiod_line_info_get_offset(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_uint(G_OBJECT(self), "offset");
}

const gchar *g_gpiod_line_info_get_name(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "name");
}

gboolean g_gpiod_line_info_is_used(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_bool(G_OBJECT(self), "used");
}

const gchar *g_gpiod_line_info_get_consumer(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "consumer");
}

GPIODLineDirection g_gpiod_line_info_get_direction(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "direction");
}

GPIODLineEdge g_gpiod_line_info_get_edge_detection(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "edge-detection");
}

GPIODLineBias g_gpiod_line_info_get_bias(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "bias");
}

GPIODLineDrive g_gpiod_line_info_get_drive(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "drive");
}

gboolean g_gpiod_line_info_is_active_low(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_bool(G_OBJECT(self), "active-low");
}

gboolean g_gpiod_line_info_is_debounced(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_bool(G_OBJECT(self), "debounced");
}

GTimeSpan g_gpiod_line_info_get_debounce_period_us(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_timespan(G_OBJECT(self), "debounce-period-us");
}

GPIODLineClock g_gpiod_line_info_get_event_clock(GPIODLineInfo *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "event-clock");
}
