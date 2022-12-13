/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

#ifndef __GPIODBUS_DAEMON_H__
#define __GPIODBUS_DAEMON_H__

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

G_DECLARE_FINAL_TYPE(GPIODBusDaemon, gpiodbus_daemon,
		     GPIODBUS, DAEMON, GObject);

#define GPIODBUS_DAEMON_TYPE (gpiodbus_daemon_get_type())
#define GPIODBUS_DAEMON(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), \
	 GPIODBUS_DAEMON_TYPE, GPIODBusDaemon))

GPIODBusDaemon *gpiodbus_daemon_new(void);
void gpiodbus_daemon_start(GPIODBusDaemon *daemon, GDBusConnection *con);

#endif /* __GPIODBUS_DAEMON_H__ */
