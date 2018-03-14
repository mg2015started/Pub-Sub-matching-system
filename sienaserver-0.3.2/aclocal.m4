dnl
dnl  Author: Antonio Carzaniga <carzanig@cs.colorado.edu>
dnl
dnl  Copyright (c) 1999-2000  University of Colorado
dnl
dnl
dnl  This program is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU General Public License
dnl  as published by the Free Software Foundation; either version 2
dnl  of the License, or (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
dnl  USA, or send email to serl@cs.colorado.edu.
dnl 
dnl
dnl $Id: aclocal.m4,v 1.3 2002/11/22 20:57:45 carzanig Exp $
dnl

dnl
dnl AC_ANTO_SEARCH_LIBS(FUNCTION, SEARCH-LIBS [, ACTION-IF-FOUND
dnl                     [, ACTION-IF-NOT-FOUND [, OTHER-LIBRARIES]]])
dnl
dnl Search for a library defining FUNC, if it's not already available.
dnl
AC_DEFUN(AC_ANTO_SEARCH_LIBS, [
  AC_CACHE_CHECK([for library containing $1], [ac_cv_search_$1], [
    ac_func_search_save_LIBS="$LIBS"
    ac_cv_search_$1="no"
    AC_TRY_LINK_FUNC([$1], [ac_cv_search_$1="none required"])
    test "$ac_cv_search_$1" = "no" && for i in $2; do
      LIBS="-l$i $5 $ac_func_search_save_LIBS"
      AC_TRY_LINK_FUNC([$1], [
        ac_cv_search_$1="-l$i"
        break])
      done
    LIBS="$ac_func_search_save_LIBS"])
  if test "$ac_cv_search_$1" != "no"; then
    test "$ac_cv_search_$1" = "none required" || LIBS="$ac_cv_search_$1 $LIBS"
    $3
  else :
    $4
  fi])

dnl 
dnl  AC_ANTO_CXX_NAMESPACES([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl 
dnl  checks whether CXX implements namespaces
dnl 
AC_DEFUN(AC_ANTO_CXX_NAMESPACES,[
  AC_CACHE_CHECK([whether c++ implements namespaces], 
     ac_cv_anto_cxx_namespaces, 
     AC_TRY_COMPILE([
       namespace Outer { namespace Inner { int i = 0; }}
      ], [
       using namespace Outer::Inner; return i;
      ], ac_cv_anto_cxx_namespaces=yes, ac_cv_anto_cxx_namespaces=no))
  if test "$ac_cv_cxx_namespaces" = yes; then
    AC_DEFINE(HAVE_CXX_NAMESPACE)
    $1
  else :
    $2
  fi])

dnl 
dnl  AC_ANTO_CXX_EXCEPTIONS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl 
dnl  checks whether CXX implements exceptions
dnl 
AC_DEFUN(AC_ANTO_CXX_EXCEPTIONS,[
  AC_CACHE_CHECK([whether c++ implements exceptions], 
    ac_cv_anto_cxx_exceptions,
    AC_TRY_COMPILE([
      #include <exception>
      using namespace std;
      ], [
      try {
 	  int x;
       } catch (exception &ex) {
          throw;
       }
      ], ac_cv_anto_cxx_exceptions=yes, ac_cv_anto_cxx_exceptions=no))
  if test "$ac_cv_cxx_namespaces" = yes; then
    AC_DEFINE(HAVE_CXX_EXCEPTIONS)
    $1
  else :
    $2
  fi])

dnl
dnl  AC_ANTO_TYPE(TYPE, DEFAULT, [,EXTRA-INCLUDES])
dnl
dnl  looks for the specific type in network includes
dnl
AC_DEFUN(AC_ANTO_TYPE, [
  AC_CACHE_CHECK([for $1], ac_cv_anto_type_$1,
    AC_TRY_COMPILE([
#     include <sys/types.h> 
      ][
$3
      ], [
      $1 x;
      ], ac_cv_anto_type_$1=yes, ac_cv_anto_type_$1=no))
  if test $ac_cv_anto_type_$1 = no; then
    AC_DEFINE($1,$2)
  fi])

dnl 
dnl  check for MAXHOSTNAMELEN
dnl 
AC_DEFUN(AC_ANTO_MAXHOSTNAMELEN, [
  AC_CACHE_CHECK([for MAXHOSTNAMELEN], ac_cv_anto_maxhostnamelen, 
    AC_TRY_COMPILE([
#     include <netdb.h>
#     include <sys/param.h>], [int i = MAXHOSTNAMELEN;] , 
      ac_cv_anto_maxhostnamelen=yes, ac_cv_anto_maxhostnamelen=no))
  if test $ac_cv_anto_maxhostnamelen = no; then 
    AC_DEFINE(MAXHOSTNAMELEN, 256)
  fi])


dnl 
dnl  check for TIOCNOTTY
dnl 
AC_DEFUN(AC_ANTO_TIOCNOTTY, [
  AC_CACHE_CHECK([for TIOCNOTTY], ac_cv_anto_macro_tiocnotty, 
    AC_TRY_COMPILE([
      #include <termios.h>], [
      int i = TIOCNOTTY; ], 
      ac_cv_anto_macro_tiocnotty=yes, ac_cv_anto_macro_tiocnotty=no))
  if test $ac_cv_anto_macro_tiocnotty = yes; then
     AC_DEFINE(HAVE_TIOCNOTTY)
  fi])


dnl
dnl  AC_ANTO_LIBPAR(LIBRARY [, DEFAULTBASE])
dnl
AC_DEFUN(AC_ANTO_LIBPAR, 
  ifelse([$2], , [
  $1_include="$2/include"
  $1_libdir="$2/lib"
  ], )[
  AC_ARG_WITH($1-include,
    [  --with-$1-include=dir  finds $1 includes in dir], 
    $1_include="-I$withval")

  AC_ARG_WITH($1-libdir,
    [  --with-$1-libdir=dir  finds $1 libraries in dir], 
    $1_libdir="-L$withval")
  test -z "$$1_libs" && $1_libs="-l$1"
  AC_ARG_WITH($1-libs,
    [  --with-$1-libs=libraries  $1 libraries (default=-l$1)], 
    $1_libs="$withval")])

dnl
dnl  AC_ANTO_SAVEFLAGS(LIBRARY)
dnl
AC_DEFUN(AC_ANTO_SAVEFLAGS, [
  CFLAGS_before_$1="$CFALGS"
  CFLAGS="$$1_include $CFLAGS"
  CXXFLAGS_before_$1="$CXXFALGS"
  CXXFLAGS="$$1_include $CXXFLAGS"
  LDFLAGS_before_$1="$LDFLAGS"
  LDFLAGS="$$1_libdir $LDFLAGS"
  LIBS_before_$1="$LIBS"
  LIBS="$$1_libs $LIBS"
])

dnl
dnl  AC_ANTO_RESTOREFLAGS(LIBRARY)
dnl
AC_DEFUN(AC_ANTO_RESTOREFLAGS, [
  CFLAGS="$CFLAGS_before_$1"
  CXXFLAGS="$CXXFLAGS_before_$1"
  LDFLAGS="$LDFLAGS_before_$1"
  LIBS="$LIBS_before_$1"
])

dnl
dnl  AC_ANTO_PTHREAD_RWLOCK([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl  check for pthread read/write locks
dnl 
AC_DEFUN(AC_ANTO_PTHREAD_RWLOCK, [
  AC_CACHE_CHECK([for pthread_rwlock], ac_cv_anto_pthread_rwlock,
  AC_TRY_COMPILE([
      #include <pthread.h>
    ], [
      pthread_rwlock_t l;
      pthread_rwlock_init(&l, NULL);
    ], ac_cv_anto_pthread_rwlock=yes, ac_cv_anto_pthread_rwlock=no))
    if test $ac_cv_anto_pthread_rwlock = yes; then
      AC_DEFINE(HAVE_PTHREAD_RWLOCK)
      $1
    else :
      $2
    fi])

dnl
dnl  AC_ANTO_PTHREAD_YIELD([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl  check for pthread read/write locks
dnl 
AC_DEFUN(AC_ANTO_PTHREAD_YIELD, [
  AC_CACHE_CHECK([for pthread_yield], ac_cv_anto_pthread_yield,
  AC_TRY_LINK([
      #include <pthread.h>
    ], [
      pthread_yield();
    ], ac_cv_anto_pthread_yield=yes, ac_cv_anto_pthread_yield=no))
    if test $ac_cv_anto_pthread_yield = yes; then
      AC_DEFINE(HAVE_PTHREAD_YIELD)
      $1
    else :
      $2
    fi])

dnl
dnl  AC_ANTO_PTHREADS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl  check for POSIX threads
dnl 
AC_DEFUN(AC_ANTO_PTHREADS, [
  AC_ANTO_LIBPAR(pthread)
  AC_ANTO_SAVEFLAGS(pthread)
  #
  # look for POSIX threads
  # 
  AC_CACHE_CHECK([for POSIX 1.c threads], ac_cv_anto_pthread, 
    AC_TRY_LINK([
      #include <pthread.h>
      ], [
      pthread_create(0,0,0,0); pthread_attr_init(NULL);], 
      ac_cv_anto_pthread=yes, [
      LDFLAGS="-pthread $LDFLAGS"
	AC_TRY_LINK([
        #include <pthread.h>
        ], [
        pthread_create(0,0,0,0); 
        pthread_attr_init(NULL);
        ], [
        ac_cv_anto_pthread=yes
        pthread_libdir="-pthread $pthread_libdir"
        ], ac_cv_anto_pthread=no)]))
    if test $ac_cv_anto_pthread = yes; then
      AC_DEFINE(HAVE_PTHREADS)
      $1
    else :
      pthread_include=""
      pthread_libdir=""
      pthread_libs=""
      $2
    fi
    AC_ANTO_RESTOREFLAGS(pthread)
    AC_SUBST(pthread_include)
    AC_SUBST(pthread_libs)
    AC_SUBST(pthread_libdir)])

dnl 
dnl  AC_ANTO_GDBM([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl  check for gdbm
dnl 
AC_DEFUN(AC_ANTO_GDBM, [
  AC_ANTO_LIBPAR(gdbm)
  AC_ANTO_SAVEFLAGS(gdbm)
  AC_CACHE_CHECK([for gdbm library], ac_cv_anto_gdbm, 
    AC_TRY_LINK([
      #include <fcntl.h>
      #include <gdbm.h>], [
      GDBM_FILE   db;
      db =  gdbm_open("test", 0, O_RDWR | O_CREAT, 0600, (void (*)())0);], 
      ac_cv_anto_gdbm=yes, ac_cv_anto_gdbm=no))
  if test $ac_cv_anto_gdbm = yes; then
     AC_DEFINE(HAVE_GDBM)
     $1
  else :
     $2
  fi
  AC_ANTO_RESTOREFLAGS(gdbm)
  AC_SUBST(gdbm_include)
  AC_SUBST(gdbm_libs)
  AC_SUBST(gdbm_libdir)])


AC_DEFUN(AC_ANTO_SIENA, [
  siena_libs="-lsiena"
  AC_ANTO_LIBPAR(siena)
  AC_ANTO_SAVEFLAGS(siena)
  AC_CACHE_CHECK([for Siena], ac_cv_anto_siena, 
    AC_TRY_LINK([
         #include <siena.h>
         #include <siena/Monitor.h>
      ], [
         Event e; Filter f;
         Monitor * m = new SimpleMonitor("localhost", 1234);
      ], ac_cv_anto_siena=yes, ac_cv_anto_siena=no))
    if test $ac_cv_anto_siena = yes; then
      AC_DEFINE(HAVE_SIENA)
      $1
    else :
      siena_include=""
      siena_libdir=""
      siena_libs=""
      $2
    fi
    AC_SUBST(siena_include)
    AC_SUBST(siena_libs)
    AC_SUBST(siena_libdir)
    AC_ANTO_RESTOREFLAGS(siena)])

dnl  
dnl  AC_ANTO_MKPATH([ACTION-IF-NOT-FOUND][, ACTION-IF-NOT-FOUND])
dnl
dnl  check for a way to create an entire directory path 
dnl  tries `mkdir -p' and `$srcdir/mkinstalldirs'
dnl 
AC_DEFUN(AC_ANTO_MKPATH, [
  AC_CACHE_CHECK([how to create a directory path], ac_cv_anto_mkpath, [
    anto_td=_test_anto
    # paranoia...
    while test -r $anto_td; do anto_td="$anto_td."; done
    ac_cv_anto_mkpath=no
    if mkdir -p $anto_td/1/2 > /dev/null 2>&1 && test -d $anto_td/1/2; then
      /bin/rm -rf $anto_td
      ac_cv_anto_mkpath="mkdir -p"
    elif $srcdir/mkinstalldirs $anto_td/1/2 > /dev/null 2>&1 && test -d $anto_td/1/2; then
      ac_cv_anto_mkpath=\$\(srcdir\)/mkinstalldirs
    else
      ac_cv_anto_mkpath=no
    fi])
  if test "$ac_cv_anto_mkpath" = no; then
    MKPATH=""
    : 
    $1
  else
    MKPATH="$ac_cv_anto_mkpath"
    AC_SUBST(MKPATH)
    :
    $2
  fi])
