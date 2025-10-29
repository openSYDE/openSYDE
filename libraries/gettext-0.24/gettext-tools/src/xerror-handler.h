/* Error handling during reading and writing of textual message catalogs.
   Copyright (C) 2005-2024 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#ifndef _XERROR_HANDLER_H
#define _XERROR_HANDLER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct message_ty;


/* These values must be the same as those in gettext-po.h.  */
#define CAT_SEVERITY_WARNING     0 /* just a warning, tell the user */
#define CAT_SEVERITY_ERROR       1 /* an error, the operation cannot complete */
#define CAT_SEVERITY_FATAL_ERROR 2 /* an error, the operation must be aborted */

/* An xerror_handler handles error situations.
   This one is higher-level than the one in po-error.h.  */
struct xerror_handler
{
  /* Signals a problem of the given severity.
     MESSAGE and/or FILENAME + LINENO indicate where the problem occurred.
     If FILENAME is NULL, FILENAME and LINENO and COLUMN should be ignored.
     If LINENO is (size_t)(-1), LINENO and COLUMN should be ignored.
     If COLUMN is (size_t)(-1), it should be ignored.
     MESSAGE_TEXT is the problem description (if MULTILINE_P is true,
     multiple lines of text, each terminated with a newline, otherwise
     usually a single line).
     Must not return if SEVERITY is CAT_SEVERITY_FATAL_ERROR.  */
  void (*xerror) (int severity,
                  const struct message_ty *message,
                  const char *filename, size_t lineno, size_t column,
                  int multiline_p, const char *message_text);

  /* Signals a problem that refers to two messages.
     Similar to two calls to xerror.
     If possible, a "..." can be appended to MESSAGE_TEXT1 and prepended to
     MESSAGE_TEXT2.  */
  void (*xerror2) (int severity,
                   const struct message_ty *message1,
                   const char *filename1, size_t lineno1, size_t column1,
                   int multiline_p1, const char *message_text1,
                   const struct message_ty *message2,
                   const char *filename2, size_t lineno2, size_t column2,
                   int multiline_p2, const char *message_text2);

  /* Points to an error_message_count variable or field.  */
  unsigned int *error_message_count_p;
};
typedef const struct xerror_handler *xerror_handler_ty;

/* The default xerror_handler, that uses the global variable error_message_count
   from <error.h>, and therefore is not multithread-safe.  */
extern DLL_VARIABLE const struct xerror_handler textmode_xerror_handler_struct;
#define textmode_xerror_handler (&textmode_xerror_handler_struct)


#ifdef __cplusplus
}
#endif

#endif /* _XERROR_HANDLER_H */
