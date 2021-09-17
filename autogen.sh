#! /bin/sh

set -x

aclocal -I m4 --install
libtoolize --copy --force --automake
autoheader --force
autoconf --force
automake --add-missing --copy --force --foreign


