# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
#
# Copyright (c) 2010 Expidus Development Team
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

menudir = $(sysconfdir)/xdg/menus
menu_DATA =								\
	expidus-applications.menu

directorydir = $(datadir)/desktop-directories
directory_in_files =							\
	expidus-accessories.directory.in					\
	expidus-development.directory.in					\
	expidus-education.directory.in					\
	expidus-games.directory.in						\
	expidus-graphics.directory.in					\
	expidus-hardware.directory.in					\
	expidus-multimedia.directory.in					\
	expidus-network.directory.in					\
	expidus-office.directory.in					\
	expidus-other.directory.in						\
	expidus-personal.directory.in					\
	expidus-screensavers.directory.in					\
	expidus-settings.directory.in					\
	expidus-system.directory.in

directory_DATA = $(directory_in_files:.directory.in=.directory)
@INTLTOOL_DIRECTORY_RULE@

EXTRA_DIST =								\
	$(directory_in_files)						\
	$(menu_DATA)

DISTCLEANFILES =							\
	$(directory_DATA)
