// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

/* Minimal example of reading a single line. */

#include <glib.h>
#include <gpiod-glib.h>
#include <stdlib.h>

int main(void)
{
	/* Example configuration - customize to suit your situation. */
	static const gchar *const chip_path = "/dev/gpiochip1";
	static const guint line_offset = 5;

	g_autoptr(GPIODRequestConfig) req_cfg = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineConfig) line_cfg = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GError) err = NULL;
	guint offset;
	gboolean ret;

	chip = g_gpiod_chip_new(chip_path, &err);
	if (!chip) {
		g_printerr("unable to open %s: %s\n", chip_path, err->message);
		return EXIT_FAILURE;
	}

	offsets = g_array_new(FALSE, TRUE, sizeof(guint));
	g_array_append_val(offsets, line_offset);

	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_INPUT,
			NULL);

	line_cfg = g_gpiod_line_config_new();
	ret = g_gpiod_line_config_add_line_settings(line_cfg, offsets,
						    settings, &err);
	if (!ret) {
		g_printerr("failed to add line settings to line config: %s\n",
			   err->message);
		return EXIT_FAILURE;
	}

	req_cfg = g_gpiod_request_config_new("consumer", "get-line-value-glib",
					     NULL);

	request = g_gpiod_chip_request_lines(chip, req_cfg, line_cfg, &err);
	if (!request) {
		g_printerr("failed to request lines: %s\n", err->message);
		return EXIT_FAILURE;
	}

	ret = g_gpiod_line_request_get_value(request, line_offset,
					     &offset, &err);
	if (!ret) {
		g_printerr("failed to read line values: %s\n", err->message);
		return EXIT_FAILURE;
	}

	g_print("%u\n", offset);

	return EXIT_SUCCESS;
}
