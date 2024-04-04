// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODLineConfig {
	GObject parent_instance;
	struct gpiod_line_config *handle;
};

enum {
	G_GPIOD_LINE_CONFIG_PROP_HANDLE = 1,
	G_GPIOD_LINE_CONFIG_PROP_CONFIGURED_OFFSETS,
};

G_DEFINE_TYPE(GPIODLineConfig, g_gpiod_line_config, G_TYPE_OBJECT);

static void g_gpiod_line_config_get_property(GObject *obj, guint prop_id,
					     GValue *val, GParamSpec *pspec)
{
	GPIODLineConfig *self = G_GPIOD_LINE_CONFIG_OBJ(obj);
	g_autofree guint *offsets = NULL;
	gsize num_offsets, i;
	GArray *boxed;

	switch (prop_id) {
	case G_GPIOD_LINE_CONFIG_PROP_HANDLE:
		g_value_set_pointer(val, self->handle);
		break;
	case G_GPIOD_LINE_CONFIG_PROP_CONFIGURED_OFFSETS:
		num_offsets = gpiod_line_config_get_num_configured_offsets(
								self->handle);
		offsets = g_malloc0(num_offsets * sizeof(guint));
		gpiod_line_config_get_configured_offsets(self->handle, offsets,
							 num_offsets);

		boxed = g_array_new(FALSE, TRUE, sizeof(guint));
		for (i = 0; i < num_offsets; i++)
			g_array_append_val(boxed, offsets[i]);

		g_value_set_boxed(val, boxed);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_line_config_finalize(GObject *obj)
{
	GPIODLineConfig *self = G_GPIOD_LINE_CONFIG_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_line_config_free);

	G_OBJECT_CLASS(g_gpiod_line_config_parent_class)->finalize(obj);
}

static void
g_gpiod_line_config_class_init(GPIODLineConfigClass *line_config_class)
{
	GObjectClass *class = G_OBJECT_CLASS(line_config_class);

	class->get_property = g_gpiod_line_config_get_property;
	class->finalize = g_gpiod_line_config_finalize;

	g_object_class_install_property(class, G_GPIOD_LINE_CONFIG_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO line config object.",
			G_PARAM_READABLE));

	g_object_class_install_property(class,
				G_GPIOD_LINE_CONFIG_PROP_CONFIGURED_OFFSETS,
		g_param_spec_boxed("configured-offsets", "Configured Offsets",
			"Array of offsets for which line settings have been set.",
			G_TYPE_ARRAY,
			G_PARAM_READABLE));
}

static void g_gpiod_line_config_init(GPIODLineConfig *self)
{
	self->handle = gpiod_line_config_new();
	if (!self->handle)
		/* The only possible error is ENOMEM. */
		g_error("Failed to allocate memory for the request-config object.");
}

GPIODLineConfig *g_gpiod_line_config_new(void)
{
	return G_GPIOD_LINE_CONFIG_OBJ(
			g_object_new(G_GPIOD_LINE_CONFIG_TYPE, NULL));
}

void g_gpiod_line_config_reset(GPIODLineConfig *self)
{
	g_assert(self);

	gpiod_line_config_reset(self->handle);
}

gboolean g_gpiod_line_config_add_line_settings(GPIODLineConfig *self,
					       const GArray *offsets,
					       GPIODLineSettings *settings,
					       GError **err)
{
	struct gpiod_line_settings *settings_handle;
	int ret;

	g_assert(self);

	if (!offsets || !offsets->len) {
		g_set_error(err, G_GPIOD_ERROR, G_GPIOD_ERR_INVAL,
			    "at least one offset must be specified when adding line settings");
		return FALSE;
	}

	settings_handle = settings ?
		g_gpiod_get_prop_pointer(G_OBJECT(settings), "handle") : NULL;
	ret = gpiod_line_config_add_line_settings(self->handle,
						  (unsigned int *)offsets->data,
						  offsets->len,
						  settings_handle);
	if (ret) {
		g_gpiod_set_error_from_errno(err,
			"failed to add line settings to line config");
		return FALSE;
	}

	return TRUE;
}

GPIODLineSettings *
g_gpiod_line_config_get_line_settings(GPIODLineConfig *self, guint offset)
{
	struct gpiod_line_settings *settings;

	g_assert(self);

	settings = gpiod_line_config_get_line_settings(self->handle, offset);
	if (!settings) {
		if (errno == ENOENT)
			return NULL;

		/* Let's bail-out on ENOMEM/ */
		g_error("failed to retrieve line settings for offset %u: %s",
			offset, g_strerror(errno));
	}

	return G_GPIOD_LINE_SETTINGS_OBJ(
		g_object_new(G_GPIOD_LINE_SETTINGS_TYPE,
			     "handle", settings, NULL));
}

gboolean g_gpiod_line_config_set_output_values(GPIODLineConfig *self,
					       const GArray *values,
					       GError **err)
{
	g_autofree enum gpiod_line_value *vals = NULL;
	gint ret;
	guint i;

	g_assert(self);

	vals = g_malloc0(sizeof(*vals) * values->len);
	for (i = 0; i < values->len; i++)
		vals[i] = g_gpiod_line_value_to_library(
				g_array_index(values, GPIODLineValue, i));

	ret = gpiod_line_config_set_output_values(self->handle, vals,
						  values->len);
	if (ret) {
		g_gpiod_set_error_from_errno(err,
				"unable to set output values");
		return FALSE;
	}

	return TRUE;
}

GArray *g_gpiod_line_config_get_configured_offsets(GPIODLineConfig *self)
{
	return g_gpiod_get_prop_boxed_array(G_OBJECT(self),
					    "configured-offsets");
}
