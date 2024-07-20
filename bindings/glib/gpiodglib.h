/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file gpiodglib.h
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
#include "gpiodglibchip.h"
#include "gpiodglibchipinfo.h"
#include "gpiodglibedgeevent.h"
#include "gpiodgliberror.h"
#include "gpiodglibgeneratedenums.h"
#include "gpiodglibinfoevent.h"
#include "gpiodgliblineconfig.h"
#include "gpiodgliblineinfo.h"
#include "gpiodgliblinerequest.h"
#include "gpiodgliblinesettings.h"
#include "gpiodglibmisc.h"
#include "gpiodglibrequestconfig.h"
#undef __GPIODGLIB_INSIDE__

#endif /* __GPIODGLIB_H__ */
