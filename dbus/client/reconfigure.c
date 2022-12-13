// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <stdlib.h>

#include "common.h"

int gpiocli_reconfigure_main(int argc, char **argv)
{
	static const gchar *const summary =
"Change the line configuration for an existing request.";

	g_autoptr(GPIODBusObject) req_obj = NULL;
	g_autoptr(GVariant) line_config = NULL;
	LineConfigOpts line_cfg_opts = { };
	g_autoptr(GArray) offsets = NULL;
	g_auto(GStrv) remaining = NULL;
	g_autoptr(GError) err = NULL;
	GPIODBusRequest *request;
	gboolean ret;

	const GOptionEntry opts[] = {
		LINE_CONFIG_OPTIONS(&line_cfg_opts),
		{
			.long_name		= G_OPTION_REMAINING,
			.flags			= G_OPTION_FLAG_NONE,
			.arg			= G_OPTION_ARG_STRING_ARRAY,
			.arg_data		= &remaining,
			.arg_description	= "<request> <line1>[=value1] [line2[=value2]] ...",
		},
		{ }
	};

	parse_options(opts, summary, NULL, &argc, &argv);
	validate_line_config_opts(&line_cfg_opts);

	if (!remaining || g_strv_length(remaining) != 1)
		die_parsing_opts("Exactly one request to reconfigure must be specified.");

	check_manager();

	req_obj = get_request_obj(remaining[0]);
	request = gpiodbus_object_peek_request(req_obj);
	offsets = get_request_offsets(request);
	line_config = make_line_config(offsets, &line_cfg_opts);

	ret = gpiodbus_request_call_reconfigure_lines_sync(request, line_config,
							   NULL, &err);
	if (!ret)
		die_gerror(err, "Failed to reconfigure lines");

	return EXIT_SUCCESS;
}
