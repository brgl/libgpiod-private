# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2022-2023 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

SUBDIRS = .

if WITH_TESTS

SUBDIRS += tests

endif

if WITH_EXAMPLES

SUBDIRS += examples

endif

lib_LTLIBRARIES = libgpiodglib.la

libgpiodglib_la_SOURCES = \
	gpiodglibchip.c \
	gpiodglibchipinfo.c \
	gpiodglibedgeevent.c \
	gpiodgliberror.c \
	gpiodglibinfoevent.c \
	gpiodglibinternal.c \
	gpiodglibinternal.h \
	gpiodgliblineconfig.c \
	gpiodgliblineinfo.c \
	gpiodgliblinerequest.c \
	gpiodgliblinesettings.c \
	gpiodglibmisc.c \
	gpiodglibrequestconfig.c

otherincludedir = $(includedir)/gpiodglib
otherinclude_HEADERS = \
	gpiodglibchip.h \
	gpiodglibchipinfo.h \
	gpiodglibedgeevent.h \
	gpiodgliberror.h \
	gpiodglibgeneratedenums.h \
	gpiodglibinfoevent.h \
	gpiodglibline.h \
	gpiodgliblineconfig.h \
	gpiodgliblineinfo.h \
	gpiodgliblinerequest.h \
	gpiodgliblinesettings.h \
	gpiodglibmisc.h \
	gpiodglibrequestconfig.h

EXTRA_DIST = \
	gpiodglibgeneratedenums.c.template \
	gpiodglibgeneratedenums.h.template

project_headers = \
	$(srcdir)/gpiodglibline.h \
	$(srcdir)/gpiodglibedgeevent.h \
	$(srcdir)/gpiodglibinfoevent.h

gpiodglibgeneratedenums.c: $(project_headers) gpiodglibgeneratedenums.c.template
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template=$(srcdir)/gpiodglibgeneratedenums.c.template \
		--output=$(builddir)/$@ \
		$(project_headers)

gpiodglibgeneratedenums.h: $(project_headers) gpiodglibgeneratedenums.h.template
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template=$(srcdir)/gpiodglibgeneratedenums.h.template \
		--output=$(srcdir)/$@ \
		$(project_headers)

nodist_libgpiodglib_la_SOURCES = \
	gpiodglibgeneratedenums.c \
	gpiodglibgeneratedenums.h

BUILT_SOURCES = $(nodist_libgpiodglib_la_SOURCES)
CLEANFILES = $(nodist_libgpiodglib_la_SOURCES)

libgpiodglib_la_CFLAGS = -Wall -Wextra -g
libgpiodglib_la_CFLAGS += -I$(top_srcdir)/include/ -include $(top_builddir)/config.h
libgpiodglib_la_CFLAGS += $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GIO_UNIX_CFLAGS)
libgpiodglib_la_CFLAGS += -DG_LOG_DOMAIN=\"gpiodglib\"
libgpiodglib_la_CFLAGS += $(PROFILING_CFLAGS)
libgpiodglib_la_LDFLAGS = -version-info $(subst .,:,$(ABI_GLIB_VERSION))
libgpiodglib_la_LDFLAGS += -lgpiod -L$(top_builddir)/lib
libgpiodglib_la_LDFLAGS += $(GLIB_LIBS) $(GIO_LIBS) $(GIO_UNIX_LIBS)
libgpiodglib_la_LDFLAGS += $(PROFILING_LDFLAGS)

include_HEADERS = gpiodglib.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gpiodglib.pc

if HAVE_INTROSPECTION

INTROSPECTION_GIRS = gpiodglib-1.0.gir

girdir = $(INTROSPECTION_GIRDIR)
gir_DATA = gpiodglib-1.0.gir

typelibsdir = $(INTROSPECTION_TYPELIBDIR)
typelibs_DATA = gpiodglib-1.0.typelib

gpiodglib_1_0_gir_SCANNERFLAGS = \
	--c-include="gpiodglib.h" \
	--warn-all \
	--namespace Gpiodglib \
	--identifier-prefix Gpiodglib \
	--symbol-prefix gpiodglib

gpiodglib_1_0_gir_CFLAGS = $(libgpiodglib_la_CFLAGS)

gpiodglib-1.0.gir: libgpiodglib.la
gpiodglib_1_0_gir_INCLUDES = Gio-2.0
gpiodglib_1_0_gir_LIBS = libgpiodglib.la
gpiodglib_1_0_gir_FILES = $(libgpiodglib_la_SOURCES)
gpiodglib_1_0_gir_EXPORT_PACKAGES = gpiodglib

include $(INTROSPECTION_MAKEFILE)

endif
