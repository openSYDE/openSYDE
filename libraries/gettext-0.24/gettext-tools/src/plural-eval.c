/* Expression evaluation for plural form selection.
   Copyright (C) 2000-2023 Free Software Foundation, Inc.
   Written by Ulrich Drepper <drepper@cygnus.com>, 2000.

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

/* Specification.  */
#include "plural-eval.h"

#include <stddef.h>

#include "plural-exp.h"


#define STATIC /*extern*/

/* Include the expression evaluation code from libintl, this time with
   'extern' linkage.  */
#include "eval-plural.h"
