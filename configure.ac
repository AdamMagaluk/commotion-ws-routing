
AC_INIT(ws-routing,0.1)
AM_INIT_AUTOMAKE(ws-routing,0.1,no-define)

AC_CONFIG_SRCDIR([src/main.c])

AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX

#AC_HEADER_STDC
#AC_CHECK_HEADERS(unistd.h ,,AC_MSG_ERROR([required header file missing]))

# Checks for third party libs
AC_CHECK_LIB(websockets, main,,AC_MSG_ERROR(missing websockets.))
AC_CHECK_LIB(z, main,,AC_MSG_ERROR(missing z.))
AC_CHECK_LIB(jansson, main,,AC_MSG_ERROR(missing jansson.))


echo "================="
echo ""
echo "Thank you for configuring '$PACKAGE_NAME' version '$PACKAGE_VERSION'"
echo "Runtime:             $RUNTIME"
echo "C++ compiler:        $CXX"
echo "installation prefix: $prefix"
echo ""
echo "================="

AC_OUTPUT(Makefile src/Makefile)
