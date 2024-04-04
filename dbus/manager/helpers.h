/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

#ifndef __GPIODBUS_HELPERS_H__
#define __GPIODBUS_HELPERS_H__

#include <gio/gio.h>
#include <gpiod-glib.h>
#include <glib.h>
#include <gpiodbus.h>

gint gpiodbus_id_cmp(gconstpointer a, gconstpointer b, gpointer user_data);
gint gpiodbus_id_alloc(GTree *id_root);
void gpiodbus_id_free(GTree *id_root, gint id);
gboolean
gpiodbus_chip_set_props(GPIODBusChip *skeleton, GPIODChip *chip, GError **err);
void gpiodbus_line_set_props(GPIODBusLine *skeleton, GPIODLineInfo *info);
void gpiodbus_request_set_props(GPIODBusRequest *skeleton,
				GPIODLineRequest *request, GPIODBusChip *chip,
				GDBusObjectManager *line_manager);
GPIODLineConfig *gpiodbus_line_config_from_variant(GVariant *variant);
GPIODRequestConfig *gpiodbus_request_config_from_variant(GVariant *variant);

#endif /* __GPIODBUS_HELPERS_H__ */
