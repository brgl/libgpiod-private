// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <errno.h>
#include <glib.h>
#include <gpiod-glib.h>
#include <stdarg.h>

G_DEFINE_QUARK(g-gpiod-error, g_gpiod_error)

static GPIODError error_from_errno(void)
{
	switch (errno) {
	case EPERM:
		return G_GPIOD_ERR_PERM;
	case ENOENT:
		return G_GPIOD_ERR_NOENT;
	case EINTR:
		return G_GPIOD_ERR_INTR;
	case EIO:
		return G_GPIOD_ERR_IO;
	case ENXIO:
		return G_GPIOD_ERR_NXIO;
	case E2BIG:
		return G_GPIOD_ERR_E2BIG;
	case EBADFD:
		return G_GPIOD_ERR_BADFD;
	case ECHILD:
		return G_GPIOD_ERR_CHILD;
	case EAGAIN:
		return G_GPIOD_ERR_AGAIN;
	case ENOMEM:
		/* Special case - as a convention GLib just aborts on ENOMEM. */
		g_error("out of memory");
	case EACCES:
		return G_GPIOD_ERR_ACCES;
	case EFAULT:
		return G_GPIOD_ERR_FAULT;
	case EBUSY:
		return G_GPIOD_ERR_BUSY;
	case EEXIST:
		return G_GPIOD_ERR_EXIST;
	case ENODEV:
		return G_GPIOD_ERR_NODEV;
	case EINVAL:
		return G_GPIOD_ERR_INVAL;
	case ENOTTY:
		return G_GPIOD_ERR_NOTTY;
	case EPIPE:
		return G_GPIOD_ERR_PIPE;
	default:
		return G_GPIOD_ERR_FAILED;
	}
}

void g_gpiod_set_error_from_errno(GError **err, const gchar *fmt, ...)
{
	g_autofree gchar *msg = NULL;
	va_list va;

	va_start(va, fmt);
	msg = g_strdup_vprintf(fmt, va);
	va_end(va);

	g_set_error(err, G_GPIOD_ERROR, error_from_errno(),
		    "%s: %s", msg, g_strerror(errno));
}
