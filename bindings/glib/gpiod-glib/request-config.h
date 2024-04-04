/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file request-config.h
 */

#ifndef __GPIOD_GLIB_REQUEST_CONFIG_H__
#define __GPIOD_GLIB_REQUEST_CONFIG_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODRequestConfig, g_gpiod_request_config,
		     G_GPIOD, REQUEST_CONFIG, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_request_config GPIO request-config GObject
 * @{
 *
 * Request config objects are used to pass a set of options to the kernel at
 * the time of the line request.
 */

/**
 * @brief Get the GObject type for the GPIO request-config.
 */
#define G_GPIOD_REQUEST_CONFIG_TYPE (g_gpiod_request_config_get_type())

/**
 * @brief Cast a GObject to a GPIO request-config concrete GObject.
 */
#define G_GPIOD_REQUEST_CONFIG_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_REQUEST_CONFIG_TYPE, \
				    GPIODRequestConfig))

/**
 * @brief Create a new request config object.
 * @param first_prop Name of the first property to set.
 * @return New request config object.
 *
 * The constructor allows to set object's properties when it's first created
 * instead of having to build an empty object and then call mutators separately.
 *
 * Currently supported properties are: `consumer` and `event-buffer-size`.
 */
GPIODRequestConfig *g_gpiod_request_config_new(const gchar *first_prop, ...);

/**
 * @brief Set the consumer name for the request.
 * @param self GPIO request config object to manipulate.
 * @param consumer Consumer name.
 * @note If the consumer string is too long, it will be truncated to the max
 *       accepted length.
 */
void g_gpiod_request_config_set_consumer(GPIODRequestConfig *self,
					 const gchar *consumer);

/**
 * @brief Get the consumer name configured in the request config.
 * @param self GPIO request config object to manipulate.
 * @return Consumer name stored in the request config.
 */
const gchar *g_gpiod_request_config_get_consumer(GPIODRequestConfig *self);

/**
 * @brief Set the size of the kernel event buffer for the request.
 * @param self GPIO request config object to manipulate.
 * @param event_buffer_size New event buffer size.
 * @note The kernel may adjust the value if it's too high. If set to 0, the
 *       default value will be used.
 */
void g_gpiod_request_config_set_event_buffer_size(GPIODRequestConfig *self,
						  guint event_buffer_size);


/**
 * @brief Get the edge event buffer size for the request config.
 * @param self GPIO request config object to manipulate.
 * @return Edge event buffer size setting from the request config.
 */
guint g_gpiod_request_config_get_event_buffer_size(GPIODRequestConfig *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_REQUEST_CONFIG_H__ */
