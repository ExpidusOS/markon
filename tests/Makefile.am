# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
# 
# Copyright (c) 2009 Jannis Pohlmann <jannis@expidus.org>
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

AM_CPPFLAGS =								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"markon-tests\"

noinst_PROGRAMS =							\
	test-menu-parser						\
	test-menu-spec							\
	test-display-menu-gtk3

# test-menu-parser
test_menu_parser_SOURCES =						\
	test-menu-parser.c

test_menu_parser_CFLAGS =						\
	$(LIBEXPIDUS1UTIL_CFLAGS)						\
	$(GIO_CFLAGS)							\
	$(GLIB_CFLAGS)							\
	$(GOBJECT_CFLAGS)

test_menu_parser_DEPENDENCIES =						\
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la

test_menu_parser_LDADD =						\
	$(LIBEXPIDUS1UTIL_LIBS)						\
	$(GIO_LIBS)							\
	$(GLIB_LIBS)							\
	$(GOBJECT_LIBS)							\
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la

# test-menu-spec
test_menu_spec_SOURCES =						\
	test-menu-spec.c

test_menu_spec_CFLAGS =							\
	$(LIBEXPIDUS1UTIL_CFLAGS)						\
	$(GIO_CFLAGS)							\
	$(GLIB_CFLAGS)							\
	$(GOBJECT_CFLAGS)

test_menu_spec_DEPENDENCIES =						\
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la

test_menu_spec_LDADD =							\
	$(LIBEXPIDUS1UTIL_LIBS)						\
	$(GIO_LIBS)							\
	$(GLIB_LIBS)							\
	$(GOBJECT_LIBS)							\
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la

# test-display-menu-gtk3
test_display_menu_gtk3_SOURCES =				\
	test-display-menu.c

test_display_menu_gtk3_CFLAGS =					\
	$(LIBEXPIDUS1UTIL_CFLAGS)						\
	$(GIO_CFLAGS)							\
	$(GLIB_CFLAGS)							\
	$(GOBJECT_CFLAGS)						\
	$(GTK3_CFLAGS)

test_display_menu_gtk3_DEPENDENCIES =				\
	$(top_builddir)/markon/libmarkon-1.la

test_display_menu_gtk3_LDADD =					\
	$(LIBEXPIDUS1UTIL_LIBS)						\
	$(GIO_LIBS)							\
	$(GLIB_LIBS)							\
	$(GOBJECT_LIBS)							\
	$(GTK3_LIBS)							\
	$(top_builddir)/markon/libmarkon-$(MARKON_VERSION_API).la	\
	$(top_builddir)/markon-gtk/libmarkon-gtk3-1.la
