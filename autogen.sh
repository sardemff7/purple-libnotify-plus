#! /bin/sh

mkdir -p m4
intltoolize --automake --force --copy
autoreconf --install
