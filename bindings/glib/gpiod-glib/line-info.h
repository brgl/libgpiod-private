/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-info.h
 */

#ifndef __GPIOD_GLIB_LINE_INFO_H__
#define __GPIOD_GLIB_LINE_INFO_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "line.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODLineInfo, g_gpiod_line_info,
		     G_GPIOD, LINE_INFO, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_line_info GPIO line-info GObject
 * @{
 *
 * Line info object contains an immutable snapshot of a line's status.
 *
 * The line info contains all the publicly available information about a
 * line, which does not include the line value. The line must be requested
 * to access the line value.
 */

/**
 * @brief Get the GObject type for the GPIO line-info.
 */
#define G_GPIOD_LINE_INFO_TYPE (g_gpiod_line_info_get_type())

/**
 * @brief Cast a GObject to a GPIO line-info concrete GObject.
 */
#define G_GPIOD_LINE_INFO_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_LINE_INFO_TYPE, \
				    GPIODLineInfo))

/**
 * @brief Get the offset of the line.
 * @param self GPIO line info object to manipulate.
 * @return Offset of the line within the parent chip.
 *
 * The offset uniquely identifies the line on the chip. The combination of the
 * chip and offset uniquely identifies the line within the system.
 */
guint g_gpiod_line_info_get_offset(GPIODLineInfo *self);

/**
 * @brief Get the name of the line.
 * @param self GPIO line info object to manipulate.
 * @return Name of the GPIO line as it is represented in the kernel.
 *         This function returns a valid pointer to a null-terminated string
 *         or NULL if the line is unnamed. The string lifetime is tied to the
 *         line info object so the pointer must not be freed.
 */
const gchar *g_gpiod_line_info_get_name(GPIODLineInfo *self);

/**
 * @brief Check if the line is in use.
 * @param self GPIO line info object to manipulate.
 * @return True if the line is in use, false otherwise.
 *
 * The exact reason a line is busy cannot be determined from user space.
 * It may have been requested by another process or hogged by the kernel.
 * It only matters that the line is used and can't be requested until
 * released by the existing consumer.
 */
gboolean g_gpiod_line_info_is_used(GPIODLineInfo *self);

/**
 * @brief Get the name of the consumer of the line.
 * @param self GPIO line info object to manipulate.
 * @return Name of the GPIO consumer as it is represented in the kernel.
 *         This function returns a valid pointer to a null-terminated string
 *         or NULL if the consumer name is not set. The string lifetime is tied
 *         to the line info object so the pointer must not be freed.
 */
const gchar *g_gpiod_line_info_get_consumer(GPIODLineInfo *self);

/**
 * @brief Get the direction setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::G_GPIOD_LINE_DIRECTION_INPUT or
 *        ::G_GPIOD_LINE_DIRECTION_OUTPUT.
 */
GPIODLineDirection g_gpiod_line_info_get_direction(GPIODLineInfo *self);

/**
 * @brief Get the edge detection setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::G_GPIOD_LINE_EDGE_NONE, ::G_GPIOD_LINE_EDGE_RISING,
 *        ::G_GPIOD_LINE_EDGE_FALLING or ::G_GPIOD_LINE_EDGE_BOTH.
 */
GPIODLineEdge g_gpiod_line_info_get_edge_detection(GPIODLineInfo *self);

/**
 * @brief Get the bias setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::G_GPIOD_LINE_BIAS_PULL_UP, ::G_GPIOD_LINE_BIAS_PULL_DOWN,
 *         ::G_GPIOD_LINE_BIAS_DISABLED or ::G_GPIOD_LINE_BIAS_UNKNOWN.
 */
GPIODLineBias g_gpiod_line_info_get_bias(GPIODLineInfo *self);

/**
 * @brief Get the drive setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::G_GPIOD_LINE_DRIVE_PUSH_PULL,
 *         ::G_GPIOD_LINE_DRIVE_OPEN_DRAIN or ::G_GPIOD_LINE_DRIVE_OPEN_SOURCE.
 */
GPIODLineDrive g_gpiod_line_info_get_drive(GPIODLineInfo *self);

/**
 * @brief Check if the logical value of the line is inverted compared to the
 *        physical.
 * @param self GPIO line info object to manipulate.
 * @return TRUE if the line is "active-low", FALSE otherwise.
 */
gboolean g_gpiod_line_info_is_active_low(GPIODLineInfo *self);

/**
 * @brief Check if the line is debounced (either by hardware or by the kernel
 *        software debouncer).
 * @param self GPIO line info object to manipulate.
 * @return TRUE if the line is debounced, FALSE otherwise.
 */
gboolean g_gpiod_line_info_is_debounced(GPIODLineInfo *self);

/**
 * @brief Get the debounce period of the line, in microseconds.
 * @param self GPIO line info object to manipulate.
 * @return Debounce period in microseconds. 0 if the line is not debounced.
 */
GTimeSpan g_gpiod_line_info_get_debounce_period_us(GPIODLineInfo *self);

/**
 * @brief Get the event clock setting used for edge event timestamps for the
 *        line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::G_GPIOD_LINE_CLOCK_MONOTONIC, ::G_GPIOD_LINE_CLOCK_HTE or
 *         ::G_GPIOD_LINE_CLOCK_REALTIME.
 */
GPIODLineClock g_gpiod_line_info_get_event_clock(GPIODLineInfo *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_LINE_INFO_H__ */
