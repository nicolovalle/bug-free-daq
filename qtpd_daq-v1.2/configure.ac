#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(QTPD_DAQ, [m4_esyscmd_s([echo $VERSION])], support.computing@caen.it)
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects tar-ustar])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_DEFINE(LINUX,[1],"Define LINUX")

# Checks for CAEN libraries
vme_not_found_msg="CAENVMELib must be installed on your system."
AC_CHECK_HEADER([CAENVMElib.h], [], [AC_MSG_ERROR($vme_not_found_msg)])
AC_SEARCH_LIBS([CAENVME_Init2], [CAENVME], [], [AC_MSG_ERROR($vme_not_found_msg)])

# Checks for programs
AC_CHECK_PROG(GNUPLOT, gnuplot, "yes", "no")
if test $GNUPLOT == "no"; then
  AC_MSG_ERROR(
    *****************************************************************
    GnuPlot not found. Please install it and retry.
    *****************************************************************
  )
fi

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT
