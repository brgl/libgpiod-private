// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

/* Minimal example of periodically toggling multiple lines. */

#include <glib.h>
#include <gpiod-glib.h>
#include <stdlib.h>

typedef struct {
	GPIODLineRequest *request;
	GArray *offsets;
	GArray *values;
} ToggleData;

static void toggle_values(GArray *values)
{
	GPIODLineValue *value;
	guint i;

	for (i = 0; i < values->len; i++) {
		value = &g_array_index(values, GPIODLineValue, i);
		*value = *value == G_GPIOD_LINE_VALUE_ACTIVE ?
					G_GPIOD_LINE_VALUE_INACTIVE :
					G_GPIOD_LINE_VALUE_ACTIVE;
	}
}

static gboolean toggle_lines(gpointer user_data)
{
	ToggleData *data = user_data;
	g_autoptr(GError) err = NULL;
	gboolean ret;
	guint i;

	toggle_values(data->values);

	ret = g_gpiod_line_request_set_values_subset(data->request,
						     data->offsets,
						     data->values, &err);
	if (!ret) {
		g_printerr("failed to set line values: %s\n", err->message);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < data->offsets->len; i++)
		g_print("%u=%s ",
			g_array_index(data->offsets, guint, i),
			g_array_index(data->values,
				      GPIODLineValue,
				      i) == G_GPIOD_LINE_VALUE_ACTIVE ?
						"active" : "inactive");
	g_print("\n");

	return G_SOURCE_CONTINUE;
}

int main(void)
{
	/* Example configuration - customize to suit your situation. */
	static const gchar *const chip_path = "/dev/gpiochip1";
	static const guint line_offsets[] = { 5, 3, 7 };
	static const GPIODLineValue line_values[] = {
		G_GPIOD_LINE_VALUE_ACTIVE,
		G_GPIOD_LINE_VALUE_ACTIVE,
		G_GPIOD_LINE_VALUE_INACTIVE
	};
	static const gsize num_lines = 3;

	g_autoptr(GPIODRequestConfig) req_cfg = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineConfig) line_cfg = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GMainLoop) loop = NULL;
	g_autoptr(GArray) values = NULL;
	g_autoptr(GError) err = NULL;
	ToggleData data;
	gboolean ret;

	offsets = g_array_new(FALSE, TRUE, sizeof(guint));
	g_array_append_vals(offsets, line_offsets, num_lines);

	values = g_array_new(FALSE, TRUE, sizeof(GPIODLineValue));
	g_array_append_vals(values, line_values, num_lines);

	chip = g_gpiod_chip_new(chip_path, &err);
	if (!chip) {
		g_printerr("unable to open %s: %s\n", chip_path, err->message);
		return EXIT_FAILURE;
	}

	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_OUTPUT,
			NULL);
	line_cfg = g_gpiod_line_config_new();
	ret = g_gpiod_line_config_add_line_settings(line_cfg, offsets,
						    settings, &err);
	if (!ret) {
		g_printerr("failed to add line settings to line config: %s\n",
			   err->message);
		return EXIT_FAILURE;
	}

	ret = g_gpiod_line_config_set_output_values(line_cfg, values, &err);
	if (!ret) {
		g_printerr("failed to set output values: %s\n", err->message);
		return EXIT_FAILURE;
	}

	req_cfg = g_gpiod_request_config_new("consumer", "toggle-line-value",
					     NULL);

	request = g_gpiod_chip_request_lines(chip, req_cfg, line_cfg, &err);
	if (!request) {
		g_printerr("failed to request lines: %s\n", err->message);
		return EXIT_FAILURE;
	}

	data.request = request;
	data.offsets = offsets;
	data.values = values;

	loop = g_main_loop_new(NULL, FALSE);
	/* Do the GLib way: add a callback to be invoked from the main loop. */
	g_timeout_add_seconds(1, toggle_lines, &data);

	g_main_loop_run(loop);

	return EXIT_SUCCESS;
}
