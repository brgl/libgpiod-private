// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

/* Minimal example of asynchronously watching for edges on a single line. */

#include <glib.h>
#include <gpiod-glib.h>
#include <stdlib.h>

static void on_edge_event(GPIODLineRequest *request G_GNUC_UNUSED,
			  GPIODEdgeEvent *event,
			  gpointer data G_GNUC_UNUSED)
{
	g_autoptr(GString) event_name = NULL;
	guint64 timestamp;
	guint offset;

	event_name = g_string_new(
			g_enum_to_string(G_GPIOD_EDGE_EVENT_TYPE_TYPE,
				g_gpiod_edge_event_get_event_type(event)));
	g_string_replace(event_name, "G_GPIOD_EDGE_EVENT_", "", 0);
	timestamp = g_gpiod_edge_event_get_timestamp_ns(event);
	offset = g_gpiod_edge_event_get_line_offset(event);

	g_print("%s %lu %u\n", event_name->str, timestamp, offset);
}

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
	g_autoptr(GMainLoop) loop = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GError) err = NULL;
	gboolean ret;

	offsets = g_array_new(FALSE, TRUE, sizeof(guint));
	g_array_append_val(offsets, line_offset);

	chip = g_gpiod_chip_new(chip_path, &err);
	if (!chip) {
		g_printerr("unable to open %s: %s\n", chip_path, err->message);
		return EXIT_FAILURE;
	}

	/*
	 * Assume a button connecting the pin to ground, so pull it up and
	 * provide some debounce.
	 */
	settings = g_gpiod_line_settings_new(
			"direction", G_GPIOD_LINE_DIRECTION_INPUT,
			"edge-detection", G_GPIOD_LINE_EDGE_BOTH,
			"bias", G_GPIOD_LINE_BIAS_PULL_UP,
			"debounce-period-us", 1000,
			NULL);

	line_cfg = g_gpiod_line_config_new();
	ret = g_gpiod_line_config_add_line_settings(line_cfg, offsets,
						    settings, &err);
	if (!ret) {
		g_printerr("failed to add line settings to line config: %s",
			   err->message);
		return EXIT_FAILURE;
	}

	req_cfg = g_gpiod_request_config_new("consumer", "watch-line-value",
					     NULL);

	request = g_gpiod_chip_request_lines(chip, req_cfg, line_cfg, &err);
	if (!request) {
		g_printerr("failed to request lines: %s", err->message);
		return EXIT_FAILURE;
	}

	loop = g_main_loop_new(NULL, FALSE);

	/* Connect to the edge-event signal on the line-request. */
	g_signal_connect(request, "edge-event",
			 G_CALLBACK(on_edge_event), NULL);

	g_main_loop_run(loop);

	return EXIT_SUCCESS;
}
