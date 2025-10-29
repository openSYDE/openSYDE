/* Rust format strings.
   Copyright (C) 2001-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2025.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "format.h"
#include "c-ctype.h"
#include "unictype.h"
#include "unistr.h"
#include "xalloc.h"
#include "xvasprintf.h"
#include "format-invalid.h"
#include "gettext.h"

#define _(str) gettext (str)

/* Rust format strings are those supported by the 'formatx' library
     <https://crates.io/crates/formatx>
     <https://lib.rs/crates/formatx>
     <https://github.com/clitic/formatx>
   namely those supported by the 'format!' built-in
     <https://doc.rust-lang.org/std/fmt/>
   with the restrictions listed in
     <https://crates.io/crates/formatx>, section "Limitations".

   A format string directive here consists of
     - an opening brace '{',
     - an optional non-empty sequence of digits
       or an optional identifier_or_keyword according to
       <https://doc.rust-lang.org/1.84.0/reference/identifiers.html>
       (that is, a XID_Start character and a sequence of XID_Continue characters
       or an underscore '_' and a non-empty sequence of XID_Continue characters),
     - optionally, a ':' and a format specifier, where a format specifier is
       of the form [[fill]align][sign][#][0][minimumwidth][.precision][type]
       where
         - the fill character is any character,
         - the align flag is one of '<', '>', '^',
         - the sign is one of '+', '-',
         - the # flag is '#',
         - the 0 flag is '0',
         - minimumwidth is a non-empty sequence of digits,
         - precision is a non-empty sequence of digits,
         - type is one of
           - '?', for any type of argument,
     - optionally, a sequence of Unicode (UTF-8) characters with property
       White_Space,
     - a closing brace '}'.
   Brace characters '{' and '}' can be escaped by doubling them: '{{' and '}}'.

   Numbered ('{m}') and unnumbered ('{}') argument specifications cannot be used
   in the same string; that's unsupported (although it does not always lead to
   an error at runtime, see <https://github.com/clitic/formatx/issues/7>).

   Named ('{name}') and unnamed ('{m}', '{}') argument specifications can be
   used in the same string.  In the formatx! arguments, all unnamed arguments
   must come before all named arguments; but this is not a restriction for the
   format string.

   In the 'format!' built-in, all arguments that are passed must be used by the
   format string, but this is not a requirement for formatx!:
     formatx!("{1} {1}", 44, 55)
     formatx!("{}", 9, a = 47)
   are valid (see <https://github.com/clitic/formatx/issues/8>).  */

struct named_arg
{
  char *name;
};

struct numbered_arg
{
  /* The number of the argument, 0-based.  */
  unsigned int number;
};

struct spec
{
  unsigned int directives;
  unsigned int named_arg_count;
  unsigned int numbered_arg_count;
  struct named_arg *named;
  struct numbered_arg *numbered;
};


/* All the parse_* functions (except parse_upto) follow the same
   calling convention.  FORMATP shall point to the beginning of a token.
   If parsing succeeds, FORMATP will point to the next character after
   the token, and true is returned.  Otherwise, FORMATP will be
   unchanged and false is returned.  */

static int
named_arg_compare (const void *p1, const void *p2)
{
  return strcmp (((const struct named_arg *) p1)->name,
                 ((const struct named_arg *) p2)->name);
}

static int
numbered_arg_compare (const void *p1, const void *p2)
{
  unsigned int n1 = ((const struct numbered_arg *) p1)->number;
  unsigned int n2 = ((const struct numbered_arg *) p2)->number;

  return (n1 > n2 ? 1 : n1 < n2 ? -1 : 0);
}

static void *
format_parse (const char *format, bool translated, char *fdi,
              char **invalid_reason)
{
  struct spec spec;
  unsigned int named_allocated;
  unsigned int numbered_allocated;
  bool seen_numbered_args;
  unsigned int unnumbered_arg_count;
  struct spec *result;

  spec.directives = 0;
  spec.named_arg_count = 0;
  spec.numbered_arg_count = 0;
  spec.named = NULL;
  spec.numbered = NULL;
  named_allocated = 0;
  numbered_allocated = 0;
  seen_numbered_args = false;
  unnumbered_arg_count = 0;

  for (; *format != '\0';)
    {
      /* Invariant: !seen_numbered_args || unnumbered_arg_count == 0.  */
      if (*format == '{')
        {
          char c;

          c = *++format;
          if (c == '{')
            {
              /* An escaped '{'.  */
            }
          else
            {
              const char *const format_start = format;
              bool seen_identifier_or_keyword = false;
              unsigned int arg_id;

              if (c_isdigit (*format))
                {
                  /* Numbered and unnumbered specifications are exclusive.  */
                  if (unnumbered_arg_count > 0)
                    {
                      *invalid_reason = INVALID_MIXES_NUMBERED_UNNUMBERED ();
                      FDI_SET (format, FMTDIR_ERROR);
                      goto bad_format;
                    }
                  seen_numbered_args = true;

                  arg_id = 0;
                  do
                    {
                      if (arg_id >= UINT_MAX / 10)
                        {
                          *invalid_reason =
                            xasprintf (_("In the directive number %u, the argument number is too large."), spec.directives);
                          FDI_SET (format, FMTDIR_ERROR);
                          goto bad_format;
                        }
                      /* Here arg_id <= floor(UINT_MAX/10) - 1.  */
                      arg_id = arg_id * 10 + (*format - '0');
                      /* Here arg_id < floor(UINT_MAX/10)*10 <= UINT_MAX.  */
                      format++;
                    }
                  while (c_isdigit (*format));
                }
              else
                {
                  /* Try to parse an identifier_or_keyword (that is,
                     - a XID_Start character and a sequence of XID_Continue
                       characters
                     - or an underscore '_' and a non-empty sequence of
                       XID_Continue characters).  */
                  {
                    ucs4_t uc1;
                    int n1 = u8_mbtouc (&uc1,
                                       (const uint8_t *) format,
                                       strnlen (format, 4));
                    if (n1 > 0
                        && (uc_is_property_xid_start (uc1) || uc1 == '_'))
                      {
                        const char *name_start = format;
                        const char *f = format + n1;

                        for (;;)
                          {
                            ucs4_t uc;
                            int n = u8_mbtouc (&uc,
                                               (const uint8_t *) f,
                                               strnlen (f, 4));
                            if (n > 0 && uc_is_property_xid_continue (uc))
                              f += n;
                            else
                              break;
                          }
                        if (uc1 != '_' || f > format + 1)
                          {
                            const char *name_end = f;
                            size_t n = name_end - name_start;
                            char *name = XNMALLOC (n + 1, char);
                            memcpy (name, name_start, n);
                            name[n] = '\0';

                            if (named_allocated == spec.named_arg_count)
                              {
                                named_allocated = 2 * named_allocated + 1;
                                spec.named = (struct named_arg *) xrealloc (spec.named, named_allocated * sizeof (struct named_arg));
                              }
                            spec.named[spec.named_arg_count].name = name;
                            spec.named_arg_count++;

                            format = f;
                            seen_identifier_or_keyword = true;
                          }
                      }
                  }
                  if (!seen_identifier_or_keyword)
                    {
                      /* Numbered and unnumbered specifications are exclusive.  */
                      if (seen_numbered_args > 0)
                        {
                          *invalid_reason = INVALID_MIXES_NUMBERED_UNNUMBERED ();
                          FDI_SET (format - 1, FMTDIR_ERROR);
                          goto bad_format;
                        }

                      arg_id = unnumbered_arg_count;
                      unnumbered_arg_count++;
                    }
                }

              c = *format;
              if (c == ':')
                {
                  format++;

                  /* Format specifiers is in the form:
                     [[fill]align][sign][#][0][minimumwidth][.precision][type]  */

                  /* Look ahead two characters to skip [[fill]align].  */
                  int c1, c2;

                  c1 = format[0];
                  if (c1 == '\0')
                    {
                      *invalid_reason =
                        xasprintf (_("The directive number %u is unterminated."),
                                   spec.directives);
                      FDI_SET (format - 1, FMTDIR_ERROR);
                      goto bad_format;
                    }

                  c2 = format[1];

                  if (c2 == '<' || c2 == '>' || c2 == '^')
                    format += 2;
                  else if (c1 == '<' || c1 == '>' || c1 == '^')
                    format++;

                  if (*format == '+' || *format == '-')
                    format++;
                  if (*format == '#')
                    format++;
                  if (*format == '0')
                    format++;

                  /* Parse the optional minimumwidth.  */
                  while (c_isdigit (*format))
                    format++;

                  /* Parse the optional .precision.  */
                  if (*format == '.')
                    {
                      format++;
                      if (c_isdigit (*format))
                        do
                          format++;
                        while (c_isdigit (*format));
                      else
                        format--;
                    }

                  /* Parse the optional type.  */
                  if (*format == '?')
                    format++;
                }

              /* Parse Unicode (UTF-8) character with property White_Space.  */
              while (*format != '}')
                {
                  ucs4_t uc;
                  int n = u8_mbtouc (&uc,
                                     (const uint8_t *) format,
                                     strnlen (format, 4));
                  if (n > 0 && uc_is_property_white_space (uc))
                    format += n;
                  else
                    break;
                }

              if (*format != '}')
                {
                  *invalid_reason =
                    xasprintf (_("The directive number %u is unterminated."),
                               spec.directives);
                  FDI_SET (format - 1, FMTDIR_ERROR);
                  goto bad_format;
                }

              spec.directives++;

              if (!seen_identifier_or_keyword)
                {
                  if (numbered_allocated == spec.numbered_arg_count)
                    {
                      numbered_allocated = 2 * numbered_allocated + 1;
                      spec.numbered = (struct numbered_arg *) xrealloc (spec.numbered, numbered_allocated * sizeof (struct numbered_arg));
                    }
                  spec.numbered[spec.numbered_arg_count].number = arg_id;
                  spec.numbered_arg_count++;
                }

              FDI_SET (format, FMTDIR_END);
            }
        }
      format++;
    }

  /* Sort the numbered argument array, and eliminate duplicates.  */
  if (spec.numbered_arg_count > 1)
    {
      unsigned int i, j;

      qsort (spec.numbered, spec.numbered_arg_count,
             sizeof (struct numbered_arg), numbered_arg_compare);

      /* Remove duplicates: Copy from i to j, keeping 0 <= j <= i.  */
      for (i = j = 0; i < spec.numbered_arg_count; i++)
        if (j > 0 && spec.numbered[i].number == spec.numbered[j-1].number)
          ;
        else
          {
            if (j < i)
              spec.numbered[j].number = spec.numbered[i].number;
            j++;
          }
      spec.numbered_arg_count = j;
    }

  /* Sort the named argument array, and eliminate duplicates.  */
  if (spec.named_arg_count > 1)
    {
      unsigned int i, j;

      qsort (spec.named, spec.named_arg_count, sizeof (struct named_arg),
             named_arg_compare);

      /* Remove duplicates: Copy from i to j, keeping 0 <= j <= i.  */
      for (i = j = 0; i < spec.named_arg_count; i++)
        if (j > 0 && strcmp (spec.named[i].name, spec.named[j-1].name) == 0)
          free (spec.named[i].name);
        else
          {
            if (j < i)
              spec.named[j].name = spec.named[i].name;
            j++;
          }
      spec.named_arg_count = j;
    }

  result = XMALLOC (struct spec);
  *result = spec;
  return result;

 bad_format:
  if (spec.named != NULL)
    {
      unsigned int i;
      for (i = 0; i < spec.named_arg_count; i++)
        free (spec.named[i].name);
      free (spec.named);
    }
  if (spec.numbered != NULL)
    free (spec.numbered);
  return NULL;
}

static void
format_free (void *descr)
{
  struct spec *spec = (struct spec *) descr;

  if (spec->named != NULL)
    {
      unsigned int i;
      for (i = 0; i < spec->named_arg_count; i++)
        free (spec->named[i].name);
      free (spec->named);
    }
  free (spec->numbered);
  free (spec);
}

static int
format_get_number_of_directives (void *descr)
{
  struct spec *spec = (struct spec *) descr;

  return spec->directives;
}

static bool
format_check (void *msgid_descr, void *msgstr_descr, bool equality,
              formatstring_error_logger_t error_logger, void *error_logger_data,
              const char *pretty_msgid, const char *pretty_msgstr)
{
  struct spec *spec1 = (struct spec *) msgid_descr;
  struct spec *spec2 = (struct spec *) msgstr_descr;
  bool err = false;

  if (spec1->named_arg_count + spec2->named_arg_count > 0)
    {
      unsigned int i, j;
      unsigned int n1 = spec1->named_arg_count;
      unsigned int n2 = spec2->named_arg_count;

      /* Check the argument names in spec2 are contained in those of spec1.
         Both arrays are sorted.  We search for the first difference.  */
      for (i = 0, j = 0; i < n1 || j < n2; )
        {
          int cmp = (i >= n1 ? 1 :
                     j >= n2 ? -1 :
                     strcmp (spec1->named[i].name, spec2->named[j].name));

          if (cmp > 0)
            {
              if (error_logger)
                error_logger (error_logger_data,
                              _("a format specification for argument '%s', as in '%s', doesn't exist in '%s'"),
                              spec2->named[j].name, pretty_msgstr,
                              pretty_msgid);
              err = true;
              break;
            }
          else if (cmp < 0)
            {
              if (equality)
                {
                  if (error_logger)
                    error_logger (error_logger_data,
                                  _("a format specification for argument '%s' doesn't exist in '%s'"),
                                  spec1->named[i].name, pretty_msgstr);
                  err = true;
                  break;
                }
              else
                i++;
            }
          else
            j++, i++;
        }
    }

  if (spec1->numbered_arg_count + spec2->numbered_arg_count > 0)
    {
      unsigned int i, j;
      unsigned int n1 = spec1->numbered_arg_count;
      unsigned int n2 = spec2->numbered_arg_count;
      unsigned int missing = 0; /* only used if !equality */

      /* Check that the argument numbers are the same.
         Both arrays are sorted.  We search for the first difference.  */
      for (i = 0, j = 0; i < n1 || j < n2; )
        {
          int cmp = (i >= n1 ? 1 :
                     j >= n2 ? -1 :
                     spec1->numbered[i].number > spec2->numbered[j].number ? 1 :
                     spec1->numbered[i].number < spec2->numbered[j].number ? -1 :
                     0);

          if (cmp > 0)
            {
              if (error_logger)
                error_logger (error_logger_data,
                              _("a format specification for argument %u, as in '%s', doesn't exist in '%s'"),
                              spec2->numbered[j].number, pretty_msgstr,
                              pretty_msgid);
              err = true;
              break;
            }
          else if (cmp < 0)
            {
              if (equality)
                {
                  if (error_logger)
                    error_logger (error_logger_data,
                                  _("a format specification for argument %u doesn't exist in '%s'"),
                                  spec1->numbered[i].number, pretty_msgstr);
                  err = true;
                  break;
                }
              else if (missing)
                {
                  if (error_logger)
                    error_logger (error_logger_data,
                                  _("a format specification for arguments %u and %u doesn't exist in '%s', only one argument may be ignored"),
                                  missing, spec1->numbered[i].number,
                                  pretty_msgstr);
                  err = true;
                  break;
                }
              else
                {
                  missing = spec1->numbered[i].number;
                  i++;
                }
            }
          else
            j++, i++;
        }
    }

  return err;
}


struct formatstring_parser formatstring_rust =
{
  format_parse,
  format_free,
  format_get_number_of_directives,
  NULL,
  format_check
};


#ifdef TEST

/* Test program: Print the argument list specification returned by
   format_parse for strings read from standard input.  */

#include <stdio.h>

static void
format_print (void *descr)
{
  struct spec *spec = (struct spec *) descr;
  unsigned int last;
  unsigned int i, j;

  if (spec == NULL)
    {
      printf ("INVALID");
      return;
    }

  printf ("(");
  last = 0;
  for (i = 0; i < spec->numbered_arg_count; i++)
    {
      unsigned int number = spec->numbered[i].number;

      if (i > 0)
        printf (" ");
      if (number < last)
        abort ();
      for (; last < number; last++)
        printf ("_ ");
      printf ("*");
      last = number + 1;
    }
  for (j = 0; j < spec->named_arg_count; j++)
    {
      if (i > 0 || j > 0)
        printf (" ");
      printf ("%s", spec->named[j].name);
    }
  printf (")");
}

int
main ()
{
  for (;;)
    {
      char *line = NULL;
      size_t line_size = 0;
      int line_len;
      char *invalid_reason;
      void *descr;

      line_len = getline (&line, &line_size, stdin);
      if (line_len < 0)
        break;
      if (line_len > 0 && line[line_len - 1] == '\n')
        line[--line_len] = '\0';

      invalid_reason = NULL;
      descr = format_parse (line, false, NULL, &invalid_reason);

      format_print (descr);
      printf ("\n");
      if (descr == NULL)
        printf ("%s\n", invalid_reason);

      free (invalid_reason);
      free (line);
    }

  return 0;
}

/*
 * For Emacs M-x compile
 * Local Variables:
 * compile-command: "/bin/sh ../libtool --tag=CC --mode=link gcc -o a.out -static -O -g -Wall -I.. -I../gnulib-lib -I../../gettext-runtime/intl -DHAVE_CONFIG_H -DTEST format-rust.c ../gnulib-lib/libgettextlib.la"
 * End:
 */

#endif /* TEST */
