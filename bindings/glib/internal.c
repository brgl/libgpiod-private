// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include "internal.h"

#define get_prop(_obj, _prop, _type, _vtype, _get_func) \
	({ \
		g_auto(GValue) _val = G_VALUE_INIT; \
		_type _ret; \
		g_value_init(&_val, _vtype); \
		g_object_get_property(_obj, _prop, &_val); \
		_ret = _get_func(&_val); \
		_ret; \
	})

G_GNUC_INTERNAL const gchar *
g_gpiod_get_prop_string(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, const gchar *, G_TYPE_STRING,
			g_value_get_string);
}

G_GNUC_INTERNAL gboolean g_gpiod_get_prop_bool(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, gboolean, G_TYPE_BOOLEAN,
			g_value_get_boolean);
}

G_GNUC_INTERNAL gint g_gpiod_get_prop_enum(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, gint, G_TYPE_ENUM, g_value_get_enum);
}

G_GNUC_INTERNAL guint g_gpiod_get_prop_uint(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, guint, G_TYPE_UINT, g_value_get_uint);
}

G_GNUC_INTERNAL guint64 g_gpiod_get_prop_uint64(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, guint64, G_TYPE_UINT64, g_value_get_uint64);
}

G_GNUC_INTERNAL gulong g_gpiod_get_prop_ulong(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, gulong, G_TYPE_ULONG, g_value_get_ulong);
}

G_GNUC_INTERNAL GTimeSpan
g_gpiod_get_prop_timespan(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, GTimeSpan, G_TYPE_INT64, g_value_get_int64);
}

G_GNUC_INTERNAL GObject *
g_gpiod_get_prop_object(GObject *obj, const gchar *prop)
{
	return G_OBJECT(get_prop(obj, prop, gpointer, G_TYPE_OBJECT,
			g_value_get_object));
}

G_GNUC_INTERNAL gpointer
g_gpiod_get_prop_pointer(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, gpointer, G_TYPE_POINTER,
			g_value_get_pointer);
}

G_GNUC_INTERNAL gpointer
g_gpiod_get_prop_boxed_array(GObject *obj, const gchar *prop)
{
	return get_prop(obj, prop, gpointer, G_TYPE_ARRAY, g_value_get_boxed);
}

#define set_prop(_obj, _prop, _set_func, _vtype, _val) \
	do { \
		g_auto(GValue) _gval = G_VALUE_INIT; \
		g_value_init(&_gval, _vtype); \
		_set_func(&_gval, _val); \
		g_object_set_property(_obj, _prop, &_gval); \
	} while (0)

G_GNUC_INTERNAL void
g_gpiod_set_prop_uint(GObject *obj, const gchar *prop, guint val)
{
	set_prop(obj, prop, g_value_set_uint, G_TYPE_UINT, val);
}

G_GNUC_INTERNAL void
g_gpiod_set_prop_string(GObject *obj, const gchar *prop, const gchar *val)
{
	set_prop(obj, prop, g_value_set_string, G_TYPE_STRING, val);
}

G_GNUC_INTERNAL void
g_gpiod_set_prop_enum(GObject *obj, const gchar *prop, gint val)
{
	set_prop(obj, prop, g_value_set_enum, G_TYPE_ENUM, val);
}

G_GNUC_INTERNAL void
g_gpiod_set_prop_bool(GObject *obj, const gchar *prop, gboolean val)
{
	set_prop(obj, prop, g_value_set_boolean, G_TYPE_BOOLEAN, val);
}

G_GNUC_INTERNAL void
g_gpiod_set_prop_timespan(GObject *obj, const gchar *prop, GTimeSpan val)
{
	set_prop(obj, prop, g_value_set_int64, G_TYPE_INT64, val);
}

G_GNUC_INTERNAL GPIODLineDirection
g_gpiod_line_direction_from_library(enum gpiod_line_direction direction,
				    gboolean allow_as_is)
{
	switch (direction) {
	case GPIOD_LINE_DIRECTION_AS_IS:
		if (allow_as_is)
			return G_GPIOD_LINE_DIRECTION_AS_IS;
		break;
	case GPIOD_LINE_DIRECTION_INPUT:
		return G_GPIOD_LINE_DIRECTION_INPUT;
	case GPIOD_LINE_DIRECTION_OUTPUT:
		return G_GPIOD_LINE_DIRECTION_OUTPUT;
	}

	g_error("invalid line direction value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODLineEdge
g_gpiod_line_edge_from_library(enum gpiod_line_edge edge)
{
	switch (edge) {
	case GPIOD_LINE_EDGE_NONE:
		return G_GPIOD_LINE_EDGE_NONE;
	case GPIOD_LINE_EDGE_RISING:
		return G_GPIOD_LINE_EDGE_RISING;
	case GPIOD_LINE_EDGE_FALLING:
		return G_GPIOD_LINE_EDGE_FALLING;
	case GPIOD_LINE_EDGE_BOTH:
		return G_GPIOD_LINE_EDGE_BOTH;
	}

	g_error("invalid line edge value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODLineBias
g_gpiod_line_bias_from_library(enum gpiod_line_bias bias, gboolean allow_as_is)
{
	switch (bias) {
	case GPIOD_LINE_BIAS_AS_IS:
		if (allow_as_is)
			return G_GPIOD_LINE_BIAS_AS_IS;
		break;
	case GPIOD_LINE_BIAS_UNKNOWN:
		return G_GPIOD_LINE_BIAS_UNKNOWN;
	case GPIOD_LINE_BIAS_DISABLED:
		return G_GPIOD_LINE_BIAS_DISABLED;
	case GPIOD_LINE_BIAS_PULL_UP:
		return G_GPIOD_LINE_BIAS_PULL_UP;
	case GPIOD_LINE_BIAS_PULL_DOWN:
		return G_GPIOD_LINE_BIAS_PULL_DOWN;
	}

	g_error("invalid line bias value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODLineDrive
g_gpiod_line_drive_from_library(enum gpiod_line_drive drive)
{
	switch (drive) {
	case GPIOD_LINE_DRIVE_PUSH_PULL:
		return G_GPIOD_LINE_DRIVE_PUSH_PULL;
	case GPIOD_LINE_DRIVE_OPEN_DRAIN:
		return G_GPIOD_LINE_DRIVE_OPEN_DRAIN;
	case GPIOD_LINE_DRIVE_OPEN_SOURCE:
		return G_GPIOD_LINE_DRIVE_OPEN_SOURCE;
	}

	g_error("invalid line drive value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODLineClock
g_gpiod_line_clock_from_library(enum gpiod_line_clock event_clock)
{
	switch (event_clock) {
	case GPIOD_LINE_CLOCK_MONOTONIC:
		return G_GPIOD_LINE_CLOCK_MONOTONIC;
	case GPIOD_LINE_CLOCK_REALTIME:
		return G_GPIOD_LINE_CLOCK_REALTIME;
	case GPIOD_LINE_CLOCK_HTE:
		return G_GPIOD_LINE_CLOCK_HTE;
	}

	g_error("invalid line event clock value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODLineValue
g_gpiod_line_value_from_library(enum gpiod_line_value value)
{
	switch (value) {
	case GPIOD_LINE_VALUE_INACTIVE:
		return G_GPIOD_LINE_VALUE_INACTIVE;
	case GPIOD_LINE_VALUE_ACTIVE:
		return G_GPIOD_LINE_VALUE_ACTIVE;
	default:
		break;
	}

	g_error("invalid line value returned by libgpiod");
}

G_GNUC_INTERNAL GPIODInfoEventType
g_gpiod_info_event_type_from_library(enum gpiod_info_event_type type)
{
	switch (type) {
	case GPIOD_INFO_EVENT_LINE_REQUESTED:
		return G_GPIOD_INFO_EVENT_LINE_REQUESTED;
	case GPIOD_INFO_EVENT_LINE_RELEASED:
		return G_GPIOD_INFO_EVENT_LINE_RELEASED;
	case GPIOD_INFO_EVENT_LINE_CONFIG_CHANGED:
		return G_GPIOD_INFO_EVENT_LINE_CONFIG_CHANGED;
	}
	
	g_error("invalid info-event type returned by libgpiod");
}

G_GNUC_INTERNAL GPIODEdgeEventType
g_gpiod_edge_event_type_from_library(enum gpiod_edge_event_type type)
{
	switch (type) {
	case GPIOD_EDGE_EVENT_RISING_EDGE:
		return G_GPIOD_EDGE_EVENT_RISING_EDGE;
	case GPIOD_EDGE_EVENT_FALLING_EDGE:
		return G_GPIOD_EDGE_EVENT_FALLING_EDGE;
	}

	g_error("invalid edge-event type returned by libgpiod");
}

G_GNUC_INTERNAL enum gpiod_line_direction
g_gpiod_line_direction_to_library(GPIODLineDirection direction)
{
	switch (direction) {
	case G_GPIOD_LINE_DIRECTION_AS_IS:
		return GPIOD_LINE_DIRECTION_AS_IS;
	case G_GPIOD_LINE_DIRECTION_INPUT:
		return GPIOD_LINE_DIRECTION_INPUT;
	case G_GPIOD_LINE_DIRECTION_OUTPUT:
		return GPIOD_LINE_DIRECTION_OUTPUT;
	}

	g_error("invalid line direction value");
}

G_GNUC_INTERNAL enum gpiod_line_edge
g_gpiod_line_edge_to_library(GPIODLineEdge edge)
{
	switch (edge) {
	case G_GPIOD_LINE_EDGE_NONE:
		return GPIOD_LINE_EDGE_NONE;
	case G_GPIOD_LINE_EDGE_RISING:
		return GPIOD_LINE_EDGE_RISING;
	case G_GPIOD_LINE_EDGE_FALLING:
		return GPIOD_LINE_EDGE_FALLING;
	case G_GPIOD_LINE_EDGE_BOTH:
		return GPIOD_LINE_EDGE_BOTH;
	}

	g_error("invalid line edge value");
}

G_GNUC_INTERNAL enum gpiod_line_bias
g_gpiod_line_bias_to_library(GPIODLineBias bias)
{
	switch (bias) {
	case G_GPIOD_LINE_BIAS_AS_IS:
		return GPIOD_LINE_BIAS_AS_IS;
	case G_GPIOD_LINE_BIAS_DISABLED:
		return GPIOD_LINE_BIAS_DISABLED;
	case G_GPIOD_LINE_BIAS_PULL_UP:
		return GPIOD_LINE_BIAS_PULL_UP;
	case G_GPIOD_LINE_BIAS_PULL_DOWN:
		return GPIOD_LINE_BIAS_PULL_DOWN;
	default:
		break;
	}

	g_error("invalid line bias value");
}

G_GNUC_INTERNAL enum gpiod_line_drive
g_gpiod_line_drive_to_library(GPIODLineDrive drive)
{
	switch (drive) {
	case G_GPIOD_LINE_DRIVE_PUSH_PULL:
		return GPIOD_LINE_DRIVE_PUSH_PULL;
	case G_GPIOD_LINE_DRIVE_OPEN_SOURCE:
		return GPIOD_LINE_DRIVE_OPEN_SOURCE;
	case G_GPIOD_LINE_DRIVE_OPEN_DRAIN:
		return GPIOD_LINE_DRIVE_OPEN_DRAIN;
	}

	g_error("invalid line drive value");
}

G_GNUC_INTERNAL enum gpiod_line_clock
g_gpiod_line_clock_to_library(GPIODLineClock event_clock)
{
	switch (event_clock) {
	case G_GPIOD_LINE_CLOCK_MONOTONIC:
		return GPIOD_LINE_CLOCK_MONOTONIC;
	case G_GPIOD_LINE_CLOCK_REALTIME:
		return GPIOD_LINE_CLOCK_REALTIME;
	case G_GPIOD_LINE_CLOCK_HTE:
		return GPIOD_LINE_CLOCK_HTE;
	}

	g_error("invalid line clock value");
}

G_GNUC_INTERNAL enum gpiod_line_value
g_gpiod_line_value_to_library(GPIODLineValue value)
{
	switch (value) {
	case G_GPIOD_LINE_VALUE_INACTIVE:
		return GPIOD_LINE_VALUE_INACTIVE;
	case G_GPIOD_LINE_VALUE_ACTIVE:
		return GPIOD_LINE_VALUE_ACTIVE;
	}

	g_error("invalid line value");
}
