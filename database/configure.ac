AC_INIT([database], [1.0], [radio.internship.season40@endava.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CXX
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR([readline library not found])]) 
AC_CHECK_HEADERS([readline/readline.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT