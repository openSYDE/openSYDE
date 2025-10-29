/* Implementation of the bindtextdomain(3) function
   Copyright (C) 1995-2024 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "gettextP.h"
#ifdef _LIBC
# include <libintl.h>
#else
# include "libgnuintl.h"
#endif

/* Handle multi-threaded applications.  */
#ifdef _LIBC
# include <libc-lock.h>
# define gl_rwlock_define __libc_rwlock_define
# define gl_rwlock_wrlock __libc_rwlock_wrlock
# define gl_rwlock_unlock __libc_rwlock_unlock
#else
# include "glthread/lock.h"
#endif

#include "flexmember.h"

/* @@ end of prolog @@ */


/* Names for the libintl functions are a problem.  They must not clash
   with existing names and they should follow ANSI C.  But this source
   code is also used in GNU C Library where the names have a __
   prefix.  So we have to make a difference here.  */
#ifdef _LIBC
# define BINDTEXTDOMAIN __bindtextdomain
# define BIND_TEXTDOMAIN_CODESET __bind_textdomain_codeset
# ifndef strdup
#  define strdup(str) __strdup (str)
# endif
#else
# define BINDTEXTDOMAIN libintl_bindtextdomain
# define BIND_TEXTDOMAIN_CODESET libintl_bind_textdomain_codeset
#endif

/* Specifies the directory name *DIRNAMEP, the directory name *WDIRNAMEP
   (only on native Windows), and the output codeset *CODESETP to be used
   for the DOMAINNAME message catalog.
   If *DIRNAMEP or *WDIRNAMEP or *CODESETP is NULL, the corresponding attribute
   is not modified, only the current value is returned.
   If DIRNAMEP or WDIRNAMEP or CODESETP is NULL, the corresponding attribute is
   neither modified nor returned, except that setting WDIRNAME erases DIRNAME
   and vice versa.  */
static void
set_binding_values (const char *domainname,
		    const char **dirnamep, const wchar_t **wdirnamep,
		    const char **codesetp)
{
  struct binding *binding;
  int modified;

  /* Some sanity checks.  */
  if (domainname == NULL || domainname[0] == '\0')
    {
      if (dirnamep)
	*dirnamep = NULL;
#if defined _WIN32 && !defined __CYGWIN__
      if (wdirnamep)
	*wdirnamep = NULL;
#endif
      if (codesetp)
	*codesetp = NULL;
      return;
    }

  gl_rwlock_wrlock (_nl_state_lock);

  modified = 0;

  for (binding = _nl_domain_bindings; binding != NULL; binding = binding->next)
    {
      int compare = strcmp (domainname, binding->domainname);
      if (compare == 0)
	/* We found it!  */
	break;
      if (compare < 0)
	{
	  /* It is not in the list.  */
	  binding = NULL;
	  break;
	}
    }

  if (binding != NULL)
    {
      if (dirnamep)
	{
	  const char *dirname = *dirnamep;

	  if (dirname == NULL)
	    /* The current binding has be to returned.  */
	    *dirnamep = binding->dirname;
	  else
	    {
	      /* The domain is already bound.  If the new value and the old
		 one are equal we simply do nothing.  Otherwise replace the
		 old binding.  */
	      char *result = binding->dirname;
	      if (result == NULL || strcmp (dirname, result) != 0)
		{
		  if (strcmp (dirname, _nl_default_dirname) == 0)
		    result = (char *) _nl_default_dirname;
		  else
		    result = strdup (dirname);

		  if (__builtin_expect (result != NULL, 1))
		    {
		      if (binding->dirname != _nl_default_dirname)
			free (binding->dirname);
		      binding->dirname = result;

#if defined _WIN32 && !defined __CYGWIN__
		      free (binding->wdirname);
		      binding->wdirname = NULL;
#endif

		      modified = 1;
		    }
		}
	      *dirnamep = result;
	    }
	}

#if defined _WIN32 && !defined __CYGWIN__
      if (wdirnamep)
	{
	  const wchar_t *wdirname = *wdirnamep;

	  if (wdirname == NULL)
	    /* The current binding has be to returned.  */
	    *wdirnamep = binding->wdirname;
	  else
	    {
	      /* The domain is already bound.  If the new value and the old
		 one are equal we simply do nothing.  Otherwise replace the
		 old binding.  */
	      wchar_t *result = binding->wdirname;
	      if (result == NULL || wcscmp (wdirname, result) != 0)
		{
		  result = _wcsdup (wdirname);

		  if (__builtin_expect (result != NULL, 1))
		    {
		      if (binding->dirname != _nl_default_dirname)
			free (binding->dirname);
		      binding->dirname = NULL;

		      free (binding->wdirname);
		      binding->wdirname = result;

		      modified = 1;
		    }
		}
	      *wdirnamep = result;
	    }
	}
#endif

      if (codesetp)
	{
	  const char *codeset = *codesetp;

	  if (codeset == NULL)
	    /* The current binding has be to returned.  */
	    *codesetp = binding->codeset;
	  else
	    {
	      /* The domain is already bound.  If the new value and the old
		 one are equal we simply do nothing.  Otherwise replace the
		 old binding.  */
	      char *result = binding->codeset;
	      if (result == NULL || strcmp (codeset, result) != 0)
		{
		  result = strdup (codeset);
		  if (__builtin_expect (result != NULL, 1))
		    {
		      free (binding->codeset);

		      binding->codeset = result;
		      modified = 1;
		    }
		}
	      *codesetp = result;
	    }
	}
    }
  else if ((dirnamep == NULL || *dirnamep == NULL)
#if defined _WIN32 && !defined __CYGWIN__
	   && (wdirnamep == NULL || *wdirnamep == NULL)
#endif
	   && (codesetp == NULL || *codesetp == NULL))
    {
      /* Simply return the default values.  */
      if (dirnamep)
	*dirnamep = _nl_default_dirname;
#if defined _WIN32 && !defined __CYGWIN__
      if (wdirnamep)
	*wdirnamep = NULL;
#endif
      if (codesetp)
	*codesetp = NULL;
    }
  else
    {
      /* We have to create a new binding.  */
      size_t len = strlen (domainname) + 1;
      struct binding *new_binding =
	(struct binding *)
	malloc (FLEXNSIZEOF (struct binding, domainname, len));


      if (__builtin_expect (new_binding == NULL, 0))
	goto failed;

      memcpy (new_binding->domainname, domainname, len);

      if (dirnamep)
	{
	  const char *dirname = *dirnamep;

	  if (dirname == NULL)
	    {
#if defined _WIN32 && !defined __CYGWIN__
	      if (wdirnamep && *wdirnamep != NULL)
		dirname = NULL;
	      else
#endif
		/* The default value.  */
		dirname = _nl_default_dirname;
	    }
	  else
	    {
	      if (strcmp (dirname, _nl_default_dirname) == 0)
		dirname = _nl_default_dirname;
	      else
		{
		  char *result = strdup (dirname);
		  if (__builtin_expect (result == NULL, 0))
		    goto failed_dirname;
		  dirname = result;
		}
	    }
	  *dirnamep = dirname;
	  new_binding->dirname = (char *) dirname;
	}
      else
	{
#if defined _WIN32 && !defined __CYGWIN__
	  if (wdirnamep && *wdirnamep != NULL)
	    new_binding->dirname = NULL;
	  else
#endif
	    /* The default value.  */
	    new_binding->dirname = (char *) _nl_default_dirname;
	}

#if defined _WIN32 && !defined __CYGWIN__
      if (wdirnamep)
	{
	  const wchar_t *wdirname = *wdirnamep;

	  if (wdirname != NULL)
	    {
	      wchar_t *result = _wcsdup (wdirname);
	      if (__builtin_expect (result == NULL, 0))
		goto failed_wdirname;
	      wdirname = result;
	    }
	  *wdirnamep = wdirname;
	  new_binding->wdirname = (wchar_t *) wdirname;
	}
      else
	new_binding->wdirname = NULL;
#endif

      if (codesetp)
	{
	  const char *codeset = *codesetp;

	  if (codeset != NULL)
	    {
	      char *result = strdup (codeset);
	      if (__builtin_expect (result == NULL, 0))
		goto failed_codeset;
	      codeset = result;
	    }
	  *codesetp = codeset;
	  new_binding->codeset = (char *) codeset;
	}
      else
	new_binding->codeset = NULL;

      /* Now enqueue it.  */
      if (_nl_domain_bindings == NULL
	  || strcmp (domainname, _nl_domain_bindings->domainname) < 0)
	{
	  new_binding->next = _nl_domain_bindings;
	  _nl_domain_bindings = new_binding;
	}
      else
	{
	  binding = _nl_domain_bindings;
	  while (binding->next != NULL
		 && strcmp (domainname, binding->next->domainname) > 0)
	    binding = binding->next;

	  new_binding->next = binding->next;
	  binding->next = new_binding;
	}

      modified = 1;

      /* Here we deal with memory allocation failures.  */
      if (0)
	{
	failed_codeset:
#if defined _WIN32 && !defined __CYGWIN__
	  free (new_binding->wdirname);
	failed_wdirname:
#endif
	  if (new_binding->dirname != _nl_default_dirname)
	    free (new_binding->dirname);
	failed_dirname:
	  free (new_binding);
	failed:
	  if (dirnamep)
	    *dirnamep = NULL;
#if defined _WIN32 && !defined __CYGWIN__
	  if (wdirnamep)
	    *wdirnamep = NULL;
#endif
	  if (codesetp)
	    *codesetp = NULL;
	}
    }

  /* If we modified any binding, we flush the caches.  */
  if (modified)
    ++_nl_msg_cat_cntr;

  gl_rwlock_unlock (_nl_state_lock);
}

/* Specify that the DOMAINNAME message catalog will be found
   in DIRNAME rather than in the system locale data base.  */
char *
BINDTEXTDOMAIN (const char *domainname, const char *dirname)
{
#ifdef __EMX__
  const char *saved_dirname = dirname;
  char dirname_with_drive[_MAX_PATH];

# ifdef __KLIBC__
  if (dirname && strncmp (dirname, "/@unixroot", 10) == 0
      && (dirname[10] == '\0' || dirname[10] == '/' || dirname[10] == '\\'))
    /* kLIBC itself processes /@unixroot prefix */;
  else
# endif
  /* Resolve UNIXROOT into dirname if it is not resolved by os2compat.[ch]. */
  if (dirname && (dirname[0] == '/' || dirname[0] == '\\' ))
    {
      const char *unixroot = getenv ("UNIXROOT");
      size_t len = strlen (dirname) + 1;

      if (unixroot
          && unixroot[0] != '\0'
          && unixroot[1] == ':'
          && unixroot[2] == '\0'
          && 2 + len <= _MAX_PATH)
        {
          memcpy (dirname_with_drive, unixroot, 2);
          memcpy (dirname_with_drive + 2, dirname, len);

          dirname = dirname_with_drive;
        }
    }
#endif
  set_binding_values (domainname, &dirname, NULL, NULL);
#ifdef __EMX__
  dirname = saved_dirname;
#endif
  return (char *) dirname;
}

#if defined _WIN32 && !defined __CYGWIN__
/* Specify that the DOMAINNAME message catalog will be found
   in WDIRNAME rather than in the system locale data base.  */
wchar_t *
libintl_wbindtextdomain (const char *domainname, const wchar_t *wdirname)
{
  set_binding_values (domainname, NULL, &wdirname, NULL);
  return (wchar_t *) wdirname;
}
#endif

/* Specify the character encoding in which the messages from the
   DOMAINNAME message catalog will be returned.  */
char *
BIND_TEXTDOMAIN_CODESET (const char *domainname, const char *codeset)
{
  set_binding_values (domainname, NULL, NULL, &codeset);
  return (char *) codeset;
}

#ifdef _LIBC
/* Aliases for function names in GNU C Library.  */
weak_alias (__bindtextdomain, bindtextdomain);
weak_alias (__bind_textdomain_codeset, bind_textdomain_codeset);
#endif
