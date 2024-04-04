// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#include <gio/gio.h>
#include <gpiod.h>
#include <gpiod-glib.h>

#include "internal.h"

struct _GPIODChipInfo {
	GObject parent_instance;
	struct gpiod_chip_info *handle;
};

enum {
	G_GPIOD_CHIP_INFO_PROP_HANDLE = 1,
	G_GPIOD_CHIP_INFO_PROP_NAME,
	G_GPIOD_CHIP_INFO_PROP_LABEL,
	G_GPIOD_CHIP_INFO_PROP_NUM_LINES,
};

G_DEFINE_TYPE(GPIODChipInfo, g_gpiod_chip_info, G_TYPE_OBJECT);

static void g_gpiod_chip_info_get_property(GObject *obj, guint prop_id,
					   GValue *val, GParamSpec *pspec)
{
	GPIODChipInfo *self = G_GPIOD_CHIP_INFO_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_CHIP_INFO_PROP_NAME:
		g_value_set_static_string(val,
			gpiod_chip_info_get_name(self->handle));
		break;
	case G_GPIOD_CHIP_INFO_PROP_LABEL:
		g_value_set_static_string(val,
			gpiod_chip_info_get_label(self->handle));
		break;
	case G_GPIOD_CHIP_INFO_PROP_NUM_LINES:
		g_value_set_uint(val,
			gpiod_chip_info_get_num_lines(self->handle));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_chip_info_set_property(GObject *obj, guint prop_id,
					   const GValue *val, GParamSpec *pspec)
{
	GPIODChipInfo *self = G_GPIOD_CHIP_INFO_OBJ(obj);

	switch (prop_id) {
	case G_GPIOD_CHIP_INFO_PROP_HANDLE:
		self->handle = g_value_get_pointer(val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void g_gpiod_chip_info_finalize(GObject *obj)
{
	GPIODChipInfo *self = G_GPIOD_CHIP_INFO_OBJ(obj);

	g_clear_pointer(&self->handle, gpiod_chip_info_free);

	G_OBJECT_CLASS(g_gpiod_chip_info_parent_class)->finalize(obj);
}

static void g_gpiod_chip_info_class_init(GPIODChipInfoClass *chip_info_class)
{
	GObjectClass *class = G_OBJECT_CLASS(chip_info_class);

	class->set_property = g_gpiod_chip_info_set_property;
	class->get_property = g_gpiod_chip_info_get_property;
	class->finalize = g_gpiod_chip_info_finalize;

	g_object_class_install_property(class, G_GPIOD_CHIP_INFO_PROP_HANDLE,
		g_param_spec_pointer("handle", "Handle",
			"GPIO Chip information object.",
			G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

	g_object_class_install_property(class, G_GPIOD_CHIP_INFO_PROP_NAME,
		g_param_spec_string("name", "Name",
			"Name of this GPIO chip device.", NULL,
			G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_CHIP_INFO_PROP_LABEL,
		g_param_spec_string("label", "Label",
			"Label of this GPIO chip device.", NULL,
			G_PARAM_READABLE));

	g_object_class_install_property(class, G_GPIOD_CHIP_INFO_PROP_NUM_LINES,
		g_param_spec_uint("num-lines", "NumLines",
			"Number of GPIO lines exposed by this chip.",
			1, G_MAXUINT, 1,
			G_PARAM_READABLE));
}

static void g_gpiod_chip_info_init(GPIODChipInfo *self)
{
	self->handle = NULL;
}

const gchar *g_gpiod_chip_info_get_name(GPIODChipInfo *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "name");
}

const gchar *g_gpiod_chip_info_get_label(GPIODChipInfo *self)
{
	return g_gpiod_get_prop_string(G_OBJECT(self), "label");
}

guint g_gpiod_chip_info_get_num_lines(GPIODChipInfo *self)
{
	return g_gpiod_get_prop_uint(G_OBJECT(self), "num-lines");
}
