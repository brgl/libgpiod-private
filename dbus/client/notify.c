// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <glib-unix.h>
#include <stdlib.h>

#include "common.h"

/*
 * Used to keep line proxies and chip interfaces alive for the duration of the
 * program, which is required for signals to work.
 */
typedef struct {
	GList *lines;
	GList *chips;
	GPIODBusObject *scoped_chip;
} NotifyData;

static void clear_notify_data(NotifyData *data)
{
	g_list_free_full(data->lines, g_object_unref);
	g_list_free_full(data->chips, g_object_unref);
	g_clear_object(&data->scoped_chip);
}

G_DEFINE_AUTO_CLEANUP_CLEAR_FUNC(NotifyData, clear_notify_data);

static const gchar *bool_to_str(gboolean val)
{
	return val ? "True" : "False";
}

const gchar *bool_variant_to_str(GVariant *val)
{
	return bool_to_str(g_variant_get_boolean(val));
}

static void
on_properties_changed(GPIODBusLine *line, GVariant *changed_properties,
		      GStrv invalidated_properties G_GNUC_UNUSED,
		      gpointer user_data)
{
	GPIODBusChip *chip = user_data;
	const gchar *consumer;
	GVariantIter iter;
	GVariant *v;
	gsize len;
	gchar *k;

	if (g_variant_n_children(changed_properties) == 0)
		return;

	g_print("%s %u\n",
		gpiodbus_chip_get_name(chip),
		gpiodbus_line_get_offset(line));

	g_variant_iter_init(&iter, changed_properties);
	while (g_variant_iter_next(&iter, "{sv}", &k, &v)) {
		g_autofree gchar *req_name = NULL;
		g_autoptr(GVariant) val = v;
		g_autofree gchar *key = k;

		if (g_strcmp0(key, "Consumer") == 0) {
			g_print("  Consumer => ");
			consumer = g_variant_get_string(val, &len);
			if (!len)
				g_print("unused");
			else
				g_print("\"%s\"", consumer);
		} else if ((g_strcmp0(key, "Used") == 0) ||
			   (g_strcmp0(key, "Debounced") == 0)) {
			g_print("  %s => %s", key, bool_variant_to_str(val));
		} else if (g_strcmp0(key, "ActiveLow") == 0) {
			g_print("  Active-low => %s",
				bool_variant_to_str(val));
		} else if ((g_strcmp0(key, "Direction") == 0) ||
			   (g_strcmp0(key, "Drive") == 0) ||
			   (g_strcmp0(key, "Bias") == 0)) {
			g_print("  %s => %s", key,
				g_variant_get_string(val, NULL));
		} else if (g_strcmp0(key, "EdgeDetection") == 0) {
			g_print("  Edge => %s",
				g_variant_get_string(val, NULL));
		} else if (g_strcmp0(key, "EventClock") == 0) {
			g_print("  Clock => %s",
				g_variant_get_string(val, NULL));
		} else if (g_strcmp0(key, "DebouncePeriodUs") == 0) {
			g_print("  Debounce Period => %lu",
				g_variant_get_uint64(val));
		} else if (g_strcmp0(key, "Managed") == 0) {
			g_print("  Managed: %s",
				bool_variant_to_str(val));
		} else if (g_strcmp0(key, "RequestObject") == 0) {
			req_name = sanitize_object_path(
					g_variant_get_string(val, NULL));
			g_print("  Request: %s", req_name);
		} else {
			die("unexpected property update received from manager: '%s'",
			    key);
		}

		g_print("\n");
	}
}

static void print_line_info(GPIODBusLine *line, GPIODBusChip *chip)
{
	g_autoptr(LineProperties) props = NULL;

	props = get_line_properties(line);

	g_print("%s %u:\n", gpiodbus_chip_get_name(chip), props->offset);
	if (props->name)
		g_print("  Name: \"%s\"\n", props->name);
	else
		g_print("  Name: unnamed\n");
	g_print("  Used: %s\n", bool_to_str(props->used));
	if (props->consumer)
		g_print("  Consumer: \"%s\"\n", props->consumer);
	else
		g_print("  Consumer: unused\n");
	g_print("  Direction: %s\n", props->direction);
	g_print("  Drive: %s\n", props->drive);
	g_print("  Bias: %s\n", props->bias ?: "unknown");
	g_print("  Active-low: %s\n", bool_to_str(props->active_low));
	g_print("  Edge: %s\n", props->edge ?: "none");
	g_print("  Debounced: %s\n", bool_to_str(props->debounced));
	g_print("  Debounce Period: %lu\n", props->debounce_period);
	g_print("  Clock: %s\n", props->event_clock);
	g_print("  Managed: %s\n", bool_to_str(props->managed));
	g_print("  Request: %s\n", props->request_name);
}

static void connect_line(gpointer elem, gpointer user_data)
{
	g_autoptr(GPIODBusObject) line_obj = NULL;
	g_autoptr(GPIODBusObject) chip_obj = NULL;
	g_autoptr(GPIODBusLine) line = NULL;
	g_autoptr(GPIODBusChip) chip = NULL;
	g_autofree gchar *chip_name = NULL;
	g_autoptr(GError) err = NULL;
	NotifyData *data = user_data;
	const gchar *line_obj_path;
	GString *line_name = elem;
	gboolean ret;

	if (data->scoped_chip) {
		chip_obj = g_object_ref(data->scoped_chip);
		line_obj = get_line_obj_by_name_for_chip(chip_obj,
							 line_name->str);
		if (!line_obj) {
			chip_name = g_path_get_basename(
				g_dbus_object_get_object_path(
					G_DBUS_OBJECT(chip_obj)));
			die("no line '%s' on chip '%s'",
			    line_name->str, chip_name);
		}
	} else {
		ret = get_line_obj_by_name(line_name->str,
					   &line_obj, &chip_obj);
		if (!ret)
			die("line '%s' not found", line_name->str);
	}

	line_obj_path = g_dbus_object_get_object_path(G_DBUS_OBJECT(line_obj));

	line = gpiodbus_line_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
						    G_DBUS_PROXY_FLAGS_NONE,
						    "io.gpiod1", line_obj_path,
						    NULL, &err);
	if (err)
		die_gerror(err, "Failed to get DBus proxy for '%s'",
			   line_obj_path);

	data->lines = g_list_append(data->lines, g_object_ref(line));

	if (data->scoped_chip) {
		if (g_list_length(data->chips) == 0) {
			chip = gpiodbus_object_get_chip(chip_obj);
			data->chips = g_list_append(data->chips,
						    g_object_ref(chip));
		} else {
			chip = g_list_first(data->chips)->data;
		}
	} else {
		chip = gpiodbus_object_get_chip(chip_obj);
		data->chips = g_list_append(data->chips, g_object_ref(chip));
	}

	print_line_info(line, chip);

	g_signal_connect(line, "g-properties-changed",
			 G_CALLBACK(on_properties_changed), chip);
}

int gpiocli_notify_main(int argc, char **argv)
{
	static const gchar *const summary =
"Monitor a set of lines for property changes.";

	static const gchar *const description =
"Lines are specified by name, or optionally by offset if the chip option\n"
"is provided.\n";

	g_autolist(GString) line_name_list = NULL;
	g_autoptr(GMainLoop) loop = NULL;
	g_auto(GStrv) line_names = NULL;
	const gchar *chip_name = NULL;
	NotifyData data = {};
	guint watch_id;

	const GOptionEntry opts[] = {
		{
			.long_name		= "chip",
			.short_name		= 'c',
			.flags			= G_OPTION_FLAG_NONE,
			.arg			= G_OPTION_ARG_STRING,
			.arg_data		= &chip_name,
			.description		= "restrict scope to a particular chip",
			.arg_description	= "<chip>",
		},
		{
			.long_name		= G_OPTION_REMAINING,
			.flags			= G_OPTION_FLAG_NONE,
			.arg			= G_OPTION_ARG_STRING_ARRAY,
			.arg_data		= &line_names,
			.arg_description	= "<line1> [line2] ...",
		},
		{ }
	};

	parse_options(opts, summary, description, &argc, &argv);

	watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM, "io.gpiod1",
				    G_BUS_NAME_WATCHER_FLAGS_NONE,
				    NULL, die_on_name_vanished, NULL, NULL);
	check_manager();

	if (!line_names)
		die_parsing_opts("at least one line must be specified");

	if (chip_name)
		data.scoped_chip = get_chip_obj(chip_name);

	line_name_list = strv_to_gstring_list(line_names);
	g_list_foreach(line_name_list, connect_line, &data);

	loop = g_main_loop_new(NULL, FALSE);
	g_unix_signal_add(SIGTERM, quit_main_loop_on_signal, loop);
	g_unix_signal_add(SIGINT, quit_main_loop_on_signal, loop);

	g_main_loop_run(loop);

	g_bus_unwatch_name(watch_id);

	return EXIT_SUCCESS;
}
