/* Fill wide character array.
   Copyright (C) 1999, 2011-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 1999.

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

wchar_t *
wmemset (wchar_t *s, wchar_t c, size_t n)
{
  wchar_t *ptr = s;

  for (; n > 0; n--)
    *ptr++ = c;
  return s;
}
