// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>
#include <stdarg.h>

#include "internal.h"

struct _GPIODRequestConfig {
	GObject parent_instance;
	struct gpiod_request_config *handle;
};

enum {
	G_GPIOD_REQUEST_CONFIG_PROP_HANDLE = 1,
	G_GPIOD_REQUEST_CONFIG_PROP_CONSUMER,
	G_GPIOD_REQUEST_CONFIG_PROP_EVENT_BUFFER_SIZE,
};

G_DEFINE_TYPE(GPIODRequestConfig, g_gpiod_request_config, G_TYPE_OBJECT);

static void g_gpiod_request_config_get_property(GObject *obj, guint prop_id,
						GValue *val, GParamSpec *pspec)
{
	GPIODRequestConfig *self = G_GPIOD_REQUEST_CONFIG_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_REQUEST_CONFIG_PROP_HANDLE:
		g_value_set_pointer(val, self->handle);
		break;
	case G_GPIOD_REQUEST_CONFIG_PROP_CONSUMER:
		g_value_set_static_string(val,
			gpiod_request_config_get_consumer(self->handle));
		break;
	case G_GPIOD_REQUEST_CONFIG_PROP_EVENT_BUFFER_SIZE:
		g_value_set_uint(val,
			gpiod_request_config_get_event_buffer_size(
				self->handle));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_request_config_set_property(GObject *obj, guint prop_id,
						const GValue *val,
						GParamSpec *pspec)
{
	GPIODRequestConfig *self = G_GPIOD_REQUEST_CONFIG_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_REQUEST_CONFIG_PROP_CONSUMER:
		gpiod_request_config_set_consumer(self->handle,
						  g_value_get_string(val));
		break;
	case G_GPIOD_REQUEST_CONFIG_PROP_EVENT_BUFFER_SIZE:
		gpiod_request_config_set_event_buffer_size(self->handle,
							g_value_get_uint(val));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_request_config_finalize(GObject *obj)
{
	GPIODRequestConfig *self = G_GPIOD_REQUEST_CONFIG_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_request_config_free);

	G_OBJECT_CLASS(g_gpiod_request_config_parent_class)->finalize(obj);
}

static void
g_gpiod_request_config_class_init(GPIODRequestConfigClass *request_config_class)
{
	GObjectClass *class = G_OBJECT_CLASS(request_config_class);

	class->set_property = g_gpiod_request_config_set_property;
	class->get_property = g_gpiod_request_config_get_property;
	class->finalize = g_gpiod_request_config_finalize;

	g_object_class_install_property(class,
					G_GPIOD_REQUEST_CONFIG_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO request config object.",
			G_PARAM_READABLE));

	g_object_class_install_property(class,
					G_GPIOD_REQUEST_CONFIG_PROP_CONSUMER,
		g_param_spec_string("consumer", "Consumer",
			"Name of the request consumer.",
			NULL, G_PARAM_READWRITE));

	g_object_class_install_property(class,
				G_GPIOD_REQUEST_CONFIG_PROP_EVENT_BUFFER_SIZE,
		g_param_spec_uint("event-buffer-size", "Event Buffer Size",
			"Size of the kernel event buffer size of the request.",
			0, G_MAXUINT, 64, G_PARAM_READWRITE));
}

static void g_gpiod_request_config_init(GPIODRequestConfig *self)
{
	self->handle = gpiod_request_config_new();
	if (!self->handle)
		/* The only possible error is ENOMEM. */
		g_error("Failed to allocate memory for the request-config object.");
}

GPIODRequestConfig *g_gpiod_request_config_new(const gchar *first_prop, ...)
{
	GPIODRequestConfig *settings;
	va_list va;

	va_start(va, first_prop);
	settings = G_GPIOD_REQUEST_CONFIG_OBJ(
			g_object_new_valist(G_GPIOD_REQUEST_CONFIG_TYPE,
					    first_prop, va));
	va_end(va);

	return settings;
}

void g_gpiod_request_config_set_consumer(GPIODRequestConfig *self,
					 const gchar *consumer)
{
	g_assert(self);

	g_gpiod_set_prop_string(G_OBJECT(self), "consumer", consumer);
}

const gchar *g_gpiod_request_config_get_consumer(GPIODRequestConfig *self)
{
	g_assert(self);

	return g_gpiod_get_prop_string(G_OBJECT(self), "consumer");
}

void g_gpiod_request_config_set_event_buffer_size(GPIODRequestConfig *self,
						  guint event_buffer_size)
{
	g_assert(self);

	g_gpiod_set_prop_uint(G_OBJECT(self), "event-buffer-size",
			      event_buffer_size);
}

guint g_gpiod_request_config_get_event_buffer_size(GPIODRequestConfig *self)
{
	g_assert(self);

	return g_gpiod_get_prop_uint(G_OBJECT(self), "event-buffer-size");
}
