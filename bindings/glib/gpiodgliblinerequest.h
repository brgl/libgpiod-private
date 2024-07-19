/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-request.h
 */

#ifndef __GPIODGLIB_LINE_REQUEST_H__
#define __GPIODGLIB_LINE_REQUEST_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiodglib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibLineRequest, gpiodglib_line_request,
		     GPIODGLIB, LINE_REQUEST, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
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
#define GPIODGLIB_LINE_REQUEST_TYPE (gpiodglib_line_request_get_type())

/**
 * @brief Cast a GObject to a GPIO line-request concrete GObject.
 */
#define GPIODGLIB_LINE_REQUEST_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_LINE_REQUEST_TYPE, \
				    GpiodglibLineRequest))

/**
 * @brief Release the requested lines and free all associated resources.
 * @param self GPIO line request object to manipulate.
 */
void gpiodglib_line_request_release(GpiodglibLineRequest *self);

/**
 * @brief Check if this request was released.
 * @param self GPIO line request object to manipulate.
 * @return TRUE if this request was released and is no longer valid, FALSE
 *         otherwise.
 */
gboolean gpiodglib_line_request_is_released(GpiodglibLineRequest *self);

/**
 * @brief Get the name of the chip this request was made on.
 * @param self GPIO line request object to manipulate.
 * @return Name the GPIO chip device. The string is a copy and must be freed
 *         by the caller using g_free().
 */
gchar * G_GNUC_WARN_UNUSED_RESULT
gpiodglib_line_request_dup_chip_name(GpiodglibLineRequest *self);

/**
 * @brief Get the offsets of the lines in the request.
 * @param self GPIO line request object to manipulate.
 * @return Array containing the requested offsets.
 */
GArray *
gpiodglib_line_request_get_requested_offsets(GpiodglibLineRequest *self);

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
gboolean gpiodglib_line_request_reconfigure_lines(GpiodglibLineRequest *self,
						  GpiodglibLineConfig *config,
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
gpiodglib_line_request_get_value(GpiodglibLineRequest *self, guint offset,
				 GpiodglibLineValue *value, GError **err);

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
gboolean gpiodglib_line_request_get_values_subset(GpiodglibLineRequest *self,
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
gboolean gpiodglib_line_request_get_values(GpiodglibLineRequest *self,
					   GArray **values, GError **err);

/**
 * @brief Set the value of a single requested line.
 * @param self GPIO line request object to manipulate.
 * @param offset The offset of the line for which the value should be set.
 * @param value Value to set.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean
gpiodglib_line_request_set_value(GpiodglibLineRequest *self, guint offset,
				 GpiodglibLineValue value, GError **err);

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
gboolean gpiodglib_line_request_set_values_subset(GpiodglibLineRequest *self,
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
 *               ::gpiodglib_line_request_get_requested_offsets.
 *               case a new array will be created and its location stored here.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean gpiodglib_line_request_set_values(GpiodglibLineRequest *self,
					   GArray *values, GError **err);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_LINE_REQUEST_H__ */
