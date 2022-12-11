/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file error.h
 */

#ifndef __GPIOD_GLIB_ERROR_H__
#define __GPIOD_GLIB_ERROR_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

/**
 * @ingroup gpiod_glib
 * @{
 */

/**
 * @brief Get the libgpiod GObject error domain.
 */
#define G_GPIOD_ERROR g_gpiod_error_quark()

/**
 * @}
 *
 * @cond
 */

typedef enum {
	G_GPIOD_ERR_FAILED = 1,
	G_GPIOD_ERR_CHIP_CLOSED,
	G_GPIOD_ERR_REQUEST_RELEASED,
	G_GPIOD_ERR_PERM,
	G_GPIOD_ERR_NOENT,
	G_GPIOD_ERR_INTR,
	G_GPIOD_ERR_IO,
	G_GPIOD_ERR_NXIO,
	G_GPIOD_ERR_E2BIG,
	G_GPIOD_ERR_BADFD,
	G_GPIOD_ERR_CHILD,
	G_GPIOD_ERR_AGAIN,
	G_GPIOD_ERR_NOMEM,
	G_GPIOD_ERR_ACCES,
	G_GPIOD_ERR_FAULT,
	G_GPIOD_ERR_BUSY,
	G_GPIOD_ERR_EXIST,
	G_GPIOD_ERR_NODEV,
	G_GPIOD_ERR_INVAL,
	G_GPIOD_ERR_NOTTY,
	G_GPIOD_ERR_PIPE,
} GPIODError;

GQuark g_gpiod_error_quark(void);

/**
 * @endcond
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_ERROR_H__ */
