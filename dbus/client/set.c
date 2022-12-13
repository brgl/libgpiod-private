// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <stdlib.h>

#include "common.h"

int gpiocli_set_main(int argc, char **argv)
{
	static const gchar *const summary =
"Set values of one or more GPIO lines.";

	static const gchar *const description =
"If -r/--request is specified then all the lines must belong to the same\n"
"request (and - by extension - the same chip).";

	const gchar *request_name = NULL, *chip_path;
	g_autoptr(GPIODBusObject) chip_obj = NULL;
	g_autoptr(GPIODBusObject) req_obj = NULL;
	g_autolist(GString) line_names = NULL;
	g_autoptr(GVariant) arg_values = NULL;
	g_autoptr(GArray) values = NULL;
	g_autoptr(GError) err = NULL;
	g_auto(GStrv) lines = NULL;
	GPIODBusRequest *request;
	GVariantBuilder builder;
	GPIODBusLine *line;
	gsize num_lines, i;
	gboolean ret;
	guint offset;
	gint val;

	const GOptionEntry opts[] = {
		{
			.long_name		= "request",
			.short_name		= 'r',
			.flags			= G_OPTION_FLAG_NONE,
			.arg			= G_OPTION_ARG_STRING,
			.arg_data		= &request_name,
			.description		= "restrict scope to a particular request",
			.arg_description	= "<request>",
		},
		{
			.long_name		= G_OPTION_REMAINING,
			.flags			= G_OPTION_FLAG_NONE,
			.arg			= G_OPTION_ARG_STRING_ARRAY,
			.arg_data		= &lines,
			.arg_description	= "<line1=value1> [line2=value2] ...",
		},
		{ }
	};

	parse_options(opts, summary, description, &argc, &argv);

	if (!lines)
		die_parsing_opts("at least one line value must be specified");

	num_lines = g_strv_length(lines);
	values = g_array_sized_new(FALSE, TRUE, sizeof(gint), num_lines);

	for (i = 0; i < num_lines; i++) {
		g_auto(GStrv) tokens = NULL;

		tokens = g_strsplit(lines[i], "=", 2);
		if (g_strv_length(tokens) != 2)
			die_parsing_opts("line must have a single value assigned");

		line_names = g_list_append(line_names, g_string_new(tokens[0]));
		val = output_value_from_str(tokens[1]);
		g_array_append_val(values, val);
	}

	check_manager();

	if (!request_name) {

	} else {
		g_autoptr(GArray) offsets = NULL;

		req_obj = get_request_obj(request_name);
		request = gpiodbus_object_peek_request(req_obj);
		chip_path = gpiodbus_request_get_chip_path(request);
		chip_obj = get_chip_obj_by_path(chip_path);
		offsets = g_array_sized_new(FALSE, TRUE, sizeof(guint),
					    num_lines);

		for (i = 0; i < num_lines; i++) {
			g_autoptr(GPIODBusObject) line_obj = NULL;

			line_obj = get_line_obj_by_name_for_chip(chip_obj,
								 lines[i]);
			if (!line_obj)
				die("Line not found: %s\n", lines[i]);

			line = gpiodbus_object_peek_line(line_obj);
			offset = gpiodbus_line_get_offset(line);
			g_array_append_val(offsets, offset);
		}

		g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);
		for (i = 0; i < num_lines; i++) {
			g_variant_builder_add(&builder, "(ui)",
					      g_array_index(offsets, guint, i),
					      g_array_index(values, gint, i));
		}

		arg_values = g_variant_ref_sink(
				g_variant_builder_end(&builder));

		ret = gpiodbus_request_call_set_values_sync(request, arg_values,
							    NULL, &err);
		if (!ret)
			die_gerror(err, "Failed to set line values");
	}

	return EXIT_SUCCESS;
}
