// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

/* Minimal example of reading the info for a line. */

#include <glib.h>
#include <gpiod-glib.h>
#include <stdlib.h>

static GString *make_flags(GPIODLineInfo *info)
{
	g_autofree gchar *drive_str = NULL;
	g_autofree gchar *edge_str = NULL;
	g_autofree gchar *bias_str = NULL;
	GPIODLineDrive drive;
	GPIODLineEdge edge;
	GPIODLineBias bias;
	GString *ret;

	edge = g_gpiod_line_info_get_edge_detection(info);
	bias = g_gpiod_line_info_get_bias(info);
	drive = g_gpiod_line_info_get_drive(info);

	edge_str = g_enum_to_string(G_GPIOD_LINE_EDGE_TYPE, edge);
	bias_str = g_enum_to_string(G_GPIOD_LINE_BIAS_TYPE, bias);
	drive_str = g_enum_to_string(G_GPIOD_LINE_DRIVE_TYPE, drive);

	ret = g_string_new(NULL);
	g_string_printf(ret, "%s, %s, %s", edge_str, bias_str, drive_str);
	g_string_replace(ret, "G_GPIOD_LINE_", "", 0);

	return ret;
}

int main(void)
{
	/* Example configuration - customize to suit your situation. */
	static const gchar *const chip_path = "/dev/gpiochip0";
	static const guint line_offset = 4;

	g_autoptr(GPIODLineInfo) info = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GString) flags = NULL;
	const gchar *name, *consumer;
	GPIODLineDirection direction;
	g_autoptr(GError) err = NULL;
	gboolean active_low;

	chip = g_gpiod_chip_new(chip_path, &err);
	if (err) {
		g_printerr("Failed to open the GPIO chip at '%s': %s\n",
			   chip_path, err->message);
		return EXIT_FAILURE;
	}

	info = g_gpiod_chip_get_line_info(chip, line_offset, &err);
	if (err) {
		g_printerr("Failed to retrieve GPIO line info: %s\n",
			   err->message);
		return EXIT_FAILURE;
	}

	name = g_gpiod_line_info_get_name(info);
	consumer = g_gpiod_line_info_get_consumer(info);
	direction = g_gpiod_line_info_get_direction(info);
	active_low = g_gpiod_line_info_is_active_low(info);
	flags = make_flags(info);

	g_print("\tline: %u %s %s %s %s [%s]\n",
		line_offset,
		name ?: "unnamed",
		consumer ?: "unused",
		direction == G_GPIOD_LINE_DIRECTION_INPUT ?
					"input" : "output",
		active_low ? "active-low" : "active-high",
		flags->str);

	return EXIT_SUCCESS;
}
