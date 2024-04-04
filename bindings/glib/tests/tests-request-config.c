// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <glib.h>
#include <gpiod-glib.h>
#include <gpiod-test.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/request-config"

GPIOD_TEST_CASE(default_config)
{
	g_autoptr(GPIODRequestConfig) config = NULL;

	config = g_gpiod_request_config_new(NULL);

	g_assert_null(g_gpiod_request_config_get_consumer(config));
	g_assert_cmpuint(g_gpiod_request_config_get_event_buffer_size(config),
			 ==, 0);
}

GPIOD_TEST_CASE(set_consumer)
{
	g_autoptr(GPIODRequestConfig) config = NULL;

	config = g_gpiod_request_config_new(NULL);

	g_gpiod_request_config_set_consumer(config, "foobar");
	g_assert_cmpstr(g_gpiod_request_config_get_consumer(config), ==,
			"foobar");

	g_gpiod_request_config_set_consumer(config, NULL);
	g_assert_null(g_gpiod_request_config_get_consumer(config));
}

GPIOD_TEST_CASE(set_event_buffer_size)
{
	g_autoptr(GPIODRequestConfig) config = NULL;

	config = g_gpiod_request_config_new(NULL);

	g_gpiod_request_config_set_event_buffer_size(config, 128);
	g_assert_cmpuint(g_gpiod_request_config_get_event_buffer_size(config),
			 ==, 128);
}

GPIOD_TEST_CASE(set_properties_in_constructor)
{
	g_autoptr(GPIODRequestConfig) config = NULL;

	config = g_gpiod_request_config_new("consumer", "foobar",
					    "event-buffer-size", 64, NULL);
	g_assert_cmpstr(g_gpiod_request_config_get_consumer(config), ==,
			"foobar");
	g_assert_cmpuint(g_gpiod_request_config_get_event_buffer_size(config),
			 ==, 64);
}
