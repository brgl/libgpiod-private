/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file chip-info.h
 */

#ifndef __GPIODGLIB_CHIP_INFO_H__
#define __GPIODGLIB_CHIP_INFO_H__

#if !defined (__GPIODGLIB_INSIDE__)
#error "Only <gpiodglib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GpiodglibChipInfo, gpiodglib_chip_info,
		     GPIODGLIB, CHIP_INFO, GObject);

/**
 * @endcond
 *
 * @ingroup gpiodglib
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
#define GPIODGLIB_CHIP_INFO_TYPE (gpiodglib_chip_info_get_type())

/**
 * @brief Cast a GObject to a GPIO chip-info concrete GObject.
 */
#define GPIODGLIB_CHIP_INFO_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GPIODGLIB_CHIP_INFO_TYPE, \
				    GpiodglibChipInfo))

/**
 * @brief Get the name of the chip as represented in the kernel.
 * @param self GPIO chip info object to manipulate.
 * @return Valid pointer to a human-readable string containing the chip name.
 *         The returned string is a copy and must be freed by the caller using
 *         g_free().
 */
gchar * G_GNUC_WARN_UNUSED_RESULT
gpiodglib_chip_info_dup_name(GpiodglibChipInfo *self);

/**
 * @brief Get the label of the chip as represented in the kernel.
 * @param self GPIO chip info object to manipulate.
 * @return Valid pointer to a human-readable string containing the chip label.
 *         The returned string is a copy and must be freed by the caller using
 *         g_free().
 */
gchar * G_GNUC_WARN_UNUSED_RESULT
gpiodglib_chip_info_dup_label(GpiodglibChipInfo *self);

/**
 * @brief Get the number of lines exposed by the chip.
 * @param self GPIO chip info object to manipulate.
 * @return Number of GPIO lines.
 */
guint gpiodglib_chip_info_get_num_lines(GpiodglibChipInfo *self);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIODGLIB_CHIP_INFO_H__ */
