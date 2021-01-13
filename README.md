[![License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://gitlab.expidus.org/expidus/garcon/-/blob/master/COPYING)

# garcon


Garcon is an implementation of the [[https://specifications.freedesktop.org/menu-spec/latest/|freedesktop.org compliant menu]] specification
replacing the former Expidus menu library libexpidus1menu. It is based on
GLib/GIO and aims at covering the entire specification except for
legacy menus. It was started as a complete rewrite of the former
Expidus menu library called libexpidus1menu, which, in contrast to garcon,
was lacking menu merging features essential for loading menus modified
with menu editors.

----

### Homepage

[Garcon documentation](https://docs.expidus.org/expidus/garcon/start)

### Changelog

See [NEWS](https://gitlab.expidus.org/expidus/garcon/-/blob/master/NEWS) for details on changes and fixes made in the current release.

### Source Code Repository

[Garcon source code](https://gitlab.expidus.org/expidus/garcon)

### Download a Release Tarball

[Garcon archive](https://archive.expidus.org/src/expidus/garcon)
    or
[Garcon tags](https://gitlab.expidus.org/expidus/garcon/-/tags)

### Installation

From source: 

    % cd garcon
    % ./autogen.sh
    % make
    % make install

From release tarball:

    % tar xf garcon-<version>.tar.bz2
    % cd garcon-<version>
    % ./configure
    % make
    % make install

### Reporting Bugs

Visit the [reporting bugs](https://docs.expidus.org/expidus/garcon/bugs) page to view currently open bug reports and instructions on reporting new bugs or submitting bugfixes.

