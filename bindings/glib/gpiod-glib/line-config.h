/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line-config.h
 */

#ifndef __GPIOD_GLIB_LINE_CONFIG_H__
#define __GPIOD_GLIB_LINE_CONFIG_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "line-settings.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODLineConfig, g_gpiod_line_config,
		     G_GPIOD, LINE_CONFIG, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
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
#define G_GPIOD_LINE_CONFIG_TYPE (g_gpiod_line_config_get_type())

/**
 * @brief Cast a GObject to a GPIO line-config concrete GObject.
 */
#define G_GPIOD_LINE_CONFIG_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_LINE_CONFIG_TYPE, \
				    GPIODLineConfig))

/**
 * @brief Create a new line config object.
 * @return Empty line config object.
 */
GPIODLineConfig *g_gpiod_line_config_new(void);

/**
 * @brief Reset the line config object.
 * @param self GPIO line config to manipulate.
 */
void g_gpiod_line_config_reset(GPIODLineConfig *self);

/**
 * @brief Add line settings for a set of offsets.
 * @param self GPIO line config to manipulate.
 * @param offsets GArray of offsets for which to apply the settings.
 * @param settings Line settings to apply.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean g_gpiod_line_config_add_line_settings(GPIODLineConfig *self,
					       const GArray *offsets,
					       GPIODLineSettings *settings,
					       GError **err);

/**
 * @brief Get line settings for offset.
 * @param self GPIO line config to manipulate.
 * @param offset Offset for which to get line settings.
 * @return New reference to a line settings object.
 */
GPIODLineSettings *
g_gpiod_line_config_get_line_settings(GPIODLineConfig *self, guint offset);

/**
 * @brief Set output values for a number of lines.
 * @param self GPIO line config to manipulate.
 * @param values GArray containing the output values.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on error.
 */
gboolean g_gpiod_line_config_set_output_values(GPIODLineConfig *self,
					       const GArray *values,
					       GError **err);

/**
 * @brief Get configured offsets.
 * @param self GPIO line config to manipulate.
 * @return GArray containing the offsets for which configuration has been set.
 */
GArray *g_gpiod_line_config_get_configured_offsets(GPIODLineConfig *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_LINE_CONFIG_H__ */
