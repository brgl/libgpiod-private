/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file edge-event.h
 */

#ifndef __GPIOD_GLIB_EDGE_EVENT_H__
#define __GPIOD_GLIB_EDGE_EVENT_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "line-info.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODEdgeEvent, g_gpiod_edge_event,
		     G_GPIOD, EDGE_EVENT, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_edge_event GPIO edge-event GObject
 * @{
 *
 * An edge event object contains information about a single line edge event.
 * It contains the event type, timestamp and the offset of the line on which
 * the event occurred as well as two sequence numbers (global for all lines
 * in the associated request and local for this line only).
 */

/**
 * @brief Get the GObject type for the GPIO edge-event.
 */
#define G_GPIOD_EDGE_EVENT_TYPE (g_gpiod_edge_event_get_type())

/**
 * @brief Cast a GObject to a GPIO edge-event concrete GObject.
 */
#define G_GPIOD_EDGE_EVENT_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_EDGE_EVENT_TYPE, \
				    GPIODEdgeEvent))

/**
 * @brief Event types.
 */
typedef enum {
	G_GPIOD_EDGE_EVENT_RISING_EDGE = 1,
	/**< Rising edge event. */
	G_GPIOD_EDGE_EVENT_FALLING_EDGE,
	/**< Falling edge event. */
} GPIODEdgeEventType;

/**
 * @brief Get the event type.
 * @param self GPIO edge event to manipulate.
 * @return The event type (::G_GPIOD_EDGE_EVENT_RISING_EDGE or
 *         ::G_GPIOD_EDGE_EVENT_FALLING_EDGE).
 */
GPIODEdgeEventType g_gpiod_edge_event_get_event_type(GPIODEdgeEvent *self);

/**
 * @brief Get the timestamp of the event.
 * @param self GPIO edge event to manipulate.
 * @return Timestamp in nanoseconds.
 * @note The source clock for the timestamp depends on the event_clock
 *       setting for the line.
 */
guint64 g_gpiod_edge_event_get_timestamp_ns(GPIODEdgeEvent *self);

/**
 * @brief Get the offset of the line which triggered the event.
 * @param self GPIO edge event to manipulate.
 * @return Line offset.
 */
guint g_gpiod_edge_event_get_line_offset(GPIODEdgeEvent *self);

/**
 * @brief Get the global sequence number of the event.
 * @param self GPIO edge event to manipulate.
 * @return Sequence number of the event in the series of events for all lines
 *         in the associated line request.
 */
gulong g_gpiod_edge_event_get_global_seqno(GPIODEdgeEvent *self);

/**
 * @brief Get the event sequence number specific to the line.
 * @param self GPIO edge event to manipulate.
 * @return Sequence number of the event in the series of events only for this
 *         line within the lifetime of the associated line request.
 */
gulong g_gpiod_edge_event_get_line_seqno(GPIODEdgeEvent *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_EDGE_EVENT_H__ */
