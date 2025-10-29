/* Reading PO files.
   Copyright (C) 2006-2024 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

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

#ifndef _READ_PO_H
#define _READ_PO_H

#include "read-catalog-abstract.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Describes a .po / .pot file parser.  */
extern DLL_VARIABLE const struct catalog_input_format input_format_po;


/* Global variables.  */

/* Number of parse errors within a PO file that cause the program to
   terminate.  Cf. error_message_count, declared in <error.h>.  */
extern DLL_VARIABLE unsigned int gram_max_allowed_errors;


#ifdef __cplusplus
}
#endif


#endif /* _READ_PO_H */
