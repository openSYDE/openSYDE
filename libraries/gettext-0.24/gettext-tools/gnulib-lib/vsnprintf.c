/* Formatted output to strings.
   Copyright (C) 2004, 2006-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

int
vsnprintf (char *str, size_t size, const char *format, va_list args)
{
  ptrdiff_t ret = vsnzprintf (str, size, format, args);

#if PTRDIFF_MAX > INT_MAX
  if (ret > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }
#endif

  return ret;
}
