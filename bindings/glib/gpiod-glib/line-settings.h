/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-settings.h
 */

#ifndef __GPIODGLIB_LINE_SETTINGS_H__
#define __GPIODGLIB_LINE_SETTINGS_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "line.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibLineSettings, gpiodglib_line_settings,
		     GPIODGLIB, LINE_SETTINGS, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
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
#define GPIODGLIB_LINE_SETTINGS_TYPE (gpiodglib_line_settings_get_type())

/**
 * @brief Cast a GObject to a GPIO line-settings concrete GObject.
 */
#define GPIODGLIB_LINE_SETTINGS_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_LINE_SETTINGS_TYPE, \
				    GpiodglibLineSettings))

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
GpiodglibLineSettings *
gpiodglib_line_settings_new(const gchar *first_prop, ...);

/**
 * @brief Reset the line settings object to its default values.
 * @param self Line settings object to manipulate.
 */
void gpiodglib_line_settings_reset(GpiodglibLineSettings *self);

/**
 * @brief Set direction.
 * @param self Line settings object to manipulate.
 * @param direction New direction.
 */
void gpiodglib_line_settings_set_direction(GpiodglibLineSettings *self,
					   GpiodglibLineDirection direction);

/**
 * @brief Get direction.
 * @param self Line settings object to manipulate.
 * @return Current direction.
 */
GpiodglibLineDirection
gpiodglib_line_settings_get_direction(GpiodglibLineSettings *self);

/**
 * @brief Set edge detection.
 * @param self Line settings object to manipulate.
 * @param edge New edge detection setting.
 */
void gpiodglib_line_settings_set_edge_detection(GpiodglibLineSettings *self,
						GpiodglibLineEdge edge);

/**
 * @brief Get edge detection.
 * @param self Line settings object to manipulate.
 * @return Current edge detection setting.
 */
GpiodglibLineEdge
gpiodglib_line_settings_get_edge_detection(GpiodglibLineSettings *self);

/**
 * @brief Set bias.
 * @param self Line settings object to manipulate.
 * @param bias New bias.
 */
void gpiodglib_line_settings_set_bias(GpiodglibLineSettings *self,
				      GpiodglibLineBias bias);

/**
 * @brief Get bias.
 * @param self Line settings object to manipulate.
 * @return Current bias setting.
 */
GpiodglibLineBias gpiodglib_line_settings_get_bias(GpiodglibLineSettings *self);

/**
 * @brief Set drive.
 * @param self Line settings object to manipulate.
 * @param drive New drive setting.
 */
void gpiodglib_line_settings_set_drive(GpiodglibLineSettings *self,
				       GpiodglibLineDrive drive);

/**
 * @brief Get drive.
 * @param self Line settings object to manipulate.
 * @return Current drive setting.
 */
GpiodglibLineDrive
gpiodglib_line_settings_get_drive(GpiodglibLineSettings *self);

/**
 * @brief Set active-low setting.
 * @param self Line settings object to manipulate.
 * @param active_low New active-low setting.
 */
void gpiodglib_line_settings_set_active_low(GpiodglibLineSettings *self,
					    gboolean active_low);

/**
 * @brief Get active-low setting.
 * @param self Line settings object to manipulate.
 * @return TRUE if active-low is enabled, FALSE otherwise.
 */
gboolean gpiodglib_line_settings_get_active_low(GpiodglibLineSettings *self);

/**
 * @brief Set debounce period.
 * @param self Line settings object to manipulate.
 * @param period New debounce period in microseconds.
 */
void gpiodglib_line_settings_set_debounce_period_us(GpiodglibLineSettings *self,
						    GTimeSpan period);

/**
 * @brief Get debounce period.
 * @param self Line settings object to manipulate.
 * @return Current debounce period in microseconds.
 */
GTimeSpan
gpiodglib_line_settings_get_debounce_period_us(GpiodglibLineSettings *self);

/**
 * @brief Set event clock.
 * @param self Line settings object to manipulate.
 * @param event_clock New event clock.
 */
void gpiodglib_line_settings_set_event_clock(GpiodglibLineSettings *self,
					     GpiodglibLineClock event_clock);

/**
 * @brief Get event clock setting.
 * @param self Line settings object to manipulate.
 * @return Current event clock setting.
 */
GpiodglibLineClock
gpiodglib_line_settings_get_event_clock(GpiodglibLineSettings *self);

/**
 * @brief Set the output value.
 * @param self Line settings object to manipulate.
 * @param value New output value.
 */
void gpiodglib_line_settings_set_output_value(GpiodglibLineSettings *self,
					      GpiodglibLineValue value);

/**
 * @brief Get the output value.
 * @param self Line settings object to manipulate.
 * @return Current output value.
 */
GpiodglibLineValue
gpiodglib_line_settings_get_output_value(GpiodglibLineSettings *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_LINE_SETTINGS_H__ */
