#! /bin/sh

mkdir m4
autoreconf --install
intltoolize --automake --force --copy
