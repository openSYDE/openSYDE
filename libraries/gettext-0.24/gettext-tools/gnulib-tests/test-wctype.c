/* Test of getting descriptor for a wide character property.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include <wctype.h>

#include "signature.h"
SIGNATURE_CHECK (wctype, wctype_t, (const char *));

#include "macros.h"

int
main (int argc, char *argv[])
{
  wctype_t desc;

  desc = wctype ("any");
  ASSERT (desc == (wctype_t) 0);

  desc = wctype ("blank");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("space");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("punct");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("lower");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("upper");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("alpha");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("digit");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("xdigit");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("alnum");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("cntrl");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("graph");
  ASSERT (desc != (wctype_t) 0);

  desc = wctype ("print");
  ASSERT (desc != (wctype_t) 0);

  return test_exit_status;
}
