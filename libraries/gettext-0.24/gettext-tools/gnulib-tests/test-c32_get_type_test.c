/* Test of getting descriptor for a 32-bit wide character property.
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

#include <uchar.h>

#include "signature.h"
SIGNATURE_CHECK (c32_get_type_test, c32_type_test_t, (const char *));

#include "macros.h"

int
main (int argc, char *argv[])
{
  c32_type_test_t desc;

  desc = c32_get_type_test ("any");
  ASSERT (desc == (c32_type_test_t) 0);

  desc = c32_get_type_test ("blank");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("space");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("punct");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("lower");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("upper");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("alpha");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("digit");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("xdigit");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("alnum");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("cntrl");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("graph");
  ASSERT (desc != (c32_type_test_t) 0);

  desc = c32_get_type_test ("print");
  ASSERT (desc != (c32_type_test_t) 0);

  return test_exit_status;
}
