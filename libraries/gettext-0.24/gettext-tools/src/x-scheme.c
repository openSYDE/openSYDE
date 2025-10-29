/* xgettext Scheme backend.
   Copyright (C) 2004-2024 Free Software Foundation, Inc.

   This file was written by Bruno Haible <bruno@clisp.org>, 2004-2005.

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
# include "config.h"
#endif

/* Specification.  */
#include "x-scheme.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include "attribute.h"
#include "message.h"
#include "xgettext.h"
#include "unicase.h"
#include "uninorm.h"
#include "xg-pos.h"
#include "xg-mixed-string.h"
#include "xg-arglist-context.h"
#include "xg-arglist-callshape.h"
#include "xg-arglist-parser.h"
#include "xg-message.h"
#include "if-error.h"
#include "xalloc.h"
#include "mem-hash-map.h"
#include "gettext.h"

#define _(s) gettext(s)

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))


/* The Scheme syntax is described in R5RS and following standards:
     - R5RS: https://conservatory.scheme.org/schemers/Documents/Standards/R5RS/HTML/
     - R6RS: https://www.r6rs.org/
     - R7RS: https://standards.scheme.org/corrected-r7rs/r7rs.html

   It is implemented in guile-3.0.10/module/ice-9/read.scm, with support of
   #!r6rs.  What you see in guile-3.0.10/libguile/read.c is just the bootstrap
   reader, without support of #!r6rs and similar directives.

   Since we are interested only in strings and in forms similar to
        (gettext msgid ...)
   or   (ngettext msgid msgid_plural ...)
   we make the following simplifications:

   - Assume the keywords and strings are in an ASCII compatible encoding.
     This means we can read the input file one byte at a time, instead of
     one character at a time.  No need to worry about multibyte characters:
     If they occur as part of identifiers, they most probably act as
     constituent characters, and the byte based approach will do the same.

   - Assume the read-hash-procedures is in the default state.
     Non-standard reader extensions are mostly used to read data, not programs.

   The remaining syntax rules are:

   - The syntax code assigned to each character, and how tokens are built
     up from characters (single escape, multiple escape etc.).

   - Directives:
       #!r6rs          (see R6RS § 4) turns R6RS compliance on
       #!fold-case     (see R7RS § 2.1) turns case-folding of identifiers on
       #!no-fold-case  (see R7RS § 2.1) turns case-folding of identifiers off
       #!curly-infix                    (guile specific)
       #!curly-infix-and-bracket-lists  (guile specific)

   - Comment syntax:
       ';' up to end of line
       '#;' <datum> (see R6RS § 4.2.3, R7RS § 2.2)
       '#! ... !#'
       '#| ... |#' (may be nested)

   - String syntax: "..." with single escapes.

   - Read macros and dispatch macro character '#'.  Needed to be able to
     tell which is the n-th argument of a function call.

 */


/* ====================== Keyword set customization.  ====================== */

/* If true extract all strings.  */
static bool extract_all = false;

static hash_table keywords;
static bool default_keywords = true;


void
x_scheme_extract_all ()
{
  extract_all = true;
}


void
x_scheme_keyword (const char *name)
{
  if (name == NULL)
    default_keywords = false;
  else
    {
      const char *end;
      struct callshape shape;
      const char *colon;

      if (keywords.table == NULL)
        hash_init (&keywords, 100);

      split_keywordspec (name, &end, &shape);

      /* The characters between name and end should form a valid Lisp symbol.
         Extract the symbol name part.  */
      colon = strchr (name, ':');
      if (colon != NULL && colon < end)
        {
          name = colon + 1;
          if (name < end && *name == ':')
            name++;
          colon = strchr (name, ':');
          if (colon != NULL && colon < end)
            return;
        }

      insert_keyword_callshape (&keywords, name, end - name, &shape);
    }
}

/* Finish initializing the keywords hash table.
   Called after argument processing, before each file is processed.  */
static void
init_keywords ()
{
  if (default_keywords)
    {
      /* When adding new keywords here, also update the documentation in
         xgettext.texi!  */
      x_scheme_keyword ("gettext");             /* libguile/i18n.c */
      x_scheme_keyword ("ngettext:1,2");        /* libguile/i18n.c */
      x_scheme_keyword ("gettext-noop");
      default_keywords = false;
    }
}

void
init_flag_table_scheme ()
{
  xgettext_record_flag ("gettext:1:pass-scheme-format");
  xgettext_record_flag ("ngettext:1:pass-scheme-format");
  xgettext_record_flag ("ngettext:2:pass-scheme-format");
  xgettext_record_flag ("gettext-noop:1:pass-scheme-format");
  xgettext_record_flag ("format:2:scheme-format");
}


/* ======================== Reading of characters.  ======================== */

/* The input file stream.  */
static FILE *fp;


/* Fetch the next character from the input file.  */
static int
phase0_getc ()
{
  int c = getc (fp);

  if (c == EOF)
    {
      if (ferror (fp))
        error (EXIT_FAILURE, errno,
               _("error while reading \"%s\""), real_file_name);
    }

  return c;
}

/* Put back the last fetched character, not EOF.  */
MAYBE_UNUSED static void
phase0_ungetc (int c)
{
  ungetc (c, fp);
}


/* 1. line_number handling.  */

/* Maximum used.
   Must be larger than the longest possible directive.  */
#define MAX_PHASE1_PUSHBACK 32
static unsigned char phase1_pushback[MAX_PHASE1_PUSHBACK];
static int phase1_pushback_length;

/* Read the next single character from the input file.  */
static int
phase1_getc ()
{
  int c;

  if (phase1_pushback_length)
    c = phase1_pushback[--phase1_pushback_length];
  else
    c = phase0_getc ();

  if (c == '\n')
    ++line_number;

  return c;
}

/* Supports MAX_PHASE1_PUSHBACK characters of pushback.  */
static void
phase1_ungetc (int c)
{
  if (c != EOF)
    {
      if (c == '\n')
        --line_number;

      if (phase1_pushback_length == SIZEOF (phase1_pushback))
        abort ();
      phase1_pushback[phase1_pushback_length++] = c;
    }
}


/* ========================== Reading of tokens.  ========================== */


/* True to follow what Guile does (before a '#!r6rs' directive is seen).
   False to follow R6RS and R7RS.  */
static bool follow_guile;

/* True if all read identifiers are to be casefolded, i.e. essentially mapped
   to lower case.  */
static bool casefold;

/* A token consists of a sequence of characters.  */
struct token
{
  int allocated;                /* number of allocated 'token_char's */
  int charcount;                /* number of used 'token_char's */
  char *chars;                  /* the token's constituents */
};

/* Initialize a 'struct token'.  */
static inline void
init_token (struct token *tp)
{
  tp->allocated = 10;
  tp->chars = XNMALLOC (tp->allocated, char);
  tp->charcount = 0;
}

/* Free the memory pointed to by a 'struct token'.  */
static inline void
free_token (struct token *tp)
{
  free (tp->chars);
}

/* Ensure there is enough room in the token for one more character.  */
static inline void
grow_token (struct token *tp)
{
  if (tp->charcount == tp->allocated)
    {
      tp->allocated *= 2;
      tp->chars = (char *) xrealloc (tp->chars, tp->allocated * sizeof (char));
    }
}

/* Read the next token.  'first' is the first character, which has already
   been read.  */
static void
read_token (struct token *tp, int first)
{
  init_token (tp);

  grow_token (tp);
  tp->chars[tp->charcount++] = first;

  for (;;)
    {
      int c = phase1_getc ();

      if (c == EOF)
        break;
      if (c == ' ' || c == '\r' || c == '\f' || c == '\t' || c == '\n'
          || c == '"' || c == '(' || c == ')' || c == ';')
        {
          phase1_ungetc (c);
          break;
        }
      grow_token (tp);
      tp->chars[tp->charcount++] = c;
    }
}

/* Tests if a token represents an integer.
   Taken from guile-1.6.4/libguile/numbers.c:scm_istr2int().  */
static inline bool
is_integer_syntax (const char *str, int len, int radix)
{
  const char *p = str;
  const char *p_end = str + len;

  /* The accepted syntax is
       ['+'|'-'] DIGIT+
     where DIGIT is a hexadecimal digit whose value is below radix.  */

  if (p == p_end)
    return false;
  if (*p == '+' || *p == '-')
    {
      p++;
      if (p == p_end)
        return false;
    }
  do
    {
      int c = *p++;

      if (c >= '0' && c <= '9')
        c = c - '0';
      else if (c >= 'A' && c <= 'F')
        c = c - 'A' + 10;
      else if (c >= 'a' && c <= 'f')
        c = c - 'a' + 10;
      else
        return false;
      if (c >= radix)
        return false;
    }
  while (p < p_end);
  return true;
}

/* Tests if a token represents a rational, floating-point or complex number.
   If unconstrained is false, only real numbers are accepted; otherwise,
   complex numbers are accepted as well.
   Taken from guile-1.6.4/libguile/numbers.c:scm_istr2flo().  */
static bool
is_other_number_syntax (const char *str, int len, int radix, bool unconstrained)
{
  const char *p = str;
  const char *p_end = str + len;
  bool seen_sign;
  bool seen_digits;

  /* The accepted syntaxes are:
     for a floating-point number:
       ['+'|'-'] DIGIT+ [EXPONENT]
       ['+'|'-'] DIGIT* '.' DIGIT+ [EXPONENT]
       where EXPONENT ::= ['d'|'e'|'f'|'l'|'s'] DIGIT+
       (Dot and exponent are allowed only if radix is 10.)
     for a rational number:
       ['+'|'-'] DIGIT+ '/' DIGIT+
     for a complex number:
       REAL-NUMBER {'+'|'-'} REAL-NUMBER-WITHOUT-SIGN 'i'
       REAL-NUMBER {'+'|'-'} 'i'
       {'+'|'-'} REAL-NUMBER-WITHOUT-SIGN 'i'
       {'+'|'-'} 'i'
       REAL-NUMBER '@' REAL-NUMBER
   */
  if (p == p_end)
    return false;
  /* Parse leading sign.  */
  seen_sign = false;
  if (*p == '+' || *p == '-')
    {
      p++;
      if (p == p_end)
        return false;
      seen_sign = true;
      /* Recognize complex number syntax: {'+'|'-'} 'i'  */
      if (unconstrained && (*p == 'I' || *p == 'i') && p + 1 == p_end)
        return true;
    }
  /* Parse digits before dot or exponent or slash.  */
  seen_digits = false;
  do
    {
      int c = *p;

      if (c >= '0' && c <= '9')
        c = c - '0';
      else if (c >= 'A' && c <= 'F')
        {
          if (c >= 'D' && radix == 10) /* exponent? */
            break;
          c = c - 'A' + 10;
        }
      else if (c >= 'a' && c <= 'f')
        {
          if (c >= 'd' && radix == 10) /* exponent? */
            break;
          c = c - 'a' + 10;
        }
      else
        break;
      if (c >= radix)
        return false;
      seen_digits = true;
      p++;
    }
  while (p < p_end);
  /* If p == p_end, we know that seen_digits = true, and the number is an
     integer without exponent.  */
  if (p < p_end)
    {
      /* If we have no digits so far, we need a decimal point later.  */
      if (!seen_digits && !(*p == '.' && radix == 10))
        return false;
      /* Trailing '#' signs are equivalent to zeroes.  */
      while (p < p_end && *p == '#')
        p++;
      if (p < p_end)
        {
          if (*p == '/')
            {
              /* Parse digits after the slash.  */
              bool all_zeroes = true;
              p++;
              for (; p < p_end; p++)
                {
                  int c = *p;

                  if (c >= '0' && c <= '9')
                    c = c - '0';
                  else if (c >= 'A' && c <= 'F')
                    c = c - 'A' + 10;
                  else if (c >= 'a' && c <= 'f')
                    c = c - 'a' + 10;
                  else
                    break;
                  if (c >= radix)
                    return false;
                  if (c != 0)
                    all_zeroes = false;
                }
              /* A zero denominator is not allowed.  */
              if (all_zeroes)
                return false;
              /* Trailing '#' signs are equivalent to zeroes.  */
              while (p < p_end && *p == '#')
                p++;
            }
          else
            {
              if (*p == '.')
                {
                  /* Decimal point notation.  */
                  if (radix != 10)
                    return false;
                  /* Parse digits after the decimal point.  */
                  p++;
                  for (; p < p_end; p++)
                    {
                      int c = *p;

                      if (c >= '0' && c <= '9')
                        seen_digits = true;
                      else
                        break;
                    }
                  /* Digits are required before or after the decimal point.  */
                  if (!seen_digits)
                    return false;
                  /* Trailing '#' signs are equivalent to zeroes.  */
                  while (p < p_end && *p == '#')
                    p++;
                }
              if (p < p_end)
                {
                  /* Parse exponent.  */
                  switch (*p)
                    {
                    case 'D': case 'd':
                    case 'E': case 'e':
                    case 'F': case 'f':
                    case 'L': case 'l':
                    case 'S': case 's':
                      if (radix != 10)
                        return false;
                      p++;
                      if (p == p_end)
                        return false;
                      if (*p == '+' || *p == '-')
                        {
                          p++;
                          if (p == p_end)
                            return false;
                        }
                      if (!(*p >= '0' && *p <= '9'))
                        return false;
                      for (;;)
                        {
                          p++;
                          if (p == p_end)
                            break;
                          if (!(*p >= '0' && *p <= '9'))
                            break;
                        }
                      break;
                    default:
                      break;
                    }
                }
            }
        }
    }
  if (p == p_end)
    return true;
  /* Recognize complex number syntax.  */
  if (unconstrained)
    {
      /* Recognize the syntax  {'+'|'-'} REAL-NUMBER-WITHOUT-SIGN 'i'  */
      if (seen_sign && (*p == 'I' || *p == 'i') && p + 1 == p_end)
        return true;
      /* Recognize the syntaxes
           REAL-NUMBER {'+'|'-'} REAL-NUMBER-WITHOUT-SIGN 'i'
           REAL-NUMBER {'+'|'-'} 'i'
       */
      if (*p == '+' || *p == '-')
        return (p_end[-1] == 'I' || p_end[-1] == 'i')
                && (p + 1 == p_end - 1
                    || is_other_number_syntax (p, p_end - 1 - p, radix, false));
      /* Recognize the syntax  REAL-NUMBER '@' REAL-NUMBER  */
      if (*p == '@')
        {
          p++;
          return is_other_number_syntax (p, p_end - p, radix, false);
        }
    }
  return false;
}

/* Tests if a token represents a number.
   Taken from guile-1.6.4/libguile/numbers.c:scm_istring2number().  */
static bool
is_number (const struct token *tp)
{
  const char *str = tp->chars;
  int len = tp->charcount;
  enum { unknown, exact, inexact } exactness = unknown;
  bool seen_radix_prefix = false;
  bool seen_exactness_prefix = false;

  if (len == 1)
    if (*str == '+' || *str == '-')
      return false;
  while (len >= 2 && *str == '#')
    {
      switch (str[1])
        {
        case 'B': case 'b':
          if (seen_radix_prefix)
            return false;
          seen_radix_prefix = true;
          break;
        case 'O': case 'o':
          if (seen_radix_prefix)
            return false;
          seen_radix_prefix = true;
          break;
        case 'D': case 'd':
          if (seen_radix_prefix)
            return false;
          seen_radix_prefix = true;
          break;
        case 'X': case 'x':
          if (seen_radix_prefix)
            return false;
          seen_radix_prefix = true;
          break;
        case 'E': case 'e':
          if (seen_exactness_prefix)
            return false;
          exactness = exact;
          seen_exactness_prefix = true;
          break;
        case 'I': case 'i':
          if (seen_exactness_prefix)
            return false;
          exactness = inexact;
          seen_exactness_prefix = true;
          break;
        default:
          return false;
        }
      str += 2;
      len -= 2;
    }
  if (exactness != inexact)
    {
      /* Try to parse an integer.  */
      if (is_integer_syntax (str, len, 10))
        return true;
      /* FIXME: Other Scheme implementations support exact rational numbers
         or exact complex numbers.  */
    }
  if (exactness != exact)
    {
      /* Try to parse a rational, floating-point or complex number.  */
      if (is_other_number_syntax (str, len, 10, true))
        return true;
    }
  return false;
}

/* Read a string literal.  */
static void
accumulate_escaped (struct mixed_string_buffer *literal)
{
  for (;;)
    {
      int c = phase1_getc ();
      if (c == EOF)
        /* Invalid input.  Be tolerant, no error message.  */
        break;
      if (c == '"')
        break;
      if (c == '\\')
        {
          c = phase1_getc ();
          if (c == EOF)
            /* Invalid input.  Be tolerant, no error message.  */
            break;
          if (c == ' ' || c == '\t')
            {
              if (follow_guile)
               /* Invalid input.  Be tolerant, no error message.  */
                ;
              else
                {
                  /* In R6RS mode, a sequence of spaces and tabs is
                     allowed between the backslash and the newline.
                     Other than that, backslash-space and backslash-tab
                     are not allowed.  See R6RS § 4.2.7, R7RS § 6.7.  */
                  do
                    c = phase1_getc ();
                  while (c == ' ' || c == '\t');
                  if (c == EOF)
                    /* Invalid input.  Be tolerant, no error message.  */
                    break;
                  if (c != '\n')
                    {
                      /* Invalid input.  Be tolerant, no error message.  */
                      phase1_ungetc (c);
                      continue;
                    }
                }
            }
          if (c == '\n')
            {
              if (!follow_guile)
                {
                  /* In R6RS mode, a sequence of spaces and tabs is
                     allowed after the newline and is discarded.
                     See R6RS § 4.2.7, R7RS § 6.7.  */
                  do
                    c = phase1_getc ();
                  while (c == ' ' || c == '\t');
                  if (c == EOF)
                    /* Invalid input.  Be tolerant, no error message.  */
                    break;
                  phase1_ungetc (c);
                }
              continue;
            }
          if (follow_guile && (c == 'x' || c == 'u' || c == 'U'))
            {
              /* In Guile, \x must be followed by exactly 2 hexadecimal digits,
                 \u must be followed by exactly 4 hexadecimal digits, and
                 \U must be followed by exactly 6 hexadecimal digits, producing
                 a value < 0x110000.  See
                 <https://www.gnu.org/software/guile/manual/html_node/String-Syntax.html>.  */
              int first = c;
              unsigned int count = (c == 'x' ? 2 : c == 'u' ? 4 : 6);
              unsigned int n_limit =
                (c == 'x' ? 0x100 : c == 'u' ? 0x10000 : 0x110000);
              c = phase1_getc ();
              switch (c)
                {
                default:
                  phase1_ungetc (c);
                  phase1_ungetc (first);
                  /* Invalid input.  Be tolerant, no error message.  */
                  mixed_string_buffer_append_char (literal, '\\');
                  continue;

                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                  break;
                }
              {
                unsigned int n;
                bool overflow;

                n = 0;
                overflow = false;

                for (;;)
                  {
                    switch (c)
                      {
                      default:
                        phase1_ungetc (c);
                        goto guile_hex_escape_done;

                      case '0': case '1': case '2': case '3': case '4':
                      case '5': case '6': case '7': case '8': case '9':
                        if (n < n_limit / 16)
                          n = n * 16 + c - '0';
                        else
                          overflow = true;
                        break;

                      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                        if (n < n_limit / 16)
                          n = n * 16 + 10 + c - 'A';
                        else
                          overflow = true;
                        break;

                      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                        if (n < n_limit / 16)
                          n = n * 16 + 10 + c - 'a';
                        else
                          overflow = true;
                        break;
                      }
                    if (--count == 0)
                      goto guile_hex_escape_done;
                    c = phase1_getc ();
                  }

               guile_hex_escape_done:
                if (count > 0)
                  {
                    if_error (IF_SEVERITY_WARNING,
                              logical_file_name, line_number, (size_t)(-1), false,
                              _("hexadecimal escape sequence with too few digits"));
                    n = 0xFFFD;
                  }
                else if (overflow)
                  {
                    if_error (IF_SEVERITY_WARNING,
                              logical_file_name, line_number, (size_t)(-1), false,
                              _("hexadecimal escape sequence out of range"));
                    n = 0xFFFD;
                  }
                mixed_string_buffer_append_unicode (literal, n);
              }
              continue;
            }
          if (!follow_guile && c == 'x')
            {
              /* In R6RS mode, \x must be followed by one or more hexadecimal
                 digits and then a semicolon.  See
                 R6RS § 4.2.7, R7RS § 6.7 and § 7.1.1.  */
              unsigned int const n_limit = 0x110000;
              unsigned int count;
              unsigned int n;
              bool overflow;

              count = 0;
              n = 0;
              overflow = false;

              for (;;)
                {
                  c = phase1_getc ();
                  switch (c)
                    {
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                      if (n < n_limit / 16)
                        n = n * 16 + c - '0';
                      else
                        overflow = true;
                      break;

                    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                      if (n < n_limit / 16)
                        n = n * 16 + 10 + c - 'A';
                      else
                        overflow = true;
                      break;

                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                      if (n < n_limit / 16)
                        n = n * 16 + 10 + c - 'a';
                      else
                        overflow = true;
                      break;

                    case ';':
                      if (count == 0)
                        {
                          if_error (IF_SEVERITY_WARNING,
                                    logical_file_name, line_number, (size_t)(-1), false,
                                    _("hexadecimal escape sequence with no digits"));
                          n = 0xFFFD;
                        }
                      else if (overflow)
                        {
                          if_error (IF_SEVERITY_WARNING,
                                    logical_file_name, line_number, (size_t)(-1), false,
                                    _("hexadecimal escape sequence out of range"));
                          n = 0xFFFD;
                        }
                      goto r6rs_hex_escape_done;

                    default:
                      if_error (IF_SEVERITY_WARNING,
                                logical_file_name, line_number, (size_t)(-1), false,
                                _("hexadecimal escape sequence not terminated with a semicolon"));
                      n = '\\';
                      goto r6rs_hex_escape_done;
                    }
                  count++;
                }

             r6rs_hex_escape_done:
              mixed_string_buffer_append_unicode (literal, n);
              continue;
            }
          switch (c)
            {
            case '0':
              c = '\0';
              break;
            case 'a':
              c = '\a';
              break;
            case 'f':
              c = '\f';
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'v':
              c = '\v';
              break;
            default:
              break;
            }
        }
      mixed_string_buffer_append_char (literal, c);
    }
}


/* ========================= Accumulating comments ========================= */


static char *buffer;
static size_t bufmax;
static size_t buflen;

static inline void
comment_start ()
{
  buflen = 0;
}

static inline void
comment_add (int c)
{
  if (buflen >= bufmax)
    {
      bufmax = 2 * bufmax + 10;
      buffer = xrealloc (buffer, bufmax);
    }
  buffer[buflen++] = c;
}

static inline void
comment_line_end (size_t chars_to_remove)
{
  buflen -= chars_to_remove;
  while (buflen >= 1
         && (buffer[buflen - 1] == ' ' || buffer[buflen - 1] == '\t'))
    --buflen;
  if (chars_to_remove == 0 && buflen >= bufmax)
    {
      bufmax = 2 * bufmax + 10;
      buffer = xrealloc (buffer, bufmax);
    }
  buffer[buflen] = '\0';
  savable_comment_add (buffer);
}


/* These are for tracking whether comments count as immediately before
   keyword.  */
static int last_comment_line;
static int last_non_comment_line;


/* ========================= Accumulating messages ========================= */


static message_list_ty *mlp;


/* ========================== Reading of objects.  ========================= */


/* We are only interested in symbols (e.g. gettext or ngettext) and strings.
   Other objects need not to be represented precisely.  */
enum object_type
{
  t_symbol,     /* symbol */
  t_string,     /* string */
  t_other,      /* other kind of real object */
  t_dot,        /* '.' pseudo object */
  t_close,      /* ')' pseudo object */
  t_eof         /* EOF marker */
};

struct object
{
  enum object_type type;
  struct token *token;           /* for t_symbol */
  mixed_string_ty *mixed_string; /* for t_string */
  int line_number_at_start;      /* for t_string */
};

/* Free the memory pointed to by a 'struct object'.  */
static inline void
free_object (struct object *op)
{
  if (op->type == t_symbol)
    {
      free_token (op->token);
      free (op->token);
    }
  else if (op->type == t_string)
    mixed_string_free (op->mixed_string);
}

/* Convert a t_symbol/t_string token to a char*.  */
static char *
string_of_object (const struct object *op)
{
  char *str;

  if (op->type == t_symbol)
    {
      int n = op->token->charcount;
      str = XNMALLOC (n + 1, char);
      memcpy (str, op->token->chars, n);
      str[n] = '\0';
      /* In this case, str's encoding is the xgettext_current_source_encoding.  */
    }
  else if (op->type == t_string)
    {
      str = mixed_string_contents (op->mixed_string);
      /* In this case, str is UTF-8 encoded.  */
    }
  else
    abort ();
  return str;
}


/* Context lookup table.  */
static flag_context_list_table_ty *flag_context_list_table;


/* Maximum supported nesting depth.  */
#define MAX_NESTING_DEPTH 1000

/* Current nesting depth.  */
static int nesting_depth;

/* Current nesting depth of #;<datum> comments.  */
static int datum_comment_nesting_depth;


/* Read the next object.  */
static void
read_object (struct object *op, flag_region_ty *outer_region)
{
  if (nesting_depth > MAX_NESTING_DEPTH)
    if_error (IF_SEVERITY_FATAL_ERROR,
              logical_file_name, line_number, (size_t)(-1), false,
              _("too deeply nested objects"));
  for (;;)
    {
      int ch = phase1_getc ();
      bool seen_underscore_prefix = false;

      switch (ch)
        {
        case EOF:
          op->type = t_eof;
          return;

        case ' ': case '\r': case '\f': case '\t':
          continue;

        case '\n':
          /* Comments assumed to be grouped with a message must immediately
             precede it, with no non-whitespace token on a line between
             both.  */
          if (last_non_comment_line > last_comment_line)
            savable_comment_reset ();
          continue;

        case ';':
          {
            bool all_semicolons = true;

            last_comment_line = line_number;
            comment_start ();
            for (;;)
              {
                int c = phase1_getc ();
                if (c == EOF || c == '\n')
                  break;
                if (c != ';')
                  all_semicolons = false;
                if (!all_semicolons)
                  {
                    /* We skip all leading white space, but not EOLs.  */
                    if (!(buflen == 0 && (c == ' ' || c == '\t')))
                      comment_add (c);
                  }
              }
            comment_line_end (0);
            continue;
          }

        case '(':
          {
            int arg = 0;                /* Current argument number.  */
            flag_context_list_iterator_ty context_iter;
            const struct callshapes *shapes = NULL;
            struct arglist_parser *argparser = NULL;

            for (;; arg++)
              {
                struct object inner;
                flag_region_ty *inner_region;

                if (arg == 0)
                  inner_region = null_context_region ();
                else
                  inner_region =
                    inheriting_region (outer_region,
                                       flag_context_list_iterator_advance (
                                         &context_iter));

                ++nesting_depth;
                read_object (&inner, inner_region);
                nesting_depth--;

                /* Recognize end of list.  */
                if (inner.type == t_close)
                  {
                    op->type = t_other;
                    last_non_comment_line = line_number;
                    if (argparser != NULL)
                      arglist_parser_done (argparser, arg);
                    unref_region (inner_region);
                    return;
                  }

                /* Dots are not allowed in every position.
                   But be tolerant.  */

                /* EOF inside list is illegal.
                   But be tolerant.  */
                if (inner.type == t_eof)
                  break;

                if (arg == 0)
                  {
                    /* This is the function position.  */
                    if (datum_comment_nesting_depth == 0
                        && inner.type == t_symbol)
                      {
                        char *symbol_name = string_of_object (&inner);
                        if (casefold)
                          {
                            char *symbol_name_converted =
                              from_current_source_encoding (symbol_name,
                                                            lc_outside,
                                                            logical_file_name,
                                                            line_number);
                            size_t symbol_name_casefolded_len;
                            char *symbol_name_casefolded =
                              (char *)
                              u8_casefold ((uint8_t *) symbol_name_converted,
                                           strlen (symbol_name_converted) + 1,
                                           NULL, UNINORM_NFC,
                                           NULL, &symbol_name_casefolded_len);
                            if (symbol_name_converted != symbol_name)
                              free (symbol_name_converted);
                            if (symbol_name_casefolded != NULL)
                              {
                                free (symbol_name);
                                symbol_name = symbol_name_casefolded;
                              }
                          }

                        void *keyword_value;

                        if (hash_find_entry (&keywords,
                                             symbol_name, strlen (symbol_name),
                                             &keyword_value)
                            == 0)
                          shapes = (const struct callshapes *) keyword_value;

                        argparser = arglist_parser_alloc (mlp, shapes);

                        context_iter =
                          flag_context_list_iterator (
                            flag_context_list_table_lookup (
                              flag_context_list_table,
                              symbol_name, strlen (symbol_name)));

                        free (symbol_name);
                      }
                    else
                      context_iter = null_context_list_iterator;
                  }
                else
                  {
                    /* These are the argument positions.  */
                    if (datum_comment_nesting_depth == 0
                        && argparser != NULL && inner.type == t_string)
                      {
                        char *s = string_of_object (&inner);
                        mixed_string_ty *ms =
                          mixed_string_alloc_utf8 (s, lc_string,
                                                   logical_file_name,
                                                   inner.line_number_at_start);
                        free (s);
                        arglist_parser_remember (argparser, arg, ms,
                                                 inner_region,
                                                 logical_file_name,
                                                 inner.line_number_at_start,
                                                 savable_comment, false);
                      }
                  }

                unref_region (inner_region);
                free_object (&inner);
              }
            if (argparser != NULL)
              arglist_parser_done (argparser, arg);
          }
          op->type = t_other;
          last_non_comment_line = line_number;
          return;

        case ')':
          /* Tell the caller about the end of list.
             Unmatched closing parenthesis is illegal.
             But be tolerant.  */
          op->type = t_close;
          last_non_comment_line = line_number;
          return;

        case ',':
          {
            int c = phase1_getc ();
            /* The ,@ handling inside lists is wrong anyway, because
               ,@form expands to an unknown number of elements.  */
            if (c != EOF && c != '@')
              phase1_ungetc (c);
          }
          FALLTHROUGH;
        case '\'':
        case '`':
          {
            struct object inner;

            ++nesting_depth;
            read_object (&inner, null_context_region ());
            nesting_depth--;

            /* Dots and EOF are not allowed here.  But be tolerant.  */

            free_object (&inner);

            op->type = t_other;
            last_non_comment_line = line_number;
            return;
          }

        case '#':
          /* Dispatch macro handling.  */
          {
            int dmc = phase1_getc ();
            if (dmc == EOF)
              /* Invalid input.  Be tolerant, no error message.  */
              {
                op->type = t_other;
                return;
              }

            switch (dmc)
              {
              case '(': /* Vector */
                phase1_ungetc (dmc);
                {
                  struct object inner;
                  ++nesting_depth;
                  read_object (&inner, null_context_region ());
                  nesting_depth--;
                  /* Dots and EOF are not allowed here.
                     But be tolerant.  */
                  free_object (&inner);
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              case 'T': case 't': /* Boolean true */
              case 'F': /* Boolean false */
                op->type = t_other;
                last_non_comment_line = line_number;
                return;

              case 'a':
              case 'c':
              case 'f':
              case 'h':
              case 'l':
              case 's':
              case 'u':
              case 'v':
              case 'y':
                {
                  struct token token;
                  phase1_ungetc (dmc);
                  read_token (&token, '#');
                  if ((token.charcount == 2
                       && (token.chars[1] == 'a' || token.chars[1] == 'c'
                           || token.chars[1] == 'h' || token.chars[1] == 'l'
                           || token.chars[1] == 's' || token.chars[1] == 'u'
                           || token.chars[1] == 'y'))
                      || (token.charcount == 3
                          && (token.chars[1] == 's' || token.chars[1] == 'u')
                          && token.chars[2] == '8')
                      || (token.charcount == 4
                          && (((token.chars[1] == 's' || token.chars[1] == 'u')
                               && token.chars[2] == '1'
                               && token.chars[3] == '6')
                              || ((token.chars[1] == 'c'
                                   || token.chars[1] == 'f'
                                   || token.chars[1] == 's'
                                   || token.chars[1] == 'u')
                                  && ((token.chars[2] == '3'
                                       && token.chars[3] == '2')
                                      || (token.chars[2] == '6'
                                          && token.chars[3] == '4')))
                              || (token.chars[1] == 'v'
                                  && token.chars[2] == 'u'
                                  && token.chars[3] == '8'))))
                    {
                      int c = phase1_getc ();
                      if (c != EOF)
                        phase1_ungetc (c);
                      if (c == '(')
                        {
                          /* Homogenous vector syntax:
                               #a(...) - vector of char
                               #c(...) - vector of complex (old)
                               #c32(...) - vector of complex of single-float
                               #c64(...) - vector of complex of double-float
                               #f32(...) - vector of single-float
                               #f64(...) - vector of double-float
                               #h(...) - vector of short (old)
                               #l(...) - vector of long long (old)
                               #s(...) - vector of single-float (old)
                               #s8(...) - vector of signed 8-bit integers
                               #s16(...) - vector of signed 16-bit integers
                               #s32(...) - vector of signed 32-bit integers
                               #s64(...) - vector of signed 64-bit integers
                               #u(...) - vector of unsigned long (old)
                               #u8(...) - vector of unsigned 8-bit integers
                               #u16(...) - vector of unsigned 16-bit integers
                               #u32(...) - vector of unsigned 32-bit integers
                               #u64(...) - vector of unsigned 64-bit integers
                               #vu8(...) - vector of byte
                               #y(...) - vector of byte (old)
                           */
                          struct object inner;
                          ++nesting_depth;
                          read_object (&inner, null_context_region ());
                          nesting_depth--;
                          /* Dots and EOF are not allowed here.
                             But be tolerant.  */
                          free_token (&token);
                          free_object (&inner);
                          op->type = t_other;
                          last_non_comment_line = line_number;
                          return;
                        }
                    }
                  /* Boolean false, or unknown # object.  But be tolerant.  */
                  free_token (&token);
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              case 'B': case 'b':
              case 'O': case 'o':
              case 'D': case 'd':
              case 'X': case 'x':
              case 'E': case 'e':
              case 'I': case 'i':
                {
                  struct token token;
                  phase1_ungetc (dmc);
                  read_token (&token, '#');
                  if (is_number (&token))
                    {
                      /* A number.  */
                      free_token (&token);
                      op->type = t_other;
                      last_non_comment_line = line_number;
                      return;
                    }
                  else
                    {
                      if (token.charcount == 2
                          && (token.chars[1] == 'e' || token.chars[1] == 'i'))
                        {
                          int c = phase1_getc ();
                          if (c != EOF)
                            phase1_ungetc (c);
                          if (c == '(')
                            {
                              /* Homogenous vector syntax:
                                   #e(...) - vector of long (old)
                                   #i(...) - vector of double-float (old)
                               */
                              struct object inner;
                              ++nesting_depth;
                              read_object (&inner, null_context_region ());
                              nesting_depth--;
                              /* Dots and EOF are not allowed here.
                                 But be tolerant.  */
                              free_token (&token);
                              free_object (&inner);
                              op->type = t_other;
                              last_non_comment_line = line_number;
                              return;
                            }
                        }
                      /* Unknown # object.  But be tolerant.  */
                      free_token (&token);
                      op->type = t_other;
                      last_non_comment_line = line_number;
                      return;
                    }
                }

              case ';':
                /* Datum comment '#; <datum>'.
                   See R6RS § 4.2.3, R7RS § 2.2.  */
                {
                  struct object inner;
                  int saved_last_non_comment_line = last_non_comment_line;
                  ++datum_comment_nesting_depth;
                  ++nesting_depth;
                  read_object (&inner, null_context_region ());
                  nesting_depth--;
                  datum_comment_nesting_depth--;
                  last_non_comment_line = saved_last_non_comment_line;
                  /* Dots and EOF are not allowed here.
                     But be tolerant.  */
                  free_object (&inner);
                  last_comment_line = line_number;
                  continue;
                }

              case '!':
                /* Directive or block comment.  */
                {
                  const char * const directives[] =
                    {
                      "r6rs",
                      "fold-case",
                      "no-fold-case",
                      "curly-infix",
                      "curly-infix-and-bracket-lists"
                    };
                  int num_directives = SIZEOF (directives);
                  enum { max_directive_len = 29 };
                  bool seen_directive = false;
                  int d;
                  for (d = 0; d < num_directives; d++)
                    {
                      const char *directive = directives[d];
                      int directive_len = strlen (directive);
                      int c[max_directive_len];
                      int i;
                      for (i = 0; i < directive_len; i++)
                        {
                          c[i] = phase1_getc ();
                          if (c[i] != directive[i])
                            {
                              phase1_ungetc (c[i]);
                              break;
                            }
                        }
                      if (i == directive_len)
                        {
                          int e = phase1_getc ();
                          /* Like in read_token.  */
                          if (e == ' '
                              || e == '\r' || e == '\f' || e == '\t' || e == '\n'
                              || e == '"' || e == '(' || e == ')' || e == ';')
                            {
                              /* Seen the directive.  */
                              phase1_ungetc (e);
                              seen_directive = true;
                              switch (d)
                                {
                                case 0: /* #!r6rs */
                                  follow_guile = false;
                                  break;
                                case 1: /* #!fold-case */
                                  casefold = true;
                                  break;
                                case 2: /* #!no-fold-case */
                                  casefold = false;
                                  break;
                                case 3: /* #!curly-infix */
                                case 4: /* #!curly-infix-and-bracket-lists */
                                  if_error (IF_SEVERITY_WARNING,
                                            logical_file_name, line_number, (size_t)(-1),
                                            false,
                                            _("Unsupported Guile directive \"%s\"."),
                                            directive);
                                  break;
                                default:
                                  abort ();
                                }
                              break;
                            }
                          phase1_ungetc (e);
                        }
                      while (i > 0)
                        {
                          i--;
                          phase1_ungetc (c[i]);
                        }
                    }
                  if (!seen_directive)
                    /* Block comment '#! ... !#'.  See
                       <https://www.gnu.org/software/guile/manual/html_node/Block-Comments.html>.  */
                    {
                      int c;

                      comment_start ();
                      c = phase1_getc ();
                      for (;;)
                        {
                          if (c == EOF)
                            break;
                          if (c == '!')
                            {
                              c = phase1_getc ();
                              if (c == EOF)
                                break;
                              if (c == '#')
                                {
                                  comment_line_end (0);
                                  break;
                                }
                              else
                                comment_add ('!');
                            }
                          else
                            {
                              /* We skip all leading white space.  */
                              if (!(buflen == 0 && (c == ' ' || c == '\t')))
                                comment_add (c);
                              if (c == '\n')
                                {
                                  comment_line_end (1);
                                  comment_start ();
                                }
                              c = phase1_getc ();
                            }
                        }
                      if (c == EOF)
                        {
                          /* EOF not allowed here.  But be tolerant.  */
                          op->type = t_eof;
                          return;
                        }
                      last_comment_line = line_number;
                    }
                  continue;
                }

              case '|':
                /* Block comment '#| ... |#'.  See
                   <https://www.gnu.org/software/guile/manual/html_node/Block-Comments.html>
                   and <https://srfi.schemers.org/srfi-30/srfi-30.html>.  */
                {
                  int depth = 0;
                  int c;

                  comment_start ();
                  c = phase1_getc ();
                  for (;;)
                    {
                      if (c == EOF)
                        break;
                      if (c == '|')
                        {
                          c = phase1_getc ();
                          if (c == EOF)
                            break;
                          if (c == '#')
                            {
                              if (depth == 0)
                                {
                                  comment_line_end (0);
                                  break;
                                }
                              depth--;
                              comment_add ('|');
                              comment_add ('#');
                              c = phase1_getc ();
                            }
                          else
                            comment_add ('|');
                        }
                      else if (c == '#')
                        {
                          c = phase1_getc ();
                          if (c == EOF)
                            break;
                          comment_add ('#');
                          if (c == '|')
                            {
                              depth++;
                              comment_add ('|');
                              c = phase1_getc ();
                            }
                        }
                      else
                        {
                          /* We skip all leading white space.  */
                          if (!(buflen == 0 && (c == ' ' || c == '\t')))
                            comment_add (c);
                          if (c == '\n')
                            {
                              comment_line_end (1);
                              comment_start ();
                            }
                          c = phase1_getc ();
                        }
                    }
                  if (c == EOF)
                    {
                      /* EOF not allowed here.  But be tolerant.  */
                      op->type = t_eof;
                      return;
                    }
                  last_comment_line = line_number;
                  continue;
                }

              case '*':
                /* Bit vector.  */
                {
                  struct token token;
                  read_token (&token, dmc);
                  /* The token should consists only of '0' and '1', except
                     for the initial '*'.  But be tolerant.  */
                  free_token (&token);
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              case '{':
                /* Symbol with multiple escapes: #{...}#  */
                {
                  op->token = XMALLOC (struct token);

                  init_token (op->token);

                  for (;;)
                    {
                      int c = phase1_getc ();

                      if (c == EOF)
                        break;
                      if (c == '\\')
                        {
                          c = phase1_getc ();
                          if (c == EOF)
                            break;
                        }
                      else if (c == '}')
                        {
                          c = phase1_getc ();
                          if (c == '#')
                            break;
                          if (c != EOF)
                            phase1_ungetc (c);
                          c = '}';
                        }
                      grow_token (op->token);
                      op->token->chars[op->token->charcount++] = c;
                    }

                  op->type = t_symbol;
                  last_non_comment_line = line_number;
                  return;
                }

              case '\\':
                /* Character.  */
                {
                  struct token token;
                  int c = phase1_getc ();
                  if (c != EOF)
                    {
                      read_token (&token, c);
                      free_token (&token);
                    }
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              case ':': /* Keyword.  */
              case '&': /* Deprecated keyword, installed in optargs.scm.  */
                {
                  struct token token;
                  read_token (&token, '-');
                  free_token (&token);
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              /* The following are installed through read-hash-extend.  */

              /* arrays.scm */
              case '0': case '1': case '2': case '3': case '4':
              case '5': case '6': case '7': case '8': case '9':
                /* Multidimensional array syntax: #nx(...) where
                     n ::= DIGIT+
                     x ::= {'a'|'b'|'c'|'e'|'i'|'s'|'u'}
                 */
                {
                  int c;
                  do
                    c = phase1_getc ();
                  while (c >= '0' && c <= '9');
                  /* c should be one of {'a'|'b'|'c'|'e'|'i'|'s'|'u'}.
                     But be tolerant.  */
                }
                FALLTHROUGH;
              case '\'': /* boot-9.scm */
              case '.': /* boot-9.scm */
              case ',': /* srfi-10.scm */
                {
                  struct object inner;
                  ++nesting_depth;
                  read_object (&inner, null_context_region ());
                  nesting_depth--;
                  /* Dots and EOF are not allowed here.
                     But be tolerant.  */
                  free_object (&inner);
                  op->type = t_other;
                  last_non_comment_line = line_number;
                  return;
                }

              default:
                /* Unknown.  */
                op->type = t_other;
                last_non_comment_line = line_number;
                return;
              }
            /*NOTREACHED*/
            abort ();
          }

        case '_':
          /* GIMP script-fu extension: '_' before a string literal is
             considered a gettext call on the string.  */
          {
            int c = phase1_getc ();
            if (c == EOF)
              /* Invalid input.  Be tolerant, no error message.  */
              {
                op->type = t_other;
                return;
              }
            if (c != '"')
              {
                phase1_ungetc (c);

                /* If '_' is not followed by a string literal,
                   consider it a part of symbol.  */
                op->token = XMALLOC (struct token);
                read_token (op->token, '_');
                op->type = t_symbol;
                last_non_comment_line = line_number;
                return;
              }
            seen_underscore_prefix = true;
          }
          FALLTHROUGH;

        case '"':
          /* String literal.  */
          {
            op->line_number_at_start = line_number;
            {
              struct mixed_string_buffer literal;
              mixed_string_buffer_init (&literal, lc_string,
                                        logical_file_name, line_number);
              accumulate_escaped (&literal);
              op->mixed_string = mixed_string_buffer_result (&literal);
            }
            op->type = t_string;

            if (seen_underscore_prefix || extract_all)
              {
                lex_pos_ty pos;

                pos.file_name = logical_file_name;
                pos.line_number = op->line_number_at_start;
                remember_a_message (mlp, NULL, string_of_object (op), true,
                                    false, null_context_region (), &pos,
                                    NULL, savable_comment, false);
              }
            last_non_comment_line = line_number;
            return;
          }

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '+': case '-': case '.':
          /* Read a number or symbol token.  */
          op->token = XMALLOC (struct token);
          read_token (op->token, ch);
          if (op->token->charcount == 1 && op->token->chars[0] == '.')
            {
              free_token (op->token);
              free (op->token);
              op->type = t_dot;
            }
          else if (is_number (op->token))
            {
              /* A number.  */
              free_token (op->token);
              free (op->token);
              op->type = t_other;
            }
          else
            {
              /* A symbol.  */
              op->type = t_symbol;
            }
          last_non_comment_line = line_number;
          return;

        case ':':
        default:
          /* Read a symbol token.  */
          op->token = XMALLOC (struct token);
          read_token (op->token, ch);
          op->type = t_symbol;
          last_non_comment_line = line_number;
          return;
        }
    }
}


static void
extract_whole_file (FILE *f,
                    const char *real_filename, const char *logical_filename,
                    flag_context_list_table_ty *flag_table,
                    msgdomain_list_ty *mdlp)
{
  mlp = mdlp->item[0]->messages;

  fp = f;
  real_file_name = real_filename;
  logical_file_name = xstrdup (logical_filename);
  line_number = 1;

  phase1_pushback_length = 0;

  casefold = false;

  last_comment_line = -1;
  last_non_comment_line = -1;

  flag_context_list_table = flag_table;
  nesting_depth = 0;
  datum_comment_nesting_depth = 0;

  init_keywords ();

  /* Eat tokens until eof is seen.  When read_object returns
     due to an unbalanced closing parenthesis, just restart it.  */
  do
    {
      struct object toplevel_object;

      read_object (&toplevel_object, null_context_region ());

      if (toplevel_object.type == t_eof)
        break;

      free_object (&toplevel_object);
    }
  while (!feof (fp));

  /* Close scanner.  */
  fp = NULL;
  real_file_name = NULL;
  logical_file_name = NULL;
  line_number = 0;
}

void
extract_scheme (FILE *f,
                const char *real_filename, const char *logical_filename,
                flag_context_list_table_ty *flag_table,
                msgdomain_list_ty *mdlp)
{
  follow_guile = false;
  extract_whole_file (f, real_filename, logical_filename, flag_table, mdlp);
}

void
extract_guile (FILE *f,
               const char *real_filename, const char *logical_filename,
               flag_context_list_table_ty *flag_table,
               msgdomain_list_ty *mdlp)
{
  follow_guile = true;
  extract_whole_file (f, real_filename, logical_filename, flag_table, mdlp);
}
