/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org> */

/**
 * @file chip.h
 */

#ifndef __GPIOD_GLIB_CHIP_H__
#define __GPIOD_GLIB_CHIP_H__

#if !defined (__GPIOD_GLIB_INSIDE__)
#error "Only <gpiod-glib.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "chip-info.h"
#include "line-config.h"
#include "line-info.h"
#include "line-request.h"
#include "request-config.h"

G_BEGIN_DECLS

/**
 * @cond
 */

G_DECLARE_FINAL_TYPE(GPIODChip, g_gpiod_chip, G_GPIOD, CHIP, GObject);

/**
 * @endcond
 *
 * @ingroup gpiod_glib
 * @{
 *
 * @defgroup gobject_chip GPIO chip GObject
 * @{
 *
 * This type represents a GPIO chip. In addition to the methods defined here,
 * it exposes the `info-event` signal with the following signature:
 *
 *     void (*)(GPIODChip *, GPIODInfoEvent *, gpointer)
 */

/**
 * @brief Get the GObject type for the GPIO chip.
 */
#define G_GPIOD_CHIP_TYPE (g_gpiod_chip_get_type())

/**
 * @brief Cast a GObject to a GPIO chip concrete GObject.
 */
#define G_GPIOD_CHIP_OBJ(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), G_GPIOD_CHIP_TYPE, GPIODChip))

/**
 * @brief Instantiates a new chip object by opening the device file indicated
 *        by \p path.
 * @param path Path to the device file to open.
 * @param err Return location for error or NULL.
 * @return New GPIO chip object.
 */
GPIODChip *g_gpiod_chip_new(const gchar *path, GError **err);

/**
 * @brief Close the GPIO chip device file and free associated resources.
 * @param self Chip object to manipulate.
 * @note The chip object can live after calling this method but any of
 *       the chip's methods will result in an error being set.
 */
void g_gpiod_chip_close(GPIODChip *self);

/**
 * @brief Check if this object is valid.
 * @param self Chip object to manipulate.
 * @return TRUE if this object's methods can be used, FALSE otherwise.
 *         False usually means the chip was closed. If the user calls
 *         any of the methods of this class on an object for which this
 *         operator returned false, a logic_error will be thrown.
 */
gboolean g_gpiod_chip_is_closed(GPIODChip *self);

/**
 * @brief Get the filesystem path that was used to open this GPIO chip.
 * @param self Chip object to manipulate.
 * @return Path to the underlying character device file.
 */
const gchar *g_gpiod_chip_get_path(GPIODChip *self);

/**
 * @brief Get information about the chip.
 * @param self Chip object to manipulate.
 * @param err Return location for error or NULL.
 * @return New GPIO chip info object.
 */
GPIODChipInfo *g_gpiod_chip_get_info(GPIODChip *self, GError **err);

/**
 * @brief Retrieve the current snapshot of line information for a single line.
 * @param self Chip object to manipulate.
 * @param offset Offset of the line to get the info for.
 * @param err Return location for error or NULL.
 * @return New GPIO line info object.
 */
GPIODLineInfo *
g_gpiod_chip_get_line_info(GPIODChip *self, guint offset, GError **err);

/**
 * @brief Retrieve the current snapshot of line information for a single line
 *        and start watching this line for future changes.
 * @param self Chip object to manipulate.
 * @param offset Offset of the line to get the info for and to watch.
 * @param err Return location for error or NULL.
 * @return New GPIO line info object.
 */
GPIODLineInfo *
g_gpiod_chip_watch_line_info(GPIODChip *self, guint offset, GError **err);

/**
 * @brief Stop watching the line at given offset for info events.
 * @param self Chip object to manipulate.
 * @param offset Offset of the line to get the info for.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean
g_gpiod_chip_unwatch_line_info(GPIODChip *self, guint offset, GError **err);

/**
 * @brief Map a GPIO line's name to its offset within the chip.
 * @param self Chip object to manipulate.
 * @param name Name of the GPIO line to map.
 * @param offset Return location for the mapped offset.
 * @param err Return location for error or NULL.
 * @return TRUE on success, FALSE on failure.
 */
gboolean
g_gpiod_chip_get_line_offset_from_name(GPIODChip *self, const gchar *name,
				       guint *offset, GError **err);

/**
 * @brief Request a set of lines for exclusive usage.
 * @param self Chip object to manipulate.
 * @param req_cfg Request config object. Can be NULL for default settings.
 * @param line_cfg Line config object.
 * @param err Return location for error or NULL.
 * @return New GPIO line request object or NULL on failure.
 */
GPIODLineRequest *g_gpiod_chip_request_lines(GPIODChip *self,
					     GPIODRequestConfig *req_cfg,
					     GPIODLineConfig *line_cfg,
					     GError **err);

/**
 * @}
 *
 * @}
 */

G_END_DECLS

#endif /* __GPIOD_GLIB_CHIP_H__ */
