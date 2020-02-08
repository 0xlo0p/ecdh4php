dnl $Id$
dnl config.m4 for extension wechat_ecdh

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(wechat_ecdh, for wechat_ecdh support,
[  --with-wechat_ecdh Include wechat_ecdh support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(wechat_ecdh, whether to enable wechat_ecdh support,
dnl Make sure that the comment is aligned:
dnl [  --enable-wechat_ecdh           Enable wechat_ecdh support])

if test "$PHP_WECHAT_ECDH" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-wechat_ecdh -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/wechat_ecdh.h"  # you most likely want to change this
  dnl if test -r $PHP_WECHAT_ECDH/$SEARCH_FOR; then # path given as parameter
  dnl   WECHAT_ECDH_DIR=$PHP_WECHAT_ECDH
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for wechat_ecdh files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       WECHAT_ECDH_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$WECHAT_ECDH_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the wechat_ecdh distribution])
  dnl fi

  dnl # --with-wechat_ecdh -> add include path
  dnl PHP_ADD_INCLUDE($WECHAT_ECDH_DIR/include)

  dnl # --with-wechat_ecdh -> check for lib and symbol presence
  dnl LIBNAME=wechat_ecdh # you may want to change this
  dnl LIBSYMBOL=wechat_ecdh # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WECHAT_ECDH_DIR/$PHP_LIBDIR, WECHAT_ECDH_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_WECHAT_ECDHLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong wechat_ecdh lib version or lib not found])
  dnl ],[
  dnl   -L$WECHAT_ECDH_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(WECHAT_ECDH_SHARED_LIBADD)

  PHP_NEW_EXTENSION(wechat_ecdh, wechat_ecdh.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

AC_ARG_ENABLE(debug,[--enable-debg  compile with debugging system],[PHP_DEBUG=$enableval], [PHP_DEBUG=no])
        
fi
