# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([spear_usbloader],[1.0.0])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB


AC_PREFIX_DEFAULT(/usr)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT32_T


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([endian.h],,[AC_MSG_ERROR([endian.h not found])])


# check for libusb
PKG_CHECK_MODULES([LIBUSB], [libusb])


cat << EOF
spear_usbloader has been configured with following options:

prefix:              ${prefix}
exec prefix:         ${exec_prefix}
User binaries:       ${bindir}

Host:                ${host}
Compiler:            ${CC}
Preprocessor flags:  ${CPPFLAGS}
Compiler flags:      ${CFLAGS}
Linker flags:        ${LDFLAGS}
Libraries:           ${LIBS}

LIBUSB_CFLAGS:         ${LIBUSB_CFLAGS}
LIBUSB_LIBS:           ${LIBUSB_LIBS}

EOF


AC_OUTPUT
