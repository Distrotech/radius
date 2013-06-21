# wcsdup.m4 serial 2
dnl Copyright (C) 2011-2013 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCSDUP],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([wcsdup])
  if test $ac_cv_func_wcsdup = no; then
    HAVE_WCSDUP=0
  fi
])
