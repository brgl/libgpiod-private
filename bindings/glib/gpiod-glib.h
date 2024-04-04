/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file gpiod-glib.h
 */

#ifndef __GPIODGLIB_H__
#define __GPIODGLIB_H__

/**
 * @defgroup gpiodglib GLib bindings
 *
 * GLib bindings for libgpiod representing all data structures using the
 * GObject type system.
 */

#define __GPIODGLIB_INSIDE__
#include "gpiod-glib/chip.h"
#include "gpiod-glib/chip-info.h"
#include "gpiod-glib/edge-event.h"
#include "gpiod-glib/error.h"
#include "gpiod-glib/generated-enums.h"
#include "gpiod-glib/info-event.h"
#include "gpiod-glib/line-config.h"
#include "gpiod-glib/line-info.h"
#include "gpiod-glib/line-request.h"
#include "gpiod-glib/line-settings.h"
#include "gpiod-glib/misc.h"
#include "gpiod-glib/request-config.h"
#undef __GPIODGLIB_INSIDE__

#endif /* __GPIODGLIB_H__ */
