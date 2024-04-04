/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file chip-info.h
 */

#ifndef __GPIOD_GLIB_CHIP_INFO_H__
#define __GPIOD_GLIB_CHIP_INFO_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODChipInfo, g_gpiod_chip_info,
		     G_GPIOD, CHIP_INFO, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_chip_info GPIO chip info GObject
 * @{
 *
 * Represents an immutable snapshot of GPIO chip information.
 */

/**
 * @brief Get the GObject type for the GPIO chip-info.
 */
#define G_GPIOD_CHIP_INFO_TYPE (g_gpiod_chip_info_get_type())

/**
 * @brief Cast a GObject to a GPIO chip-info concrete GObject.
 */
#define G_GPIOD_CHIP_INFO_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_CHIP_INFO_TYPE, \
				    GPIODChipInfo))

/**
 * @brief Get the name of the chip as represented in the kernel.
 * @param self GPIO chip info object to manipulate.
 * @return Valid pointer to a human-readable string containing the chip name.
 *         The string lifetime is tied to the chip info object so the pointer
 *         must not be freed by the caller.
 */
const gchar *g_gpiod_chip_info_get_name(GPIODChipInfo *self);

/**
 * @brief Get the label of the chip as represented in the kernel.
 * @param self GPIO chip info object to manipulate.
 * @return Valid pointer to a human-readable string containing the chip label.
 *         The string lifetime is tied to the chip info object so the pointer
 *         must not be freed by the caller.
 */
const gchar *g_gpiod_chip_info_get_label(GPIODChipInfo *self);

/**
 * @brief Get the number of lines exposed by the chip.
 * @param self GPIO chip info object to manipulate.
 * @return Number of GPIO lines.
 */
guint g_gpiod_chip_info_get_num_lines(GPIODChipInfo *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_CHIP_INFO_H__ */
