// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>
#include <stdarg.h>

#include "internal.h"

struct _GPIODLineSettings {
	GObject parent_instance;
	struct gpiod_line_settings *handle;
};

enum {
	G_GPIOD_LINE_SETTINGS_PROP_HANDLE = 1,
	G_GPIOD_LINE_SETTINGS_PROP_DIRECTION,
	G_GPIOD_LINE_SETTINGS_PROP_EDGE_DETECTION,
	G_GPIOD_LINE_SETTINGS_PROP_BIAS,
	G_GPIOD_LINE_SETTINGS_PROP_DRIVE,
	G_GPIOD_LINE_SETTINGS_PROP_ACTIVE_LOW,
	G_GPIOD_LINE_SETTINGS_PROP_DEBOUNCE_PERIOD_US,
	G_GPIOD_LINE_SETTINGS_PROP_EVENT_CLOCK,
	G_GPIOD_LINE_SETTINGS_PROP_OUTPUT_VALUE,
};

G_DEFINE_TYPE(GPIODLineSettings, g_gpiod_line_settings, G_TYPE_OBJECT);

static void g_gpiod_line_settings_constructed(GObject *obj)
{
	GPIODLineSettings *self = G_GPIOD_LINE_SETTINGS_OBJ(obj);

	/*
	 * If we still haven't created the handle at this point, do it now.
	 * This is normal if called did g_gpiod_line_settings_new(NULL).
	 */
	if (!self->handle) {
		self->handle = gpiod_line_settings_new();
		if (!self->handle)
			g_error("failed to allocate line settings");
	}

	G_OBJECT_CLASS(g_gpiod_line_settings_parent_class)->constructed(obj);
}

static void g_gpiod_line_settings_get_property(GObject *obj, guint prop_id,
					       GValue *val, GParamSpec *pspec)
{
	GPIODLineSettings *self = G_GPIOD_LINE_SETTINGS_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_LINE_SETTINGS_PROP_HANDLE:
		g_value_set_pointer(val, self->handle);
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DIRECTION:
		g_value_set_enum(val,
			g_gpiod_line_direction_from_library(
				gpiod_line_settings_get_direction(
							self->handle), TRUE));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_EDGE_DETECTION:
		g_value_set_enum(val,
			g_gpiod_line_edge_from_library(
				gpiod_line_settings_get_edge_detection(
							self->handle)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_BIAS:
		g_value_set_enum(val,
			g_gpiod_line_bias_from_library(
				gpiod_line_settings_get_bias(self->handle),
				TRUE));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DRIVE:
		g_value_set_enum(val,
			g_gpiod_line_drive_from_library(
				gpiod_line_settings_get_drive(self->handle)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_ACTIVE_LOW:
		g_value_set_boolean(val,
			gpiod_line_settings_get_active_low(self->handle));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DEBOUNCE_PERIOD_US:
		g_value_set_int64(val,
			gpiod_line_settings_get_debounce_period_us(
							self->handle));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_EVENT_CLOCK:
		g_value_set_enum(val,
			g_gpiod_line_clock_from_library(
				gpiod_line_settings_get_event_clock(
							self->handle)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_OUTPUT_VALUE:
		g_value_set_enum(val,
			g_gpiod_line_value_from_library(
				gpiod_line_settings_get_output_value(
							self->handle)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_settings_set_property(GObject *obj, guint prop_id,
					       const GValue *val,
					       GParamSpec *pspec)
{
	GPIODLineSettings *self = G_GPIOD_LINE_SETTINGS_OBJ(obj);

	if (!self->handle && prop_id != G_GPIOD_LINE_SETTINGS_PROP_HANDLE) {
		self->handle = gpiod_line_settings_new();
		if (!self->handle)
			/* The only possible error is ENOMEM. */
			g_error("Failed to allocate memory for the line-settings object.");
	}

	switch (prop_id) {
	case G_GPIOD_LINE_SETTINGS_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DIRECTION:
		gpiod_line_settings_set_direction(self->handle,
			g_gpiod_line_direction_to_library(
				g_value_get_enum(val)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_EDGE_DETECTION:
		gpiod_line_settings_set_edge_detection(self->handle,
			g_gpiod_line_edge_to_library(g_value_get_enum(val)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_BIAS:
		gpiod_line_settings_set_bias(self->handle,
			g_gpiod_line_bias_to_library(g_value_get_enum(val)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DRIVE:
		gpiod_line_settings_set_drive(self->handle,
			g_gpiod_line_drive_to_library(g_value_get_enum(val)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_ACTIVE_LOW:
		gpiod_line_settings_set_active_low(self->handle,
						   g_value_get_boolean(val));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_DEBOUNCE_PERIOD_US:
		gpiod_line_settings_set_debounce_period_us(self->handle,
						g_value_get_int64(val));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_EVENT_CLOCK:
		gpiod_line_settings_set_event_clock(self->handle,
			g_gpiod_line_clock_to_library(g_value_get_enum(val)));
		break;
	case G_GPIOD_LINE_SETTINGS_PROP_OUTPUT_VALUE:
		gpiod_line_settings_set_output_value(self->handle,
			g_gpiod_line_value_to_library(g_value_get_enum(val)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_settings_finalize(GObject *obj)
{
	GPIODLineSettings *self = G_GPIOD_LINE_SETTINGS_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_line_settings_free);

	G_OBJECT_CLASS(g_gpiod_line_settings_parent_class)->finalize(obj);
}

static void
g_gpiod_line_settings_class_init(GPIODLineSettingsClass *line_settings_class)
{
	GObjectClass *class = G_OBJECT_CLASS(line_settings_class);

	class->constructed = g_gpiod_line_settings_constructed;
	class->set_property = g_gpiod_line_settings_set_property;
	class->get_property = g_gpiod_line_settings_get_property;
	class->finalize = g_gpiod_line_settings_finalize;

	g_object_class_install_property(class,
					G_GPIOD_LINE_SETTINGS_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO line settings object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_SETTINGS_PROP_DIRECTION,
		g_param_spec_enum("direction", "Direction",
			"Line direction setting.",
			G_GPIOD_LINE_DIRECTION_TYPE,
			G_GPIOD_LINE_DIRECTION_AS_IS,
			G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_SETTINGS_PROP_EDGE_DETECTION,
		g_param_spec_enum("edge-detection", "Edge Detection",
			"Line edge detection setting.",
			G_GPIOD_LINE_EDGE_TYPE,
			G_GPIOD_LINE_EDGE_NONE,
			G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_SETTINGS_PROP_BIAS,
		g_param_spec_enum("bias", "Bias",
			"Line bias setting.",
			G_GPIOD_LINE_BIAS_TYPE,
			G_GPIOD_LINE_BIAS_AS_IS,
			G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_SETTINGS_PROP_DRIVE,
		g_param_spec_enum("drive", "Drive",
			"Line drive setting.",
			G_GPIOD_LINE_DRIVE_TYPE,
			G_GPIOD_LINE_DRIVE_PUSH_PULL,
			G_PARAM_READWRITE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_SETTINGS_PROP_ACTIVE_LOW,
		g_param_spec_boolean("active-low", "Active-Low",
			"Line active-low settings.",
			FALSE, G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_SETTINGS_PROP_DEBOUNCE_PERIOD_US,
		g_param_spec_int64("debounce-period-us",
			"Debounce Period (in microseconds)",
			"Line debounce period (expressed in microseconds).",
			0, G_MAXINT64, 0, G_PARAM_READWRITE));

	g_object_class_install_property(class,
					G_GPIOD_LINE_SETTINGS_PROP_EVENT_CLOCK,
		g_param_spec_enum("event-clock", "Event Clock",
			"Clock used to timestamp edge events.",
			G_GPIOD_LINE_CLOCK_TYPE,
			G_GPIOD_LINE_CLOCK_MONOTONIC,
			G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_SETTINGS_PROP_OUTPUT_VALUE,
		g_param_spec_enum("output-value", "Output Value",
			"Line output value.",
			G_GPIOD_LINE_VALUE_TYPE,
			G_GPIOD_LINE_VALUE_INACTIVE,
			G_PARAM_READWRITE));
}

static void g_gpiod_line_settings_init(GPIODLineSettings *self)
{
	self->handle = NULL;
}

GPIODLineSettings *g_gpiod_line_settings_new(const gchar *first_prop, ...)
{
	GPIODLineSettings *settings;
	va_list va;

	va_start(va, first_prop);
	settings = G_GPIOD_LINE_SETTINGS_OBJ(
			g_object_new_valist(G_GPIOD_LINE_SETTINGS_TYPE,
					    first_prop, va));
	va_end(va);

	return settings;
}

void g_gpiod_line_settings_reset(GPIODLineSettings *self)
{
	g_assert(self);

	gpiod_line_settings_reset(self->handle);
}

void g_gpiod_line_settings_set_direction(GPIODLineSettings *self,
					 GPIODLineDirection direction)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "direction", direction);
}

GPIODLineDirection g_gpiod_line_settings_get_direction(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "direction");
}

void g_gpiod_line_settings_set_edge_detection(GPIODLineSettings *self,
					      GPIODLineEdge edge)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "edge-detection", edge);
}

GPIODLineEdge g_gpiod_line_settings_get_edge_detection(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "edge-detection");
}

void g_gpiod_line_settings_set_bias(GPIODLineSettings *self, GPIODLineBias bias)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "bias", bias);
}

GPIODLineBias g_gpiod_line_settings_get_bias(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "bias");
}

void g_gpiod_line_settings_set_drive(GPIODLineSettings *self,
				     GPIODLineDrive drive)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "drive", drive);
}

GPIODLineDrive g_gpiod_line_settings_get_drive(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "drive");
}

void g_gpiod_line_settings_set_active_low(GPIODLineSettings *self,
					  gboolean active_low)
{
	g_gpiod_set_prop_bool(G_OBJECT(self), "active-low", active_low);
}

gboolean g_gpiod_line_settings_get_active_low(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_bool(G_OBJECT(self), "active-low");
}

void g_gpiod_line_settings_set_debounce_period_us(GPIODLineSettings *self,
						  GTimeSpan period)
{
	g_gpiod_set_prop_timespan(G_OBJECT(self),
				  "debounce-period-us", period);
}

GTimeSpan g_gpiod_line_settings_get_debounce_period_us(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_timespan(G_OBJECT(self), "debounce-period-us");
}

void g_gpiod_line_settings_set_event_clock(GPIODLineSettings *self,
					   GPIODLineClock event_clock)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "event-clock", event_clock);
}

GPIODLineClock g_gpiod_line_settings_get_event_clock(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "event-clock");
}

void g_gpiod_line_settings_set_output_value(GPIODLineSettings *self,
					    GPIODLineValue value)
{
	g_gpiod_set_prop_enum(G_OBJECT(self), "output-value", value);
}

GPIODLineValue g_gpiod_line_settings_get_output_value(GPIODLineSettings *self)
{
	return g_gpiod_get_prop_enum(G_OBJECT(self), "output-value");
}
