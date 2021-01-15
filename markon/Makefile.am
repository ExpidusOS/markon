# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
# 
# Copyright (c) 2007-2010 Jannis Pohlmann <jannis@expidus.org>
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

AM_CPPFLAGS = 								\
	-I$(top_srcdir)							\
	-DMARKON_COMPILATION						\
	-DMARKON_VERSION_API=\"$(MARKON_VERSION_API)\"			\
	-DG_LOG_DOMAIN=\"markon\"					\
	-DSYSCONFIGDIR=\"$(sysconfdir)/xdg\"				\
	$(PLATFORM_CFLAGS)

lib_LTLIBRARIES = 							\
	libmarkon-1.la

libmarkon_built_public_sources =					\
	markon-marshal.h

libmarkon_built_sources =						\
	$(libmarkon_built_public_sources)				\
	markon-marshal.c

libmarkon_headers =							\
	markon.h							\
	markon-config.h							\
	markon-marshal.h						\
	markon-menu-element.h						\
	markon-menu-separator.h						\
	markon-menu-directory.h						\
	markon-menu-item-action.h						\
	markon-menu-item-pool.h						\
	markon-menu-item-cache.h					\
	markon-environment.h						\
	markon-menu.h							\
	markon-menu-item.h						\
	markon-menu-node.h						\
	markon-menu-tree-provider.h					\
	markon-menu-merger.h						\
	markon-menu-parser.h

libmarkon_sources =							\
	markon-config.c							\
	markon-marshal.c						\
	markon-menu-element.c						\
	markon-menu-separator.c						\
	markon-menu-directory.c						\
	markon-menu-item-action.c						\
	markon-menu-item-pool.c						\
	markon-menu-item-cache.c					\
	markon-environment.c						\
	markon-menu.c							\
	markon-menu-item.c						\
	markon-menu-node.c						\
	markon-menu-tree-provider.c					\
	markon-menu-merger.c						\
	markon-menu-parser.c						\
	markon-private.c						\
	markon-private.h

libmarkonincludedir = $(includedir)/markon-1/markon

libmarkoninclude_HEADERS =						\
	$(libmarkon_headers)

libmarkon_1_la_SOURCES =						\
	$(libmarkon_sources)						\
	$(libmarkon_headers)

libmarkon_1_la_CFLAGS = 						\
	$(GIO_CFLAGS)							\
	$(GLIB_CFLAGS)							\
	$(LIBEXPIDUS1UTIL_CFLAGS)						\
	$(GOBJECT_CFLAGS)						\
	$(GTHREAD_CFLAGS)						\
	$(PLATFORM_CFLAGS)

libmarkon_1_la_LDFLAGS =						\
	-no-undefined							\
	-export-dynamic							\
	-version-info $(MARKON_VERINFO)					\
	-export-symbols-regex "^[^_].*"					\
	$(PLATFORM_LDFLAGS)

libmarkon_1_la_LIBADD = 						\
	$(GIO_LIBS)							\
	$(GLIB_LIBS)							\
	$(LIBEXPIDUS1UTIL_LIBS)						\
	$(GOBJECT_LIBS)							\
	$(GTHREAD_LIBS)

EXTRA_DIST =								\
	markon-config.h.in						\
	markon-marshal.list

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = markon-1.pc

DISTCLEANFILES =							\
	stamp-markon-marshal.h						\
	markon-config.h							\
	$(libmarkon_built_sources)

BUILT_SOURCES =								\
	$(libmarkon_built_sources)

markon-marshal.h: stamp-markon-marshal.h
	@true

stamp-markon-marshal.h: markon-marshal.list Makefile
	$(AM_V_GEN) ( \
	  cd $(top_builddir)/markon \
	  && echo "#ifndef __MARKON_MARSHAL_H__" > xgen-emh \
	  && echo "#define __MARKON_MARSHAL_H__" >> xgen-emh \
	  && ( glib-genmarshal \
	  	--prefix=markon_marshal \
	  	--header $(srcdir)/markon-marshal.list ) >> xgen-emh \
	  && echo "#endif /* !__MARKON_MARSHAL_H__ */" >> xgen-emh \
	  && ( cmp -s xgen-emh markon-marshal.h \
	  	|| cp xgen-emh markon-marshal.h ) \
	  && rm -f xgen-emh \
	  && echo timestamp > $(@F) \
	)

markon-marshal.c: markon-marshal.list Makefile
	$(AM_V_GEN) ( \
	  cd $(top_builddir)/markon \
	  && echo "#include <markon/markon-marshal.h>" > xgen-emc \
	  && ( glib-genmarshal \
	  	--prefix=markon_marshal \
	  	--body $(srcdir)/markon-marshal.list ) >> xgen-emc \
	  && cp xgen-emc markon-marshal.c \
	  && rm -f xgen-emc \
	)

# Required for gtk-doc and make distcheck
dist-hook: all

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ENV = CC="$(CC)"
INTROSPECTION_SCANNER_ARGS = \
	--add-include-path=$(srcdir) \
	--warn-all \
	-DMARKON_COMPILATION \
	--symbol-prefix=g_ \
	--symbol-prefix=markon_ \
	--c-include=markon/markon.h
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
	--includedir=.

if HAVE_INTROSPECTION
introspection_sources = $(filter-out %private.h %private.c, $(libmarkon_1_la_SOURCES))

Markon-1.0.gir: libmarkon-1.la Makefile
Markon_1_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gio-2.0 Libexpidus1util-1.0
Markon_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
Markon_1_0_gir_LIBS = libmarkon-1.la
Markon_1_0_gir_FILES = $(introspection_sources)
Markon_1_0_gir_EXPORT_PACKAGES = markon-1
INTROSPECTION_GIRS += Markon-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

DISTCLEANFILES += $(gir_DATA) $(typelib_DATA)
endif
