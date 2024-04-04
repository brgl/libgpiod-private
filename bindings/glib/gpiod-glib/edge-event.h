/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file edge-event.h
 */

#ifndef __GPIODGLIB_EDGE_EVENT_H__
#define __GPIODGLIB_EDGE_EVENT_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "line-info.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibEdgeEvent, gpiodglib_edge_event,
		     GPIODGLIB, EDGE_EVENT, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
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
#define GPIODGLIB_EDGE_EVENT_TYPE (gpiodglib_edge_event_get_type())

/**
 * @brief Cast a GObject to a GPIO edge-event concrete GObject.
 */
#define GPIODGLIB_EDGE_EVENT_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_EDGE_EVENT_TYPE, \
				    GpiodglibEdgeEvent))

/**
 * @brief Event types.
 */
typedef enum {
	GPIODGLIB_EDGE_EVENT_RISING_EDGE = 1,
	/**< Rising edge event. */
	GPIODGLIB_EDGE_EVENT_FALLING_EDGE,
	/**< Falling edge event. */
} GpiodglibEdgeEventType;

/**
 * @brief Get the event type.
 * @param self GPIO edge event to manipulate.
 * @return The event type (::GPIODGLIB_EDGE_EVENT_RISING_EDGE or
 *         ::GPIODGLIB_EDGE_EVENT_FALLING_EDGE).
 */
GpiodglibEdgeEventType
gpiodglib_edge_event_get_event_type(GpiodglibEdgeEvent *self);

/**
 * @brief Get the timestamp of the event.
 * @param self GPIO edge event to manipulate.
 * @return Timestamp in nanoseconds.
 * @note The source clock for the timestamp depends on the event_clock
 *       setting for the line.
 */
guint64 gpiodglib_edge_event_get_timestamp_ns(GpiodglibEdgeEvent *self);

/**
 * @brief Get the offset of the line which triggered the event.
 * @param self GPIO edge event to manipulate.
 * @return Line offset.
 */
guint gpiodglib_edge_event_get_line_offset(GpiodglibEdgeEvent *self);

/**
 * @brief Get the global sequence number of the event.
 * @param self GPIO edge event to manipulate.
 * @return Sequence number of the event in the series of events for all lines
 *         in the associated line request.
 */
gulong gpiodglib_edge_event_get_global_seqno(GpiodglibEdgeEvent *self);

/**
 * @brief Get the event sequence number specific to the line.
 * @param self GPIO edge event to manipulate.
 * @return Sequence number of the event in the series of events only for this
 *         line within the lifetime of the associated line request.
 */
gulong gpiodglib_edge_event_get_line_seqno(GpiodglibEdgeEvent *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_EDGE_EVENT_H__ */
