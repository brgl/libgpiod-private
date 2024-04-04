/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file misc.h
 */

#ifndef __GPIOD_GLIB_MISC_H__
#define __GPIOD_GLIB_MISC_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

/**
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gpiod_glib_misc Misc functions.
 * @{
 */

/**
 * @brief Check if the file pointed to by path is a GPIO chip character device.
 * @param path Path to check.
 * @return TRUE if the file exists and is either a GPIO chip character device
 *         or a symbolic link to one, FALSE otherwise.
 */
gboolean g_gpiod_is_gpiochip_device(const gchar *path);

/**
 * @brief Get the API version of the library as a human-readable string.
 * @return A valid pointer to a human-readable string containing the library
 *         version. The pointer is valid for the lifetime of the program and
 *         must not be freed by the caller.
 */
const gchar *g_gpiod_api_version(void);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_MISC_H__ */
