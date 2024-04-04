/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file info-event.h
 */

#ifndef __GPIODGLIB_INFO_EVENT_H__
#define __GPIODGLIB_INFO_EVENT_H__

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

G_DECLARE_FINAL_TYPE(GpiodglibInfoEvent, gpiodglib_info_event,
		     GPIODGLIB, INFO_EVENT, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
 * @{
 *
 * @defgroup gobject_info_event GPIO info-event GObject
 * @{
 *
 * An info-event contains information about the event itself (timestamp, type)
 * as well as a snapshot of line's status in the form of a line-info object.
 */

/**
 * @brief Get the GObject type for the GPIO info-event.
 */
#define GPIODGLIB_INFO_EVENT_TYPE (gpiodglib_info_event_get_type())

/**
 * @brief Cast a GObject to a GPIO info-event concrete GObject.
 */
#define GPIODGLIB_INFO_EVENT_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_INFO_EVENT_TYPE, \
				    GpiodglibInfoEvent))

/**
 * @brief Line status change event types.
 */
typedef enum {
	GPIODGLIB_INFO_EVENT_LINE_REQUESTED = 1,
	/**< Line has been requested. */
	GPIODGLIB_INFO_EVENT_LINE_RELEASED,
	/**< Previously requested line has been released. */
	GPIODGLIB_INFO_EVENT_LINE_CONFIG_CHANGED,
	/**< Line configuration has changed. */
} GpiodglibInfoEventType;

/**
 * @brief Get the event type of the status change event.
 * @param self GPIO edge event to manipulate.
 * @return One of ::GPIODGLIB_INFO_EVENT_LINE_REQUESTED,
 *         ::GPIODGLIB_INFO_EVENT_LINE_RELEASED or
 *         ::GPIODGLIB_INFO_EVENT_LINE_CONFIG_CHANGED.
 */
GpiodglibInfoEventType
gpiodglib_info_event_get_event_type(GpiodglibInfoEvent *self);

/**
 * @brief Get the timestamp of the event.
 * @param self GPIO edge event to manipulate.
 * @return Timestamp in nanoseconds, read from the monotonic clock.
 */
guint64 gpiodglib_info_event_get_timestamp_ns(GpiodglibInfoEvent *self);

/**
 * @brief Get the snapshot of line-info associated with the event.
 * @param self GPIO edge event to manipulate.
 * @return Returns a new reference to the associated line-info object.
 */
GpiodglibLineInfo *gpiodglib_info_event_get_line_info(GpiodglibInfoEvent *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_INFO_EVENT_H__ */
