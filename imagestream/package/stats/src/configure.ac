#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([stats], [1.0.5], [support@imagestream.com])
AM_INIT_AUTOMAKE([stats], [1.0.5])
AC_CONFIG_SRCDIR([include/stat_if.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([alarm bzero gethostname gettimeofday memset socket strcasecmp strchr strncasecmp strstr strtol])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
