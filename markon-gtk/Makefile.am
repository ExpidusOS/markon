# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
# 
# Copyright (c) 2013 Nick Schermer <nick@expidus.org>
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU Library General Public License for more details.
# 
# You should have received a copy of the GNU Library General 
# Public License along with this library; if not, write to the 
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DMARKON_COMPILATION \
	-DMARKON_VERSION_API=\"$(MARKON_VERSION_API)\" \
	-DG_LOG_DOMAIN=\"markon\" \
	-DSYSCONFIGDIR=\"$(sysconfdir)/xdg\" \
	$(PLATFORM_CFLAGS)

lib_LTLIBRARIES = \
	libmarkon-gtk3-1.la

libmarkon_gtk_headers = \
	markon-gtk.h \
	markon-gtk-menu.h

libmarkon_gtk_sources = \
	markon-gtk-menu.c


libmarkon_gtk3includedir = \
	$(includedir)/markon-gtk3-1/markon-gtk

libmarkon_gtk3include_HEADERS = \
	$(libmarkon_gtk_headers)

libmarkon_gtk3_1_la_SOURCES = \
	$(libmarkon_gtk_sources) \
	$(libmarkon_gtk_headers)

libmarkon_gtk3_1_la_CFLAGS = \
	$(GIO_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GTK3_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	$(LIBEXPIDUS1UI2_CFLAGS) \
	$(PLATFORM_CFLAGS)

libmarkon_gtk3_1_la_LDFLAGS = \
	-no-undefined \
	-export-dynamic \
	-version-info $(MARKON_VERINFO) \
	-export-symbols-regex "^[^_].*" \
	$(PLATFORM_LDFLAGS)

libmarkon_gtk3_1_la_LIBADD = \
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la \
	$(GIO_LIBS) \
	$(GLIB_LIBS) \
	$(GTK3_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(GOBJECT_LIBS) \
	$(GTHREAD_LIBS) \
	$(LIBEXPIDUS1UI2_LIBS)

libmarkon_gtk3_1_la_DEPENDENCIES = \
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = markon-gtk3-1.pc


# Required for gtk-doc and make distcheck
dist-hook: all

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ENV = CC="$(CC)"
INTROSPECTION_SCANNER_ARGS = \
	--add-include-path=$(srcdir) \
	--add-include-path=$(top_buildir)/markon \
	--include-uninstalled=$(top_builddir)/markon/Markon-1.0.gir \
	--warn-all \
	-DMARKON_COMPILATION \
	--c-include=markon-gtk/markon-gtk.h
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
	--includedir=. \
	--includedir=../markon

if HAVE_INTROSPECTION
introspection_sources = $(libmarkon_gtk3_1_la_SOURCES)

MarkonGtk-1.0.gir: libmarkon-gtk3-1.la \
	$(top_builddir)/markon/Markon-1.0.gir Makefile
MarkonGtk_1_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gio-2.0 \
	Gtk-3.0 Libexpidus1util-1.0 Libexpidus1ui-2.0
MarkonGtk_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
MarkonGtk_1_0_gir_LIBS = libmarkon-gtk3-1.la \
	$(top_builddir)/markon/libmarkon-1.la
MarkonGtk_1_0_gir_FILES = $(introspection_sources)
MarkonGtk_1_0_gir_EXPORT_PACKAGES = markon-gtk3-1
INTROSPECTION_GIRS += MarkonGtk-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

DISTCLEANFILES = $(gir_DATA) $(typelib_DATA)
endif
