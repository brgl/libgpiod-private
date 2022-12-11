/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file info-event.h
 */

#ifndef __GPIOD_GLIB_INFO_EVENT_H__
#define __GPIOD_GLIB_INFO_EVENT_H__

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

G_DECLARE_FINAL_TYPE(GPIODInfoEvent, g_gpiod_info_event,
		     G_GPIOD, INFO_EVENT, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
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
#define G_GPIOD_INFO_EVENT_TYPE (g_gpiod_info_event_get_type())

/**
 * @brief Cast a GObject to a GPIO info-event concrete GObject.
 */
#define G_GPIOD_INFO_EVENT_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_INFO_EVENT_TYPE, \
				    GPIODInfoEvent))

/**
 * @brief Line status change event types.
 */
typedef enum {
	G_GPIOD_INFO_EVENT_LINE_REQUESTED = 1,
	/**< Line has been requested. */
	G_GPIOD_INFO_EVENT_LINE_RELEASED,
	/**< Previously requested line has been released. */
	G_GPIOD_INFO_EVENT_LINE_CONFIG_CHANGED,
	/**< Line configuration has changed. */
} GPIODInfoEventType;

/**
 * @brief Get the event type of the status change event.
 * @param self GPIO edge event to manipulate.
 * @return One of ::G_GPIOD_INFO_EVENT_LINE_REQUESTED,
 *         ::G_GPIOD_INFO_EVENT_LINE_RELEASED or
 *         ::G_GPIOD_INFO_EVENT_LINE_CONFIG_CHANGED.
 */
GPIODInfoEventType g_gpiod_info_event_get_event_type(GPIODInfoEvent *self);

/**
 * @brief Get the timestamp of the event.
 * @param self GPIO edge event to manipulate.
 * @return Timestamp in nanoseconds, read from the monotonic clock.
 */
guint64 g_gpiod_info_event_get_timestamp_ns(GPIODInfoEvent *self);

/**
 * @brief Get the snapshot of line-info associated with the event.
 * @param self GPIO edge event to manipulate.
 * @return Returns a new reference to the associated line-info object.
 */
GPIODLineInfo *g_gpiod_info_event_get_line_info(GPIODInfoEvent *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_INFO_EVENT_H__ */
