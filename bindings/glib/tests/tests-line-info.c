// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiod-test.h>
#include <gpiod-test-common.h>
#include <gpiosim-glib.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/line-info"

GPIOD_TEST_CASE(get_line_info_good)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	info = gpiod_glib_test_chip_get_line_info_or_fail(chip, 3);

	g_assert_cmpuint(g_gpiod_line_info_get_offset(info), ==, 3);
}

GPIOD_TEST_CASE(get_line_info_offset_out_of_range)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;
	g_autoptr(GError) err = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	info = g_gpiod_chip_get_line_info(chip, 8, &err);
	g_assert_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL);
}

GPIOD_TEST_CASE(line_info_basic_properties)
{
	static const GPIOSimLineName names[] = {
		{ .offset = 1, .name = "foo", },
		{ .offset = 2, .name = "bar", },
		{ .offset = 4, .name = "baz", },
		{ .offset = 5, .name = "xyz", },
		{ }
	};

	static const GPIOSimHog hogs[] = {
		{
			.offset = 3,
			.name = "hog3",
			.direction = G_GPIOSIM_DIRECTION_OUTPUT_HIGH,
		},
		{
			.offset = 4,
			.name = "hog4",
			.direction = G_GPIOSIM_DIRECTION_OUTPUT_LOW,
		},
		{ }
	};

	g_autoptr(GVariant) vnames = g_gpiosim_package_line_names(names);
	g_autoptr(GVariant) vhogs = g_gpiosim_package_hogs(hogs);
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8,
							"line-names", vnames,
							"hogs", vhogs,
							NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info4 = NULL;
	g_autoptr(GPIODLineInfo) info6 = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
				g_gpiosim_chip_get_dev_path(sim));
	info4 = gpiod_glib_test_chip_get_line_info_or_fail(chip, 4);
	info6 = gpiod_glib_test_chip_get_line_info_or_fail(chip, 6);

	g_assert_cmpuint(g_gpiod_line_info_get_offset(info4), ==, 4);
	g_assert_cmpstr(g_gpiod_line_info_get_name(info4), ==, "baz");
	g_assert_cmpstr(g_gpiod_line_info_get_consumer(info4), ==, "hog4");
	g_assert_true(g_gpiod_line_info_is_used(info4));
	g_assert_cmpint(g_gpiod_line_info_get_direction(info4), ==,
			G_GPIOD_LINE_DIRECTION_OUTPUT);
	g_assert_cmpint(g_gpiod_line_info_get_edge_detection(info4), ==,
			G_GPIOD_LINE_EDGE_NONE);
	g_assert_false(g_gpiod_line_info_is_active_low(info4));
	g_assert_cmpint(g_gpiod_line_info_get_bias(info4), ==,
			G_GPIOD_LINE_BIAS_UNKNOWN);
	g_assert_cmpint(g_gpiod_line_info_get_drive(info4), ==,
			G_GPIOD_LINE_DRIVE_PUSH_PULL);
	g_assert_cmpint(g_gpiod_line_info_get_event_clock(info4), ==,
			G_GPIOD_LINE_CLOCK_MONOTONIC);
	g_assert_false(g_gpiod_line_info_is_debounced(info4));
	g_assert_cmpuint(g_gpiod_line_info_get_debounce_period_us(info4), ==,
			 0);
}
