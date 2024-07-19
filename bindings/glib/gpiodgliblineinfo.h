/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-info.h
 */

#ifndef __GPIODGLIB_LINE_INFO_H__
#define __GPIODGLIB_LINE_INFO_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiodglib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "gpiodglibline.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibLineInfo, gpiodglib_line_info,
		     GPIODGLIB, LINE_INFO, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
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
#define GPIODGLIB_LINE_INFO_TYPE (gpiodglib_line_info_get_type())

/**
 * @brief Cast a GObject to a GPIO line-info concrete GObject.
 */
#define GPIODGLIB_LINE_INFO_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_LINE_INFO_TYPE, \
				    GpiodglibLineInfo))

/**
 * @brief Get the offset of the line.
 * @param self GPIO line info object to manipulate.
 * @return Offset of the line within the parent chip.
 *
 * The offset uniquely identifies the line on the chip. The combination of the
 * chip and offset uniquely identifies the line within the system.
 */
guint gpiodglib_line_info_get_offset(GpiodglibLineInfo *self);

/**
 * @brief Get the name of the line.
 * @param self GPIO line info object to manipulate.
 * @return Name of the GPIO line as it is represented in the kernel.
 *         This function returns a valid pointer to a null-terminated string
 *         or NULL if the line is unnamed. The string is a copy of the
 *         line name and must be freed by the caller with g_free().
 */
gchar * G_GNUC_WARN_UNUSED_RESULT
gpiodglib_line_info_dup_name(GpiodglibLineInfo *self);

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
gboolean gpiodglib_line_info_is_used(GpiodglibLineInfo *self);

/**
 * @brief Get the name of the consumer of the line.
 * @param self GPIO line info object to manipulate.
 * @return Name of the GPIO consumer as it is represented in the kernel.
 *         This function returns a valid pointer to a null-terminated string
 *         or NULL if the consumer name is not set. The string is a copy of the
 *         consumer label and must be freed by the caller with g_free().
 */
gchar * G_GNUC_WARN_UNUSED_RESULT
gpiodglib_line_info_dup_consumer(GpiodglibLineInfo *self);

/**
 * @brief Get the direction setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::GPIODGLIB_LINE_DIRECTION_INPUT or
 *        ::GPIODGLIB_LINE_DIRECTION_OUTPUT.
 */
GpiodglibLineDirection
gpiodglib_line_info_get_direction(GpiodglibLineInfo *self);

/**
 * @brief Get the edge detection setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::GPIODGLIB_LINE_EDGE_NONE, ::GPIODGLIB_LINE_EDGE_RISING,
 *        ::GPIODGLIB_LINE_EDGE_FALLING or ::GPIODGLIB_LINE_EDGE_BOTH.
 */
GpiodglibLineEdge
gpiodglib_line_info_get_edge_detection(GpiodglibLineInfo *self);

/**
 * @brief Get the bias setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::GPIODGLIB_LINE_BIAS_PULL_UP, ::GPIODGLIB_LINE_BIAS_PULL_DOWN,
 *         ::GPIODGLIB_LINE_BIAS_DISABLED or ::GPIODGLIB_LINE_BIAS_UNKNOWN.
 */
GpiodglibLineBias gpiodglib_line_info_get_bias(GpiodglibLineInfo *self);

/**
 * @brief Get the drive setting of the line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::GPIODGLIB_LINE_DRIVE_PUSH_PULL,
 *         ::GPIODGLIB_LINE_DRIVE_OPEN_DRAIN or ::GPIODGLIB_LINE_DRIVE_OPEN_SOURCE.
 */
GpiodglibLineDrive gpiodglib_line_info_get_drive(GpiodglibLineInfo *self);

/**
 * @brief Check if the logical value of the line is inverted compared to the
 *        physical.
 * @param self GPIO line info object to manipulate.
 * @return TRUE if the line is "active-low", FALSE otherwise.
 */
gboolean gpiodglib_line_info_is_active_low(GpiodglibLineInfo *self);

/**
 * @brief Check if the line is debounced (either by hardware or by the kernel
 *        software debouncer).
 * @param self GPIO line info object to manipulate.
 * @return TRUE if the line is debounced, FALSE otherwise.
 */
gboolean gpiodglib_line_info_is_debounced(GpiodglibLineInfo *self);

/**
 * @brief Get the debounce period of the line, in microseconds.
 * @param self GPIO line info object to manipulate.
 * @return Debounce period in microseconds. 0 if the line is not debounced.
 */
GTimeSpan gpiodglib_line_info_get_debounce_period_us(GpiodglibLineInfo *self);

/**
 * @brief Get the event clock setting used for edge event timestamps for the
 *        line.
 * @param self GPIO line info object to manipulate.
 * @return Returns ::GPIODGLIB_LINE_CLOCK_MONOTONIC, ::GPIODGLIB_LINE_CLOCK_HTE or
 *         ::GPIODGLIB_LINE_CLOCK_REALTIME.
 */
GpiodglibLineClock gpiodglib_line_info_get_event_clock(GpiodglibLineInfo *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_LINE_INFO_H__ */
