# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

SUBDIRS = . gpiod-glib

if WITH_TESTS

SUBDIRS += tests

endif

if WITH_EXAMPLES

SUBDIRS += examples

endif

lib_LTLIBRARIES = libgpiod-glib.la

libgpiod_glib_la_SOURCES = \
	chip.c \
	chip-info.c \
	edge-event.c \
	error.c \
	info-event.c \
	internal.c \
	internal.h \
	line-config.c \
	line-info.c \
	line-request.c \
	line-settings.c \
	misc.c \
	request-config.c

EXTRA_DIST = \
	generated-enums.c.template \
	generated-enums.h.template

project_headers = \
	$(srcdir)/gpiod-glib/line.h \
	$(srcdir)/gpiod-glib/edge-event.h \
	$(srcdir)/gpiod-glib/info-event.h

generated-enums.c: $(project_headers) generated-enums.c.template
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template=$(srcdir)/generated-enums.c.template \
		--output=$(builddir)/$@ \
		$(project_headers)

gpiod-glib/generated-enums.h: $(project_headers) generated-enums.h.template
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template=$(srcdir)/generated-enums.h.template \
		--output=$(srcdir)/$@ \
		$(project_headers)

nodist_libgpiod_glib_la_SOURCES = \
	generated-enums.c \
	gpiod-glib/generated-enums.h

BUILT_SOURCES = $(nodist_libgpiod_glib_la_SOURCES)
CLEANFILES = $(nodist_libgpiod_glib_la_SOURCES)

libgpiod_glib_la_CFLAGS = -Wall -Wextra -g
libgpiod_glib_la_CFLAGS += -I$(top_srcdir)/include/ -include $(top_builddir)/config.h
libgpiod_glib_la_CFLAGS += $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GIO_UNIX_CFLAGS)
libgpiod_glib_la_CFLAGS += -DG_LOG_DOMAIN=\"gpiod-glib\"
libgpiod_glib_la_CFLAGS += $(PROFILING_CFLAGS)
libgpiod_glib_la_LDFLAGS = -version-info $(subst .,:,$(ABI_GLIB_VERSION))
libgpiod_glib_la_LDFLAGS += -lgpiod -L$(top_builddir)/lib
libgpiod_glib_la_LDFLAGS += $(GLIB_LIBS) $(GIO_LIBS) $(GIO_UNIX_LIBS)
libgpiod_glib_la_LDFLAGS += $(PROFILING_LDFLAGS)

include_HEADERS = gpiod-glib.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gpiod-glib.pc

if HAVE_INTROSPECTION

INTROSPECTION_GIRS = gpiod-glib-1.0.gir

girdir = $(INTROSPECTION_GIRDIR)
gir_DATA = gpiod-glib-1.0.gir

typelibsdir = $(INTROSPECTION_TYPELIBDIR)
typelibs_DATA = gpiod-glib-1.0.typelib

gpiod_glib_1_0_gir_SCANNERFLAGS = \
	--c-include="gpiod-glib.h" \
	--warn-all \
	--namespace Gpiodglib \
	--identifier-prefix Gpiodglib \
	--symbol-prefix gpiodglib

gpiod_glib_1_0_gir_CFLAGS = $(libgpiod_glib_la_CFLAGS)

gpiod-glib-1.0.gir: libgpiod-glib.la
gpiod_glib_1_0_gir_INCLUDES = Gio-2.0
gpiod_glib_1_0_gir_LIBS = libgpiod-glib.la
gpiod_glib_1_0_gir_FILES = $(libgpiod_glib_la_SOURCES)
gpiod_glib_1_0_gir_EXPORT_PACKAGES = gpiod-glib

include $(INTROSPECTION_MAKEFILE)

endif
