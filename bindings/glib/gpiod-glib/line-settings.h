/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-settings.h
 */

#ifndef __GPIOD_GLIB_LINE_SETTINGS_H__
#define __GPIOD_GLIB_LINE_SETTINGS_H__

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

G_DECLARE_FINAL_TYPE(GPIODLineSettings, g_gpiod_line_settings,
		     G_GPIOD, LINE_SETTINGS, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_line_settings GPIO line-settings GObject
 * @{
 *
 * Line settings object contains a set of line properties that can be used
 * when requesting lines or reconfiguring an existing request.
 */

/**
 * @brief Get the GObject type for the GPIO line-settings.
 */
#define G_GPIOD_LINE_SETTINGS_TYPE (g_gpiod_line_settings_get_type())

/**
 * @brief Cast a GObject to a GPIO line-settings concrete GObject.
 */
#define G_GPIOD_LINE_SETTINGS_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_LINE_SETTINGS_TYPE, \
				    GPIODLineSettings))

/**
 * @brief Create a new line settings object.
 * @param first_prop Name of the first property to set.
 * @return New line settings object.
 *
 * The constructor allows to set object's properties when it's first created
 * instead of having to build an empty object and then call mutators separately.
 *
 * Currently supported properties are: `direction`, `edge-detection`, `bias`,
 * `drive`, `debounce-period-us`, `active-low`, 'event-clock` and
 * `output-value`.
 */
GPIODLineSettings *g_gpiod_line_settings_new(const gchar *first_prop, ...);

/**
 * @brief Reset the line settings object to its default values.
 * @param self Line settings object to manipulate.
 */
void g_gpiod_line_settings_reset(GPIODLineSettings *self);

/**
 * @brief Set direction.
 * @param self Line settings object to manipulate.
 * @param direction New direction.
 */
void g_gpiod_line_settings_set_direction(GPIODLineSettings *self,
					 GPIODLineDirection direction);

/**
 * @brief Get direction.
 * @param self Line settings object to manipulate.
 * @return Current direction.
 */
GPIODLineDirection g_gpiod_line_settings_get_direction(GPIODLineSettings *self);

/**
 * @brief Set edge detection.
 * @param self Line settings object to manipulate.
 * @param edge New edge detection setting.
 */
void g_gpiod_line_settings_set_edge_detection(GPIODLineSettings *self,
					      GPIODLineEdge edge);

/**
 * @brief Get edge detection.
 * @param self Line settings object to manipulate.
 * @return Current edge detection setting.
 */
GPIODLineEdge g_gpiod_line_settings_get_edge_detection(GPIODLineSettings *self);

/**
 * @brief Set bias.
 * @param self Line settings object to manipulate.
 * @param bias New bias.
 */
void g_gpiod_line_settings_set_bias(GPIODLineSettings *self,
				    GPIODLineBias bias);

/**
 * @brief Get bias.
 * @param self Line settings object to manipulate.
 * @return Current bias setting.
 */
GPIODLineBias g_gpiod_line_settings_get_bias(GPIODLineSettings *self);

/**
 * @brief Set drive.
 * @param self Line settings object to manipulate.
 * @param drive New drive setting.
 */
void g_gpiod_line_settings_set_drive(GPIODLineSettings *self,
				     GPIODLineDrive drive);

/**
 * @brief Get drive.
 * @param self Line settings object to manipulate.
 * @return Current drive setting.
 */
GPIODLineDrive g_gpiod_line_settings_get_drive(GPIODLineSettings *self);

/**
 * @brief Set active-low setting.
 * @param self Line settings object to manipulate.
 * @param active_low New active-low setting.
 */
void g_gpiod_line_settings_set_active_low(GPIODLineSettings *self,
					  gboolean active_low);

/**
 * @brief Get active-low setting.
 * @param self Line settings object to manipulate.
 * @return TRUE if active-low is enabled, FALSE otherwise.
 */
gboolean g_gpiod_line_settings_get_active_low(GPIODLineSettings *self);

/**
 * @brief Set debounce period.
 * @param self Line settings object to manipulate.
 * @param period New debounce period in microseconds.
 */
void g_gpiod_line_settings_set_debounce_period_us(GPIODLineSettings *self,
						  GTimeSpan period);

/**
 * @brief Get debounce period.
 * @param self Line settings object to manipulate.
 * @return Current debounce period in microseconds.
 */
GTimeSpan g_gpiod_line_settings_get_debounce_period_us(GPIODLineSettings *self);

/**
 * @brief Set event clock.
 * @param self Line settings object to manipulate.
 * @param event_clock New event clock.
 */
void g_gpiod_line_settings_set_event_clock(GPIODLineSettings *self,
					   GPIODLineClock event_clock);

/**
 * @brief Get event clock setting.
 * @param self Line settings object to manipulate.
 * @return Current event clock setting.
 */
GPIODLineClock g_gpiod_line_settings_get_event_clock(GPIODLineSettings *self);

/**
 * @brief Set the output value.
 * @param self Line settings object to manipulate.
 * @param value New output value.
 */
void g_gpiod_line_settings_set_output_value(GPIODLineSettings *self,
					    GPIODLineValue value);

/**
 * @brief Get the output value.
 * @param self Line settings object to manipulate.
 * @return Current output value.
 */
GPIODLineValue g_gpiod_line_settings_get_output_value(GPIODLineSettings *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_LINE_SETTINGS_H__ */
