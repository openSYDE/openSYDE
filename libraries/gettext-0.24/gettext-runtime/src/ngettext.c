/* ngettext - retrieve plural form string from message catalog and print it.
   Copyright (C) 1995-2025 Free Software Foundation, Inc.

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

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

#include <error.h>
#include "attribute.h"
#include "noreturn.h"
#include "closeout.h"
#include "progname.h"
#include "relocatable.h"
#include "basename-lgpl.h"
#include "xalloc.h"
#include "propername.h"
#include "escapes.h"
#include "gettext.h"

#define _(str) gettext (str)

/* If true, expand escape sequences in strings before looking in the
   message catalog.  */
static int do_expand;

/* Long options.  */
static const struct option long_options[] =
{
  { "context", required_argument, NULL, 'c' },
  { "domain", required_argument, NULL, 'd' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'V' },
  { NULL, 0, NULL, 0 }
};

/* Forward declaration of local functions.  */
_GL_NORETURN_FUNC static void usage (int status);

int
main (int argc, char *argv[])
{
  int optchar;
  const char *msgid;
  const char *msgid_plural;
  const char *count;
  unsigned long n;

  /* Default values for command line options.  */
  bool do_help = false;
  bool do_version = false;
  const char *domain = getenv ("TEXTDOMAIN");
  const char *domaindir = getenv ("TEXTDOMAINDIR");
  const char *context = NULL;
  do_expand = false;

  /* Set program name for message texts.  */
  set_program_name (argv[0]);

  /* Set locale via LC_ALL.  */
  setlocale (LC_ALL, "");

  /* Set the text message domain.  */
  bindtextdomain (PACKAGE, relocate (LOCALEDIR));
  bindtextdomain ("gnulib", relocate (GNULIB_LOCALEDIR));
  textdomain (PACKAGE);

  /* Ensure that write errors on stdout are detected.  */
  atexit (close_stdout);

  /* Parse command line options.  */
  while ((optchar = getopt_long (argc, argv, "+c:d:eEhV", long_options, NULL))
         != EOF)
    switch (optchar)
    {
    case '\0':          /* Long option.  */
      break;
    case 'c':
      context = optarg;
      break;
    case 'd':
      domain = optarg;
      break;
    case 'e':
      do_expand = true;
      break;
    case 'E':
      /* Ignore.  Just for compatibility.  */
      break;
    case 'h':
      do_help = true;
      break;
    case 'V':
      do_version = true;
      break;
    default:
      usage (EXIT_FAILURE);
    }

  /* Version information is requested.  */
  if (do_version)
    {
      printf ("%s (GNU %s) %s\n", last_component (program_name),
              PACKAGE, VERSION);
      /* xgettext: no-wrap */
      printf (_("Copyright (C) %s Free Software Foundation, Inc.\n\
License GPLv3+: GNU GPL version 3 or later <%s>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
"),
              "1995-2025", "https://gnu.org/licenses/gpl.html");
      printf (_("Written by %s.\n"), proper_name ("Ulrich Drepper"));
      exit (EXIT_SUCCESS);
    }

  /* Help is requested.  */
  if (do_help)
    usage (EXIT_SUCCESS);

  /* More optional command line options.  */
  switch (argc - optind)
    {
    default:
      error (EXIT_FAILURE, 0, _("too many arguments"));

    case 4:
      domain = argv[optind++];
      FALLTHROUGH;

    case 3:
      break;

    case 2:
    case 1:
    case 0:
      error (EXIT_FAILURE, 0, _("missing arguments"));
    }

  /* Now the mandatory command line options.  */
  msgid = argv[optind++];
  msgid_plural = argv[optind++];
  count = argv[optind++];

  if (optind != argc)
    abort ();

  {
    char *endp;
    unsigned long tmp_val;

    errno = 0;
    tmp_val = strtoul (count, &endp, 10);
    if (errno == 0 && count[0] != '\0' && endp[0] == '\0')
      n = tmp_val;
    else
      /* When COUNT is not valid, use plural.  */
      n = 99;
  }

  /* Expand escape sequences if enabled.  */
  if (do_expand)
    {
      msgid = expand_escapes (msgid, NULL);
      msgid_plural = expand_escapes (msgid_plural, NULL);
    }

  /* If no domain name is given we don't translate, and we use English
     plural form handling.  */
  if (domain == NULL || domain[0] == '\0')
    fputs (n == 1 ? msgid : msgid_plural, stdout);
  else
    {
      /* Bind domain to appropriate directory.  */
      if (domaindir != NULL && domaindir[0] != '\0')
        bindtextdomain (domain, domaindir);

      /* Write out the result.  */
      fputs ((context != NULL
              ? dnpgettext_expr (domain, context, msgid, msgid_plural, n)
              : dngettext (domain, msgid, msgid_plural, n)),
             stdout);
    }

  exit (EXIT_SUCCESS);
}


/* Display usage information and exit.  */
static void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try '%s --help' for more information.\n"),
             program_name);
  else
    {
      /* xgettext: no-wrap */
      printf (_("\
Usage: %s [OPTION] [TEXTDOMAIN] MSGID MSGID-PLURAL COUNT\n\
"), program_name);
      printf ("\n");
      /* xgettext: no-wrap */
      printf (_("\
Display native language translation of a textual message whose grammatical\n\
form depends on a number.\n"));
      printf ("\n");
      /* xgettext: no-wrap */
      printf (_("\
  -d, --domain=TEXTDOMAIN   retrieve translated message from TEXTDOMAIN\n"));
      printf (_("\
  -c, --context=CONTEXT     specify context for MSGID\n"));
      printf (_("\
  -e                        enable expansion of some escape sequences\n"));
      printf (_("\
  -E                        (ignored for compatibility)\n"));
      printf (_("\
  [TEXTDOMAIN]              retrieve translated message from TEXTDOMAIN\n"));
      printf (_("\
  MSGID MSGID-PLURAL        translate MSGID (singular) / MSGID-PLURAL (plural)\n"));
      printf (_("\
  COUNT                     choose singular/plural form based on this value\n"));
      printf ("\n");
      printf (_("\
Informative output:\n"));
      printf (_("\
  -h, --help                display this help and exit\n"));
      printf (_("\
  -V, --version             display version information and exit\n"));
      printf ("\n");
      /* xgettext: no-wrap */
      printf (_("\
If the TEXTDOMAIN parameter is not given, the domain is determined from the\n\
environment variable TEXTDOMAIN.  If the message catalog is not found in the\n\
regular directory, another location can be specified with the environment\n\
variable TEXTDOMAINDIR.\n\
Standard search directory: %s\n"),
              getenv ("IN_HELP2MAN") == NULL ? LOCALEDIR : "@localedir@");
      printf ("\n");
      /* TRANSLATORS: The first placeholder is the web address of the Savannah
         project of this package.  The second placeholder is the bug-reporting
         email address for this package.  Please add _another line_ saying
         "Report translation bugs to <...>\n" with the address for translation
         bugs (typically your translation team's web or email address).  */
      printf(_("\
Report bugs in the bug tracker at <%s>\n\
or by email to <%s>.\n"),
             "https://savannah.gnu.org/projects/gettext",
             "bug-gettext@gnu.org");
    }

  exit (status);
}
