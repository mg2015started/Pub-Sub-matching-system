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
dnl $Id: aclocal.m4,v 1.2 2002/11/22 17:52:34 carzanig Exp $
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
        #include <netdb.h>
        #include <sys/param.h>
      ], [int i = MAXHOSTNAMELEN;] , 
      ac_cv_anto_maxhostnamelen=yes, ac_cv_anto_maxhostnamelen=no))
  if test $ac_cv_anto_maxhostnamelen = no; then 
    AC_DEFINE(MAXHOSTNAMELEN, 256)
  fi])

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

