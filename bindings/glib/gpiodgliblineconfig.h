/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-config.h
 */

#ifndef __GPIODGLIB_LINE_CONFIG_H__
#define __GPIODGLIB_LINE_CONFIG_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiodglib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "gpiodgliblinesettings.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibLineConfig, gpiodglib_line_config,
		     GPIODGLIB, LINE_CONFIG, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
 * @{
 *
 * @defgroup gobject_line_config GPIO line config GObject
 * @{
 *
 * The line-config object contains the configuration for lines that can be
 * used in two cases:
 *  - when making a line request
 *  - when reconfiguring a set of already requested lines.
 */

/**
 * @brief Get the GObject type for the GPIO line-config.
 */
#define GPIODGLIB_LINE_CONFIG_TYPE (gpiodglib_line_config_get_type())

/**
 * @brief Cast a GObject to a GPIO line-config concrete GObject.
 */
#define GPIODGLIB_LINE_CONFIG_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_LINE_CONFIG_TYPE, \
				    GpiodglibLineConfig))

/**
 * @brief Create a new line config object.
 * @return Empty line config object.
 */
GpiodglibLineConfig *gpiodglib_line_config_new(void);

/**
 * @brief Reset the line config object.
 * @param self GPIO line config to manipulate.
 */
void gpiodglib_line_config_reset(GpiodglibLineConfig *self);

/**
 * @brief Add line settings for a set of offsets.
 * @param self GPIO line config to manipulate.
 * @param offsets GArray of offsets for which to apply the settings.
 * @param settings Line settings to apply.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean
gpiodglib_line_config_add_line_settings(GpiodglibLineConfig *self,
					const GArray *offsets,
					GpiodglibLineSettings *settings,
					GError **err);

/**
 * @brief Get line settings for offset.
 * @param self GPIO line config to manipulate.
 * @param offset Offset for which to get line settings.
 * @return New reference to a line settings object.
 */
GpiodglibLineSettings *
gpiodglib_line_config_get_line_settings(GpiodglibLineConfig *self,
					guint offset);

/**
 * @brief Set output values for a number of lines.
 * @param self GPIO line config to manipulate.
 * @param values GArray containing the output values.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on error.
 */
gboolean gpiodglib_line_config_set_output_values(GpiodglibLineConfig *self,
						 const GArray *values,
						 GError **err);

/**
 * @brief Get configured offsets.
 * @param self GPIO line config to manipulate.
 * @return GArray containing the offsets for which configuration has been set.
 */
GArray *gpiodglib_line_config_get_configured_offsets(GpiodglibLineConfig *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_LINE_CONFIG_H__ */
