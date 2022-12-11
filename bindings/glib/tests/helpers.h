/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

#ifndef __GPIOD_GLIB_TEST_HELPERS_H__
#define __GPIOD_GLIB_TEST_HELPERS_H__

#include <glib.h>
#include <gpiod-test-common.h>

#define gpiod_glib_test_new_chip_or_fail(_path) \
	({ \
		g_autoptr(GError) _err = NULL; \
		GPIODChip *_chip = g_gpiod_chip_new(_path, &_err); \
		g_assert_nonnull(_chip); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
		_chip; \
	})

#define gpiod_glib_test_chip_get_info_or_fail(_chip) \
	({ \
		g_autoptr(GError) _err = NULL; \
		GPIODChipInfo *_info = g_gpiod_chip_get_info(_chip, &_err); \
		g_assert_nonnull(_info); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
		_info; \
	})

#define gpiod_glib_test_chip_get_line_info_or_fail(_chip, _offset) \
	({ \
		g_autoptr(GError) _err = NULL; \
		GPIODLineInfo *_info = g_gpiod_chip_get_line_info(_chip, \
								  _offset, \
								  &_err); \
		g_assert_nonnull(_info); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
		_info; \
	})

#define gpiod_glib_test_chip_watch_line_info_or_fail(_chip, _offset) \
	({ \
		g_autoptr(GError) _err = NULL; \
		GPIODLineInfo *_info = g_gpiod_chip_watch_line_info(_chip, \
								    _offset, \
								    &_err); \
		g_assert_nonnull(_info); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
		_info; \
	})

#define gpiod_glib_test_chip_unwatch_line_info_or_fail(_chip, _offset) \
	do { \
		g_autoptr(GError) _err = NULL; \
		gboolean ret = \
			g_gpiod_chip_unwatch_line_info(_chip, _offset, &_err); \
		g_assert_true(ret); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
	} while (0)

#define gpiod_glib_test_line_config_add_line_settings_or_fail(_config, \
							      _offsets, \
							      _settings) \
	do { \
		g_autoptr(GError) _err = NULL; \
		gboolean _ret = \
			g_gpiod_line_config_add_line_settings(_config, \
							      _offsets,\
							      _settings, \
							      &_err); \
		g_assert_true(_ret); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
	} while (0)

#define gpiod_glib_test_line_config_get_line_settings_or_fail(_config, \
							      _offset) \
	({ \
		GPIODLineSettings *_settings = \
			g_gpiod_line_config_get_line_settings(_config, \
							      _offset); \
		g_assert_nonnull(_settings); \
		gpiod_test_return_if_failed(); \
		_settings; \
	})

#define gpiod_glib_test_line_config_set_output_values_or_fail(_config, \
							      _values) \
	do { \
		g_autoptr(GError) _err = NULL; \
		gboolean _ret = \
			g_gpiod_line_config_set_output_values(_config, \
							      _values, \
							      &_err); \
		g_assert_true(_ret); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
	} while (0)

#define gpiod_glib_test_chip_request_lines_or_fail(_chip, _req_cfg, _line_cfg) \
	({ \
		g_autoptr(GError) _err = NULL; \
		GPIODLineRequest *_req = \
			g_gpiod_chip_request_lines(_chip, _req_cfg, \
						   _line_cfg, &_err); \
		g_assert_nonnull(_req); \
		g_assert_no_error(_err); \
		gpiod_test_return_if_failed(); \
		_req; \
	})

#define gpiod_glib_test_request_lines_or_fail(_path, _req_cfg, _line_cfg) \
	({ \
		g_autoptr(GPIODChip) _chip = \
			gpiod_glib_test_new_chip_or_fail(_path); \
		GPIODLineRequest *_req = \
			gpiod_glib_test_chip_request_lines_or_fail(_chip, \
								   _req_cfg, \
								   _line_cfg); \
		_req; \
	})

#define gpiod_glib_test_check_error_or_fail(_err, _domain, _code) \
	do { \
		g_assert_nonnull(_err); \
		gpiod_test_return_if_failed(); \
		g_assert_cmpint(_domain, ==, (_err)->domain); \
		g_assert_cmpint(_code, ==, (_err)->code); \
		gpiod_test_return_if_failed(); \
	} while (0)

GArray *gpiod_glib_test_array_from_const(const gconstpointer data, gsize len,
					 gsize elem_size);

#endif /* __GPIOD_GLIB_TEST_HELPERS_H__ */

