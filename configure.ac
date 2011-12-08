#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.68])
AC_INIT([smrgygurdy], [1.0], [nick@n-ism.org])
AM_INIT_AUTOMAKE(smrgygurdy, 1.0)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX([], [-O3])
AC_PROG_MAKE_SET
AC_PROG_CPP
AC_PROG_CC
AC_PROG_INSTALL

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MKTIME
AC_CHECK_FUNCS([gettimeofday memset pow])

# Checks for libraries.
AC_CHECK_LIB([m], [pow], [], [
    AC_MSG_ERROR([cannot find libm])
])

dnl For libraries which support pkg-config, use the PKG_CHECK_MODULES
dnl macro. Don't forget to add an appropriate entry in src/Makefile.am
dnl to actually include the library.

PKG_CHECK_MODULES([LIBHID], [libhid, libusb], [], [
    AC_MSG_WARN([could not find libusb (needed with MiniLAB-1008 interface)])
])

PKG_CHECK_MODULES([ARTIFASTRING], [artifastring], [], [
    AC_MSG_ERROR([cannot find libartifastring])
])

AC_CHECK_LIB([asound], [snd_seq_open], [], [
    AC_MSG_ERROR([cannot find libasound])
])

AC_CHECK_LIB([mcchid], [volts_LS], [], [
    AC_MSG_ERROR([
        cannot find libmcchid.
        Perhaps it's installed in /usr/local/lib or elsewhere?
        Set e.g. LDFLAGS=-L/usr/local/lib CXXFLAGS=-I/usr/local/include
    ])
], [$LIBHID_LIBS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
