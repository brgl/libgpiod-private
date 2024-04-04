// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiod-test.h>
#include <gpiod-test-common.h>
#include <gpiosim-glib.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/line-config"

GPIOD_TEST_CASE(too_many_lines)
{
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GError) err = NULL;
	gboolean ret;
	guint i;

	settings = g_gpiod_line_settings_new(NULL);
	config = g_gpiod_line_config_new();
	offsets = g_array_new(FALSE, TRUE, sizeof(guint));

	for (i = 0; i < 65; i++)
		g_array_append_val(offsets, i);

	ret = g_gpiod_line_config_add_line_settings(config, offsets,
						    settings, &err);
	g_assert_false(ret);
	g_assert_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_E2BIG);
}

GPIOD_TEST_CASE(get_line_settings)
{
	static const guint offset_vals[] = { 0, 1, 2, 3 };

	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineSettings) retrieved = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;

	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_INPUT,
			"bias", G_GPIOD_LINE_BIAS_PULL_DOWN,
			NULL);
	config = g_gpiod_line_config_new();
	offsets = gpiod_glib_test_array_from_const(offset_vals, 4,
						   sizeof(guint));

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      settings);

	retrieved = gpiod_glib_test_line_config_get_line_settings_or_fail(
								config, 2);
	g_assert_cmpint(g_gpiod_line_settings_get_direction(retrieved), ==,
			G_GPIOD_LINE_DIRECTION_INPUT);
	g_assert_cmpint(g_gpiod_line_settings_get_bias(retrieved), ==,
			G_GPIOD_LINE_BIAS_PULL_DOWN);
}

GPIOD_TEST_CASE(null_settings)
{
	static const guint offset_vals[] = { 0, 1, 2, 3 };

	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GArray) offsets = NULL;

	config = g_gpiod_line_config_new();
	offsets = gpiod_glib_test_array_from_const(offset_vals, 4,
						   sizeof(guint));

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      NULL);

	settings = gpiod_glib_test_line_config_get_line_settings_or_fail(config,
									 2);

	g_assert_cmpint(g_gpiod_line_settings_get_drive(settings), ==,
			G_GPIOD_LINE_DIRECTION_AS_IS);
}

GPIOD_TEST_CASE(null_offsets)
{
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GError) err = NULL;
	gboolean ret;

	settings = g_gpiod_line_settings_new(NULL);
	config = g_gpiod_line_config_new();
	offsets = g_array_new(FALSE, TRUE, sizeof(guint));

	ret = g_gpiod_line_config_add_line_settings(config, NULL, settings,
						    &err);
	g_assert_false(ret);
	g_assert_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL);
}

GPIOD_TEST_CASE(zero_offsets)
{
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GError) err = NULL;
	gboolean ret;

	settings = g_gpiod_line_settings_new(NULL);
	config = g_gpiod_line_config_new();
	offsets = g_array_new(FALSE, TRUE, sizeof(guint));

	ret = g_gpiod_line_config_add_line_settings(config, offsets, settings,
						    &err);
	g_assert_false(ret);
	g_assert_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL);
}

GPIOD_TEST_CASE(set_global_output_values)
{
	static const guint offset_vals[] = { 0, 1, 2, 3 };
	static const GPIODLineValue output_values[] = {
		G_GPIOD_LINE_VALUE_ACTIVE,
		G_GPIOD_LINE_VALUE_INACTIVE,
		G_GPIOD_LINE_VALUE_ACTIVE,
		G_GPIOD_LINE_VALUE_INACTIVE,
	};

	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 4, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GArray) values = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));
	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_OUTPUT, NULL);
	config = g_gpiod_line_config_new();
	offsets = gpiod_glib_test_array_from_const(offset_vals, 4,
						   sizeof(guint));
	values = gpiod_glib_test_array_from_const(output_values, 4,
						  sizeof(GPIODLineValue));

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      settings);
	gpiod_glib_test_line_config_set_output_values_or_fail(config, values);

	request = gpiod_glib_test_chip_request_lines_or_fail(chip, NULL,
							     config);

	g_assert_cmpint(g_gpiosim_chip_get_value(sim, 0), ==,
			G_GPIOSIM_VALUE_ACTIVE);
	g_assert_cmpint(g_gpiosim_chip_get_value(sim, 1), ==,
			G_GPIOSIM_VALUE_INACTIVE);
	g_assert_cmpint(g_gpiosim_chip_get_value(sim, 2), ==,
			G_GPIOSIM_VALUE_ACTIVE);
	g_assert_cmpint(g_gpiosim_chip_get_value(sim, 3), ==,
			G_GPIOSIM_VALUE_INACTIVE);
}

GPIOD_TEST_CASE(handle_duplicate_offsets)
{
	static const guint offset_vals[] = { 0, 2, 2, 3 };

	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GArray) retrieved = NULL;

	config = g_gpiod_line_config_new();
	offsets = gpiod_glib_test_array_from_const(offset_vals, 4,
						   sizeof(guint));

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      NULL);

	retrieved = g_gpiod_line_config_get_configured_offsets(config);
	g_assert_cmpuint(retrieved->len, ==, 3);
	g_assert_cmpuint(g_array_index(retrieved, guint, 0), ==, 0);
	g_assert_cmpuint(g_array_index(retrieved, guint, 1), ==, 2);
	g_assert_cmpuint(g_array_index(retrieved, guint, 2), ==, 3);
}
