// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gpiod-glib.h>
#include <gpiod-test.h>
#include <gpiod-test-common.h>
#include <gpiosim-glib.h>

#include "helpers.h"

#define GPIOD_TEST_GROUP "glib/info-event"

GPIOD_TEST_CASE(watching_info_events_returns_line_info)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));
	info = gpiod_glib_test_chip_watch_line_info_or_fail(chip, 3);
	g_assert_cmpuint(g_gpiod_line_info_get_offset(info), ==, 3);
}

GPIOD_TEST_CASE(try_offset_of_out_range)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;
	g_autoptr(GError) err = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));
	info = g_gpiod_chip_watch_line_info(chip, 11, &err);
	g_assert_null(info);
	g_assert_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL);
}

static void on_bad_info_event(GPIODChip *chip G_GNUC_UNUSED,
			      GPIODInfoEvent *event G_GNUC_UNUSED,
			      gpointer data G_GNUC_UNUSED)
{
	g_test_fail_printf("unexpected info event received");
}

static gboolean on_expected_timeout(gpointer data)
{
	gboolean *done = data;

	*done = TRUE;

	return G_SOURCE_REMOVE;
}

GPIOD_TEST_CASE(event_timeout)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;
	gboolean done = FALSE;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));

	g_signal_connect(chip, "info-event",
			 G_CALLBACK(on_bad_info_event), NULL);
	g_timeout_add(100, on_expected_timeout, &done);

	info = gpiod_glib_test_chip_watch_line_info_or_fail(chip, 3);

	while (!done && !g_test_failed())
		g_main_context_iteration(NULL, TRUE);
}

typedef struct {
	const gchar *chip_path;
	guint offset;
} RequestContext;

typedef struct {
	GPtrArray *events;
	guint done;
	gboolean failed;
} EventContext;

static gpointer request_reconfigure_release_line(gpointer data)
{
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GError) err = NULL;
	RequestContext *ctx = data;
	gboolean ret;

	chip = g_gpiod_chip_new(ctx->chip_path, &err);
	g_assert_no_error(err);
	if (g_test_failed())
		return NULL;

	offsets = gpiod_glib_test_array_from_const(&ctx->offset, 1,
						   sizeof(guint));
	config = g_gpiod_line_config_new();
	settings = g_gpiod_line_settings_new(NULL);

	ret = g_gpiod_line_config_add_line_settings(config, offsets,
						    settings, &err);
	g_assert_true(ret);
	g_assert_no_error(err);
	if (g_test_failed())
		return NULL;

	g_usleep(1000);

	request = g_gpiod_chip_request_lines(chip, NULL, config, &err);
	g_assert_nonnull(request);
	g_assert_no_error(err);

	g_usleep(1000);

	g_gpiod_line_config_reset(config);
	g_gpiod_line_settings_set_direction(settings,
					    G_GPIOD_LINE_DIRECTION_OUTPUT);
	ret = g_gpiod_line_config_add_line_settings(config, offsets,
						    settings, &err);
	g_assert_true(ret);
	g_assert_no_error(err);
	if (g_test_failed())
		return NULL;

	ret = g_gpiod_line_request_reconfigure_lines(request, config, &err);
	g_assert_true(ret);
	g_assert_no_error(err);
	if (g_test_failed())
		return NULL;

	g_usleep(1000);

	g_gpiod_line_request_release(request);

	return NULL;
}

static void basic_on_info_event(GPIODChip *chip G_GNUC_UNUSED,
			  GPIODInfoEvent *event, gpointer data)
{
	EventContext *ctx = data;

	g_ptr_array_add(ctx->events, g_object_ref(event));
	ctx->done++;
}

static gboolean on_timeout(gpointer data)
{
	gboolean *failed = data;

	g_test_fail_printf("wait for info event timed out");
	*failed = TRUE;

	return G_SOURCE_CONTINUE;
}

GPIOD_TEST_CASE(request_reconfigure_release_events)
{
	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;
	g_autoptr(GPtrArray) events = NULL;
	g_autoptr(GThread) thread = NULL;
	const gchar *chip_path = g_gpiosim_chip_get_dev_path(sim);
	GPIODInfoEvent *req_ev, *reconf_ev, *rel_ev;
	guint64 req_ts, reconf_ts, rel_ts;
	EventContext ev_ctx;
	RequestContext req_ctx;
	guint timeout_id;

	events = g_ptr_array_new_full(3, g_object_unref);

	chip = gpiod_glib_test_new_chip_or_fail(chip_path);
	g_signal_connect(chip, "info-event", G_CALLBACK(basic_on_info_event),
			 &ev_ctx);
	timeout_id = g_timeout_add_seconds(5, on_timeout, &ev_ctx.failed);

	info = gpiod_glib_test_chip_watch_line_info_or_fail(chip, 3);

	g_assert_false(g_gpiod_line_info_is_used(info));

	req_ctx.chip_path = chip_path;
	req_ctx.offset = 3;

	thread = g_thread_new("request-reconfigure-release",
			      request_reconfigure_release_line, &req_ctx);
	g_thread_ref(thread);

	ev_ctx.done = 0;
	ev_ctx.failed = FALSE;
	ev_ctx.events = events;

	while (ev_ctx.done != 3 && !ev_ctx.failed)
		g_main_context_iteration(NULL, TRUE);

	g_source_remove(timeout_id);
	g_thread_join(thread);

	req_ev = g_ptr_array_index(events, 0);
	reconf_ev = g_ptr_array_index(events, 1);
	rel_ev = g_ptr_array_index(events, 2);

	g_assert_cmpint(g_gpiod_info_event_get_event_type(req_ev), ==,
			G_GPIOD_INFO_EVENT_LINE_REQUESTED);
	g_assert_cmpint(g_gpiod_info_event_get_event_type(reconf_ev), ==,
			G_GPIOD_INFO_EVENT_LINE_CONFIG_CHANGED);
	g_assert_cmpint(g_gpiod_info_event_get_event_type(rel_ev), ==,
			G_GPIOD_INFO_EVENT_LINE_RELEASED);

	req_ts = g_gpiod_info_event_get_timestamp_ns(req_ev);
	reconf_ts = g_gpiod_info_event_get_timestamp_ns(reconf_ev);
	rel_ts = g_gpiod_info_event_get_timestamp_ns(rel_ev);

	g_assert_cmpuint(req_ts, <, reconf_ts);
	g_assert_cmpuint(reconf_ts, <, rel_ts);
}

static void unwatch_on_info_event(GPIODChip *chip G_GNUC_UNUSED,
				  GPIODInfoEvent *event G_GNUC_UNUSED,
				  gpointer data)
{
	gboolean *got_event = data;

	*got_event = TRUE;
}

GPIOD_TEST_CASE(unwatch_and_check_that_no_events_are_generated)
{
	static const guint offset = 3;

	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) info = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;
	gboolean got_event = FALSE;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));
	g_signal_connect(chip, "info-event", G_CALLBACK(unwatch_on_info_event),
			 &got_event);

	offsets = gpiod_glib_test_array_from_const(&offset, 1, sizeof(guint));
	config = g_gpiod_line_config_new();
	settings = g_gpiod_line_settings_new(NULL);

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      settings);

	info = gpiod_glib_test_chip_watch_line_info_or_fail(chip, offset);

	request = gpiod_glib_test_chip_request_lines_or_fail(chip, NULL,
							     config);

	g_main_context_iteration(NULL, TRUE);

	g_assert_true(got_event);

	gpiod_glib_test_chip_unwatch_line_info_or_fail(chip, offset);

	got_event = FALSE;
	g_gpiod_line_request_release(request);

	g_main_context_iteration(NULL, TRUE);

	g_assert_false(got_event);
}

static void check_line_info_on_info_event(GPIODChip *chip G_GNUC_UNUSED,
					  GPIODInfoEvent *event, gpointer data)
{
	GPIODLineInfo **info = data;

	*info = g_gpiod_info_event_get_line_info(event);
}

GPIOD_TEST_CASE(info_event_contains_new_line_info)
{
	static const guint offset = 3;

	g_autoptr(GPIOSimChip) sim = g_gpiosim_chip_new("num-lines", 8, NULL);
	g_autoptr(GPIODChip) chip = NULL;
	g_autoptr(GPIODLineInfo) initial_info = NULL;
	g_autoptr(GPIODLineInfo) event_info = NULL;
	g_autoptr(GPIODLineRequest) request = NULL;
	g_autoptr(GPIODLineSettings) settings = NULL;
	g_autoptr(GPIODLineConfig) config = NULL;
	g_autoptr(GArray) offsets = NULL;

	chip = gpiod_glib_test_new_chip_or_fail(
			g_gpiosim_chip_get_dev_path(sim));
	g_signal_connect(chip, "info-event",
			 G_CALLBACK(check_line_info_on_info_event),
			 &event_info);

	offsets = gpiod_glib_test_array_from_const(&offset, 1, sizeof(guint));
	config = g_gpiod_line_config_new();
	settings = g_gpiod_line_settings_new(NULL);

	gpiod_glib_test_line_config_add_line_settings_or_fail(config, offsets,
							      settings);

	initial_info = gpiod_glib_test_chip_watch_line_info_or_fail(chip,
								    offset);

	request = gpiod_glib_test_chip_request_lines_or_fail(chip, NULL,
							     config);

	g_main_context_iteration(NULL, TRUE);

	g_assert_nonnull(event_info);
}
