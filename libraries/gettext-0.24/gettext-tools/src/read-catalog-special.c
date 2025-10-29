/* Parsing of special comments (#, comments) in textual message catalogs.
   Copyright (C) 1995-2024 Free Software Foundation, Inc.

   This file was written by Peter Miller <millerp@canb.auug.org.au>

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
#include "read-catalog-special.h"

#include <limits.h>
#include <string.h>


/* Parse a special comment and put the result in *fuzzyp, formatp, *rangep,
   *wrapp, scp.  */
void
parse_comment_special (const char *s,
                       bool *fuzzyp, enum is_format formatp[NFORMATS],
                       struct argument_range *rangep, enum is_wrap *wrapp,
                       enum is_syntax_check scp[NSYNTAXCHECKS])
{
  size_t i;

  *fuzzyp = false;
  for (i = 0; i < NFORMATS; i++)
    formatp[i] = undecided;
  rangep->min = -1;
  rangep->max = -1;
  *wrapp = undecided;
  for (i = 0; i < NSYNTAXCHECKS; i++)
    scp[i] = undecided;

  while (*s != '\0')
    {
      const char *t;

      /* Skip whitespace.  */
      while (*s != '\0' && strchr ("\n \t\r\f\v,", *s) != NULL)
        s++;

      /* Collect a token.  */
      t = s;
      while (*s != '\0' && strchr ("\n \t\r\f\v,", *s) == NULL)
        s++;
      if (s != t)
        {
          size_t len = s - t;

          /* Accept fuzzy flag.  */
          if (len == 5 && memcmp (t, "fuzzy", 5) == 0)
            {
              *fuzzyp = true;
              continue;
            }

          /* Accept format description.  */
          if (len >= 7 && memcmp (t + len - 7, "-format", 7) == 0)
            {
              const char *p;
              size_t n;
              enum is_format value;

              p = t;
              n = len - 7;

              if (n >= 3 && memcmp (p, "no-", 3) == 0)
                {
                  p += 3;
                  n -= 3;
                  value = no;
                }
              else if (n >= 9 && memcmp (p, "possible-", 9) == 0)
                {
                  p += 9;
                  n -= 9;
                  value = possible;
                }
              else if (n >= 11 && memcmp (p, "impossible-", 11) == 0)
                {
                  p += 11;
                  n -= 11;
                  value = impossible;
                }
              else
                value = yes;

              for (i = 0; i < NFORMATS; i++)
                if (strlen (format_language[i]) == n
                    && memcmp (format_language[i], p, n) == 0)
                  {
                    formatp[i] = value;
                    break;
                  }
              if (i < NFORMATS)
                continue;
            }

          /* Accept range description "range: <min>..<max>".  */
          if (len == 6 && memcmp (t, "range:", 6) == 0)
            {
              /* Skip whitespace.  */
              while (*s != '\0' && strchr ("\n \t\r\f\v,", *s) != NULL)
                s++;

              /* Collect a token.  */
              t = s;
              while (*s != '\0' && strchr ("\n \t\r\f\v,", *s) == NULL)
                s++;
              /* Parse it.  */
              if (*t >= '0' && *t <= '9')
                {
                  unsigned int min = 0;

                  for (; *t >= '0' && *t <= '9'; t++)
                    {
                      if (min <= INT_MAX / 10)
                        {
                          min = 10 * min + (*t - '0');
                          if (min > INT_MAX)
                            min = INT_MAX;
                        }
                      else
                        /* Avoid integer overflow.  */
                        min = INT_MAX;
                    }
                  if (*t++ == '.')
                    if (*t++ == '.')
                      if (*t >= '0' && *t <= '9')
                        {
                          unsigned int max = 0;
                          for (; *t >= '0' && *t <= '9'; t++)
                            {
                              if (max <= INT_MAX / 10)
                                {
                                  max = 10 * max + (*t - '0');
                                  if (max > INT_MAX)
                                    max = INT_MAX;
                                }
                              else
                                /* Avoid integer overflow.  */
                                max = INT_MAX;
                            }
                          if (min <= max)
                            {
                              rangep->min = min;
                              rangep->max = max;
                              continue;
                            }
                        }
                }
            }

          /* Accept wrap description.  */
          if (len == 4 && memcmp (t, "wrap", 4) == 0)
            {
              *wrapp = yes;
              continue;
            }
          if (len == 7 && memcmp (t, "no-wrap", 7) == 0)
            {
              *wrapp = no;
              continue;
            }

          /* Accept syntax check description.  */
          if (len >= 6 && memcmp (t + len - 6, "-check", 6) == 0)
            {
              const char *p;
              size_t n;
              enum is_syntax_check value;

              p = t;
              n = len - 6;

              if (n >= 3 && memcmp (p, "no-", 3) == 0)
                {
                  p += 3;
                  n -= 3;
                  value = no;
                }
              else
                value = yes;

              for (i = 0; i < NSYNTAXCHECKS; i++)
                if (strlen (syntax_check_name[i]) == n
                    && memcmp (syntax_check_name[i], p, n) == 0)
                  {
                    scp[i] = value;
                    break;
                  }
              if (i < NSYNTAXCHECKS)
                continue;
            }

          /* Unknown special comment marker.  It may have been generated
             from a future xgettext version.  Ignore it.  */
        }
    }
}
