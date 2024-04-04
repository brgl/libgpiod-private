/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file line.h
 */

#ifndef __GPIOD_GLIB_LINE_H__
#define __GPIOD_GLIB_LINE_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

/**
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gpiod_glib_line Line definitions
 * @{
 *
 * These defines are used across the API.
 */

/**
 * @brief Logical line state.
 */
typedef enum {
	G_GPIOD_LINE_VALUE_INACTIVE = 0,
	/**< Line is logically inactive. */
	G_GPIOD_LINE_VALUE_ACTIVE = 1,
	/**< Line is logically active. */
} GPIODLineValue;

/**
 * @brief Direction settings.
 */
typedef enum {
	G_GPIOD_LINE_DIRECTION_AS_IS = 1,
	/**< Request the line(s), but don't change direction. */
	G_GPIOD_LINE_DIRECTION_INPUT,
	/**< Direction is input - for reading the value of an externally driven
	 *   GPIO line. */
	G_GPIOD_LINE_DIRECTION_OUTPUT,
	/**< Direction is output - for driving the GPIO line. */
} GPIODLineDirection;

/**
 * @brief Edge detection settings.
 */
typedef enum {
	G_GPIOD_LINE_EDGE_NONE = 1,
	/**< Line edge detection is disabled. */
	G_GPIOD_LINE_EDGE_RISING,
	/**< Line detects rising edge events. */
	G_GPIOD_LINE_EDGE_FALLING,
	/**< Line detects falling edge events. */
	G_GPIOD_LINE_EDGE_BOTH,
	/**< Line detects both rising and falling edge events. */
} GPIODLineEdge;

/**
 * @brief Internal bias settings.
 */
typedef enum {
	G_GPIOD_LINE_BIAS_AS_IS = 1,
	/**< Don't change the bias setting when applying line config. */
	G_GPIOD_LINE_BIAS_UNKNOWN,
	/**< The internal bias state is unknown. */
	G_GPIOD_LINE_BIAS_DISABLED,
	/**< The internal bias is disabled. */
	G_GPIOD_LINE_BIAS_PULL_UP,
	/**< The internal pull-up bias is enabled. */
	G_GPIOD_LINE_BIAS_PULL_DOWN,
	/**< The internal pull-down bias is enabled. */
} GPIODLineBias;

/**
 * @brief Drive settings.
 */
typedef enum {
	G_GPIOD_LINE_DRIVE_PUSH_PULL = 1,
	/**< Drive setting is push-pull. */
	G_GPIOD_LINE_DRIVE_OPEN_DRAIN,
	/**< Line output is open-drain. */
	G_GPIOD_LINE_DRIVE_OPEN_SOURCE,
	/**< Line output is open-source. */
} GPIODLineDrive;

/**
 * @brief Clock settings.
 */
typedef enum {
	G_GPIOD_LINE_CLOCK_MONOTONIC = 1,
	/**< Line uses the monotonic clock for edge event timestamps. */
	G_GPIOD_LINE_CLOCK_REALTIME,
	/**< Line uses the realtime clock for edge event timestamps. */
	G_GPIOD_LINE_CLOCK_HTE,
	/**< Line uses the hardware timestamp engine for event timestamps. */
} GPIODLineClock;

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_LINE_H__ */
