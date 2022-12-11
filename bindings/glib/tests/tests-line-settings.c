// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiod-test.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/line-settings"

GPIOD_TEST_CASE(default_config)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings),
			==, G_GPIOD_LINE_EDGE_NONE);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_PUSH_PULL);
	g_assert_false(g_gpiod_line_settings_get_active_low(settings));
	g_assert_cmpint(g_gpiod_line_settings_get_debounce_period_us(settings),
			==, 0);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_MONOTONIC);
	g_assert_cmpint(g_gpiod_line_settings_get_output_value(settings), ==,
			G_GPIOD_LINE_VALUE_INACTIVE);
}

GPIOD_TEST_CASE(set_direction)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_direction(settings,
					    G_GPIOD_LINE_DIRECTION_INPUT);
	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_INPUT);

	g_gpiod_line_settings_set_direction(settings,
					    G_GPIOD_LINE_DIRECTION_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_AS_IS);

	g_gpiod_line_settings_set_direction(settings,
					    G_GPIOD_LINE_DIRECTION_OUTPUT);
	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_OUTPUT);
}

GPIOD_TEST_CASE(set_edge_detection)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_edge_detection(settings,
						 G_GPIOD_LINE_EDGE_BOTH);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_BOTH);

	g_gpiod_line_settings_set_edge_detection(settings,
						 G_GPIOD_LINE_EDGE_NONE);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_NONE);

	g_gpiod_line_settings_set_edge_detection(settings,
						 G_GPIOD_LINE_EDGE_FALLING);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_FALLING);

	g_gpiod_line_settings_set_edge_detection(settings,
						 G_GPIOD_LINE_EDGE_RISING);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_RISING);
}

GPIOD_TEST_CASE(set_bias)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_bias(settings, G_GPIOD_LINE_BIAS_DISABLED);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_DISABLED);

	g_gpiod_line_settings_set_bias(settings, G_GPIOD_LINE_BIAS_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_AS_IS);

	g_gpiod_line_settings_set_bias(settings, G_GPIOD_LINE_BIAS_PULL_DOWN);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_PULL_DOWN);

	g_gpiod_line_settings_set_bias(settings, G_GPIOD_LINE_BIAS_PULL_UP);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_PULL_UP);
}

GPIOD_TEST_CASE(set_drive)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_drive(settings,
					G_GPIOD_LINE_DRIVE_OPEN_DRAIN);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_OPEN_DRAIN);

	g_gpiod_line_settings_set_drive(settings,
					G_GPIOD_LINE_DRIVE_PUSH_PULL);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_PUSH_PULL);

	g_gpiod_line_settings_set_drive(settings,
					G_GPIOD_LINE_DRIVE_OPEN_SOURCE);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_OPEN_SOURCE);
}

GPIOD_TEST_CASE(set_active_low)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_active_low(settings, TRUE);
	g_assert_true(g_gpiod_line_settings_get_active_low(settings));

	g_gpiod_line_settings_set_active_low(settings, FALSE);
	g_assert_false(g_gpiod_line_settings_get_active_low(settings));
}

GPIOD_TEST_CASE(set_debounce_period)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_debounce_period_us(settings, 4000);
	g_assert_cmpint(g_gpiod_line_settings_get_debounce_period_us(settings),
			==, 4000);
}

GPIOD_TEST_CASE(set_event_clock)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_event_clock(settings,
					       G_GPIOD_LINE_CLOCK_MONOTONIC);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_MONOTONIC);

	g_gpiod_line_settings_set_event_clock(settings,
					      G_GPIOD_LINE_CLOCK_REALTIME);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_REALTIME);

	g_gpiod_line_settings_set_event_clock(settings,
					      G_GPIOD_LINE_CLOCK_HTE);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_HTE);
}

GPIOD_TEST_CASE(set_output_value)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_output_value(settings,
					       G_GPIOD_LINE_VALUE_ACTIVE);
	g_assert_cmpint(g_gpiod_line_settings_get_output_value(settings), ==,
			G_GPIOD_LINE_VALUE_ACTIVE);

	g_gpiod_line_settings_set_output_value(settings,
					       G_GPIOD_LINE_VALUE_INACTIVE);
	g_assert_cmpint(g_gpiod_line_settings_get_output_value(settings), ==,
			G_GPIOD_LINE_VALUE_INACTIVE);
}

GPIOD_TEST_CASE(reset_settings)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(NULL);

	g_gpiod_line_settings_set_direction(settings,
					    G_GPIOD_LINE_DIRECTION_INPUT);
	g_gpiod_line_settings_set_edge_detection(settings,
						 G_GPIOD_LINE_EDGE_BOTH);
	g_gpiod_line_settings_set_debounce_period_us(settings, 2000);
	g_gpiod_line_settings_set_event_clock(settings,
					      G_GPIOD_LINE_CLOCK_REALTIME);

	g_gpiod_line_settings_reset(settings);

	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_NONE);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_AS_IS);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_PUSH_PULL);
	g_assert_false(g_gpiod_line_settings_get_active_low(settings));
	g_assert_cmpint(g_gpiod_line_settings_get_debounce_period_us(settings),
			==, 0);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_MONOTONIC);
	g_assert_cmpint(g_gpiod_line_settings_get_output_value(settings), ==,
			G_GPIOD_LINE_VALUE_INACTIVE);
}

GPIOD_TEST_CASE(set_props_in_constructor)
{
	g_autoptr(GPIODLineSettings) settings = NULL;

	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_INPUT,
			"edge-detection", G_GPIOD_LINE_EDGE_BOTH,
			"active-low", TRUE,
			"debounce-period-us", (GTimeSpan)3000,
			"bias", G_GPIOD_LINE_BIAS_PULL_UP,
			"event-clock", G_GPIOD_LINE_CLOCK_REALTIME,
			NULL);

	g_assert_cmpint(g_gpiod_line_settings_get_direction(settings), ==,
			G_GPIOD_LINE_DIRECTION_INPUT);
	g_assert_cmpint(g_gpiod_line_settings_get_edge_detection(settings), ==,
			G_GPIOD_LINE_EDGE_BOTH);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(settings), ==,
			G_GPIOD_LINE_BIAS_PULL_UP);
	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DRIVE_PUSH_PULL);
	g_assert_true(g_gpiod_line_settings_get_active_low(settings));
	g_assert_cmpint(g_gpiod_line_settings_get_debounce_period_us(settings),
			==, 3000);
	g_assert_cmpint(g_gpiod_line_settings_get_event_clock(settings), ==,
			G_GPIOD_LINE_CLOCK_REALTIME);
	g_assert_cmpint(g_gpiod_line_settings_get_output_value(settings), ==,
			G_GPIOD_LINE_VALUE_INACTIVE);
}
