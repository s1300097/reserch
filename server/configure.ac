#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tndhms],[0.09],[daihinmin@ice.uec.ac.jp])
#AM_INIT_AUTOMAKE(tndhms, 0.09, daihinmin@ice.uec.ac.jp)
#AM_INIT_AUTOMAKE(tndhms, 0.09, daihinmin@ice.uec.ac.jp)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/daihu.c])
AC_CONFIG_HEADER([src/config.h])

# Checks for programs.
AC_PROG_CC
# AC_PATH_X
use_x11=$with_x
if test "$no_x" != "yes" ; then
  AC_PATH_XTRA
fi
if test "$use_x11" != "no" ; then
  no_x="no"
fi

# Checks for libraries.
AC_DEFUN([LIB_SOCKET_NSL],
[
        AC_SEARCH_LIBS([gethostbyname], [nsl])
        AC_SEARCH_LIBS([socket], [socket], [], [
                AC_CHECK_LIB([socket], [socket], [LIBS="-lsocket -lnsl $LIBS"],
                [], [-lnsl])])
])
LIB_SOCKET_NSL

if test "$no_x" != "yes" ; then
  LDFLAGS="$LDFLAGS $X_LIBS"
  AC_CHECK_LIB([X11], [XmbDrawString])
  AC_CHECK_LIB([Xpm], [XpmCreatePixmapFromData])
else
  LDFLAGS="$LDFLAGS"
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h inttypes.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/param.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

if test "$no_x" != "yes" ; then
  AC_CHECK_FUNCS([bzero inet_ntoa memset socket XCopyArea XpmReadFileToPixmap],,AC_MSG_ERROR(not found.))
else 
  AC_CHECK_FUNCS([bzero inet_ntoa memset socket],,AC_MSG_ERROR(not found.))
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
