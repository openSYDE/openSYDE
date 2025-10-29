# locale-de.m4
# serial 4
dnl Copyright (C) 2003-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl From Bruno Haible.

dnl Determine the name of a german locale with UTF-8 encoding.
AC_DEFUN([gt_LOCALE_DE_UTF8],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([AM_LANGINFO_CODESET])
  AC_CACHE_CHECK([for a german Unicode locale], [gt_cv_locale_de_utf8], [
    case "$host_os" in
      *-musl* | midipix*)
        dnl On musl libc, all kinds of ll_CC.UTF-8 locales exist, even without
        dnl any locale file on disk. But they are effectively equivalent to the
        dnl C.UTF-8 locale, except for locale categories (such as LC_MESSSAGES)
        dnl for which localizations (.mo files) have been installed.
        gt_cv_locale_de_utf8=de_DE.UTF-8
        ;;
      *)
        AC_LANG_CONFTEST([AC_LANG_SOURCE([[
#include <locale.h>
#include <time.h>
#if HAVE_LANGINFO_CODESET
# include <langinfo.h>
#endif
#include <stdlib.h>
#include <string.h>
struct tm t;
char buf[16];
int main () {
  /* On BeOS and Haiku, locales are not implemented in libc.  Rather, libintl
     imitates locale dependent behaviour by looking at the environment
     variables, and all locales use the UTF-8 encoding.  */
#if !(defined __BEOS__ || defined __HAIKU__)
  /* Check whether the given locale name is recognized by the system.  */
# if defined _WIN32 && !defined __CYGWIN__
  /* On native Windows, setlocale(category, "") looks at the system settings,
     not at the environment variables.  Also, when an encoding suffix such
     as ".65001" or ".54936" is specified, it succeeds but sets the LC_CTYPE
     category of the locale to "C".  */
  if (setlocale (LC_ALL, getenv ("LC_ALL")) == NULL
      || strcmp (setlocale (LC_CTYPE, NULL), "C") == 0)
    return 1;
# else
  if (setlocale (LC_ALL, "") == NULL) return 1;
# endif
  /* Check whether nl_langinfo(CODESET) is nonempty and not "ASCII" or "646".
     On Mac OS X 10.3.5 (Darwin 7.5) in the de_DE locale, nl_langinfo(CODESET)
     is empty, and the behaviour of Tcl 8.4 in this locale is not useful.
     On OpenBSD 4.0, when an unsupported locale is specified, setlocale()
     succeeds but then nl_langinfo(CODESET) is "646". In this situation,
     some unit tests fail.  */
# if HAVE_LANGINFO_CODESET
  {
    const char *cs = nl_langinfo (CODESET);
    if (cs[0] == '\0' || strcmp (cs, "ASCII") == 0 || strcmp (cs, "646") == 0)
      return 1;
  }
# endif
# ifdef __CYGWIN__
  /* On Cygwin, avoid locale names without encoding suffix, because the
     locale_charset() function relies on the encoding suffix.  Note that
     LC_ALL is set on the command line.  */
  if (strchr (getenv ("LC_ALL"), '.') == NULL) return 1;
# endif
  /* Check whether in the name of the third month, the second character
     (should be U+00E4: LATIN SMALL LETTER A WITH DIAERESIS) is two bytes
     long, with UTF-8 encoding.  */
  t.tm_year = 1965 - 1900; t.tm_mon = 3 - 1; t.tm_mday = 29;
  if (strftime (buf, sizeof (buf), "%B", &t) < 5
      || buf[1] != (char) 0xc3 || buf[2] != (char) 0xa4)
    return 1;
#endif
#if !defined __BIONIC__ /* Bionic libc's 'struct lconv' is just a dummy.  */
  /* Check whether the decimal separator is a comma.
     On NetBSD 3.0 in the de_DE.ISO8859-1 locale, localeconv()->decimal_point
     are nl_langinfo(RADIXCHAR) are both ".".  */
  if (localeconv () ->decimal_point[0] != ',') return 1;
#endif
  return 0;
}
          ]])])
        if AC_TRY_EVAL([ac_link]) && test -s conftest$ac_exeext; then
          case "$host_os" in
            # Handle native Windows specially, because there setlocale() interprets
            # "ar" as "Arabic" or "Arabic_Saudi Arabia.1256",
            # "fr" or "fra" as "French" or "French_France.1252",
            # "ge"(!) or "deu"(!) as "German" or "German_Germany.1252",
            # "ja" as "Japanese" or "Japanese_Japan.932",
            # and similar.
            mingw* | windows*)
              # Test for the hypothetical native Windows locale name.
              if (LC_ALL=German_Germany.65001 LC_TIME= LC_CTYPE= ./conftest; exit) 2>/dev/null; then
                gt_cv_locale_de_utf8=German_Germany.65001
              else
                # None found.
                gt_cv_locale_de_utf8=none
              fi
              ;;
            *)
              # Setting LC_ALL is not enough. Need to set LC_TIME to empty, because
              # otherwise on Mac OS X 10.3.5 the LC_TIME=C from the beginning of the
              # configure script would override the LC_ALL setting. Likewise for
              # LC_CTYPE, which is also set at the beginning of the configure script.
              # Test for the usual locale name.
              if (LC_ALL=de_DE LC_TIME= LC_CTYPE= ./conftest; exit) 2>/dev/null; then
                gt_cv_locale_de_utf8=de_DE
              else
                # Test for the locale name with explicit encoding suffix.
                if (LC_ALL=de_DE.UTF-8 LC_TIME= LC_CTYPE= ./conftest; exit) 2>/dev/null; then
                  gt_cv_locale_de_utf8=de_DE.UTF-8
                else
                  # Test for the Solaris 10 locale name.
                  if (LC_ALL=de.UTF-8 LC_TIME= LC_CTYPE= ./conftest; exit) 2>/dev/null; then
                    gt_cv_locale_de_utf8=de.UTF-8
                  else
                    # None found.
                    gt_cv_locale_de_utf8=none
                  fi
                fi
              fi
              ;;
          esac
        fi
        rm -fr conftest*
        ;;
    esac
  ])
  LOCALE_DE_UTF8=$gt_cv_locale_de_utf8
  AC_SUBST([LOCALE_DE_UTF8])
])
