/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

#ifndef __GPIOD_GLIB_INTERNAL_H__
#define __GPIOD_GLIB_INTERNAL_H__

#include <glib.h>
#include <glib-object.h>
#include <gpiod.h>
#include <gpiod-glib.h>

void g_gpiod_set_error_from_errno(GError **err, const gchar *fmt, ...);

const gchar *g_gpiod_get_prop_string(GObject *obj, const gchar *prop);
gboolean g_gpiod_get_prop_bool(GObject *obj, const gchar *prop);
gint g_gpiod_get_prop_enum(GObject *obj, const gchar *prop);
guint g_gpiod_get_prop_uint(GObject *obj, const gchar *prop);
guint64 g_gpiod_get_prop_uint64(GObject *obj, const gchar *prop);
gulong g_gpiod_get_prop_ulong(GObject *obj, const gchar *prop);
GTimeSpan g_gpiod_get_prop_timespan(GObject *obj, const gchar *prop);
GObject *g_gpiod_get_prop_object(GObject *obj, const gchar *prop);
gpointer g_gpiod_get_prop_pointer(GObject *obj, const gchar *prop);
gpointer g_gpiod_get_prop_boxed_array(GObject *obj, const gchar *prop);

void g_gpiod_set_prop_uint(GObject *obj, const gchar *prop, guint val);
void g_gpiod_set_prop_string(GObject *obj, const gchar *prop, const gchar *val);
void g_gpiod_set_prop_enum(GObject *obj, const gchar *prop, gint val);
void g_gpiod_set_prop_bool(GObject *obj, const gchar *prop, gboolean val);
void g_gpiod_set_prop_timespan(GObject *obj, const gchar *prop, GTimeSpan val);

GPIODLineDirection
g_gpiod_line_direction_from_library(enum gpiod_line_direction direction,
				    gboolean allow_as_is);
GPIODLineEdge g_gpiod_line_edge_from_library(enum gpiod_line_edge edge);
GPIODLineBias g_gpiod_line_bias_from_library(enum gpiod_line_bias bias,
					     gboolean allow_as_is);
GPIODLineDrive g_gpiod_line_drive_from_library(enum gpiod_line_drive drive);
GPIODLineClock
g_gpiod_line_clock_from_library(enum gpiod_line_clock event_clock);
GPIODLineValue g_gpiod_line_value_from_library(enum gpiod_line_value value);
GPIODInfoEventType
g_gpiod_info_event_type_from_library(enum gpiod_info_event_type type);
GPIODEdgeEventType
g_gpiod_edge_event_type_from_library(enum gpiod_edge_event_type type);

enum gpiod_line_direction
g_gpiod_line_direction_to_library(GPIODLineDirection direction);
enum gpiod_line_edge g_gpiod_line_edge_to_library(GPIODLineEdge edge);
enum gpiod_line_bias g_gpiod_line_bias_to_library(GPIODLineBias bias);
enum gpiod_line_drive g_gpiod_line_drive_to_library(GPIODLineDrive drive);
enum gpiod_line_clock g_gpiod_line_clock_to_library(GPIODLineClock event_clock);
enum gpiod_line_value g_gpiod_line_value_to_library(GPIODLineValue value);

#endif /* __GPIOD_GLIB_INTERNAL_H__ */
