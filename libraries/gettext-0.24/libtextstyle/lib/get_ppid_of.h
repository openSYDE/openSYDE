/* Determine the parent process of a given process.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2019.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _GET_PPID_OF_H
#define _GET_PPID_OF_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the process id of the parent process of the given process,
   or 0 if it cannot be determined.  */
extern pid_t get_ppid_of (pid_t pid);

#ifdef __cplusplus
}
#endif

#endif /* _GET_PPID_OF_H */
