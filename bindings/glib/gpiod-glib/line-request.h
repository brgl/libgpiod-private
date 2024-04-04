/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-request.h
 */

#ifndef __GPIOD_GLIB_LINE_REQUEST_H__
#define __GPIOD_GLIB_LINE_REQUEST_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODLineRequest, g_gpiod_line_request,
		     G_GPIOD, LINE_REQUEST, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_line_request GPIO line-request GObject
 * @{
 *
 * Line request object allows interacting with a set of requested GPIO lines.
 */

/**
 * @brief Get the GObject type for the GPIO line-request.
 */
#define G_GPIOD_LINE_REQUEST_TYPE (g_gpiod_line_request_get_type())

/**
 * @brief Cast a GObject to a GPIO line-request concrete GObject.
 */
#define G_GPIOD_LINE_REQUEST_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_LINE_REQUEST_TYPE, \
				    GPIODLineRequest))

/**
 * @brief Release the requested lines and free all associated resources.
 * @param self GPIO line request object to manipulate.
 */
void g_gpiod_line_request_release(GPIODLineRequest *self);

/**
 * @brief Check if this request was released.
 * @param self GPIO line request object to manipulate.
 * @return TRUE if this request was released and is no longer valid, FALSE
 *         otherwise.
 */
gboolean g_gpiod_line_request_is_released(GPIODLineRequest *self);

/**
 * @brief Get the name of the chip this request was made on.
 * @param self GPIO line request object to manipulate.
 * @return Name the GPIO chip device.
 */
const gchar *g_gpiod_line_request_get_chip_name(GPIODLineRequest *self);

/**
 * @brief Get the offsets of the lines in the request.
 * @param self GPIO line request object to manipulate.
 * @return Array containing the requested offsets.
 */
GArray *g_gpiod_line_request_get_requested_offsets(GPIODLineRequest *self);

/**
 * @brief Update the configuration of lines associated with a line request.
 * @param self GPIO line request object to manipulate.
 * @param config New line config to apply.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 * @note The new line configuration completely replaces the old.
 * @note Any requested lines without overrides are configured to the requested
 *       defaults.
 * @note Any configured overrides for lines that have not been requested
 *       are silently ignored.
 */
gboolean g_gpiod_line_request_reconfigure_lines(GPIODLineRequest *self,
						GPIODLineConfig *config,
						GError **err);

/**
 * @brief Get the value of a single requested line.
 * @param self GPIO line request object to manipulate.
 * @param offset The offset of the line of which the value should be read.
 * @param value Return location for the value.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean
g_gpiod_line_request_get_value(GPIODLineRequest *self, guint offset,
			       GPIODLineValue *value, GError **err);

/**
 * @brief Get the values of a subset of requested lines.
 * @param self GPIO line request object to manipulate.
 * @param offsets Array of offsets identifying the subset of requested lines
 *                from which to read values.
 * @param values Array in which the values will be stored. Can be NULL in which
 *               case a new array will be created and its location stored here.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_request_get_values_subset(GPIODLineRequest *self,
						const GArray *offsets,
						GArray **values,
						GError **err);

/**
 * @brief Get the values of all requested lines.
 * @param self GPIO line request object to manipulate.
 * @param values Array in which the values will be stored. Can be NULL in which
 *               case a new array will be created and its location stored here.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_request_get_values(GPIODLineRequest *self,
					 GArray **values, GError **err);

/**
 * @brief Set the value of a single requested line.
 * @param self GPIO line request object to manipulate.
 * @param offset The offset of the line for which the value should be set.
 * @param value Value to set.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_request_set_value(GPIODLineRequest *self, guint offset,
					GPIODLineValue value, GError **err);

/**
 * @brief Set the values of a subset of requested lines.
 * @param self GPIO line request object to manipulate.
 * @param offsets Array of offsets identifying the requested lines for
 *                which to set values.
 * @param values Array in which the values will be stored. Can be NULL in which
 *               case a new array will be created and its location stored here.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_request_set_values_subset(GPIODLineRequest *self,
						const GArray *offsets,
						const GArray *values,
						GError **err);

/**
 * @brief Set the values of all lines associated with a request.
 * @param self GPIO line request object to manipulate.
 * @param values Array containing the values to set. Must be sized to
 *               contain the number of values equal to the number of requested
 *               lines. Each value is associated with the line identified by
 *               the corresponding entry in the offset array filled by
 *               ::g_gpiod_line_request_get_requested_offsets.
 *               case a new array will be created and its location stored here.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_request_set_values(GPIODLineRequest *self,
					 GArray *values, GError **err);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_LINE_REQUEST_H__ */
