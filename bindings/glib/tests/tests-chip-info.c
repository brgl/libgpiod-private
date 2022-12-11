// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiod-test.h>
#include <gpiod-test-common.h>
#include <gpiosim-glib.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/chip-info"

GPIOD_TEST_CASE(get_name)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new(NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODChipInfo) info = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	info = gpiod_glib_test_chip_get_info_or_fail(chip);

	g_assert_cmpstr(g_gpiod_chip_info_get_name(info), ==,
			g_gpiosim_chip_get_name(sim));
}

GPIOD_TEST_CASE(get_label)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("label", "foobar",
							NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODChipInfo) info = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	info = gpiod_glib_test_chip_get_info_or_fail(chip);

	g_assert_cmpstr(g_gpiod_chip_info_get_label(info), ==, "foobar");
}

GPIOD_TEST_CASE(get_num_lines)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 16, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODChipInfo) info = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	info = gpiod_glib_test_chip_get_info_or_fail(chip);

	g_assert_cmpuint(g_gpiod_chip_info_get_num_lines(info), ==, 16);
}
