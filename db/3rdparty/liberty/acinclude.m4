dnl
dnl AC_PROG_BISON (MINIMUM-VERSION)
dnl
dnl Check for availability of bison.
dnl Abort if not found or if current version is not up to par.
dnl

AC_DEFUN([AC_PROG_BISON],[
	AC_PATH_PROG(BISON, bison, no)
	if test "$BISON" = no; then
		AC_MSG_ERROR(Could not find bison)
	fi
	min_bison_version=ifelse([$1], ,1.22,$1)
	AC_MSG_CHECKING(for bison - version >= $min_bison_version)
	bison_major_version=`$BISON --version | head -1 | \
		sed 's/bison (GNU Bison) \([[0-9]]*\).\([[0-9]]*\).*/\1/'`
	bison_minor_version=`$BISON --version | head -1 | \
		sed 's/bison (GNU Bison) \([[0-9]]*\).\([[0-9]]*\).*/\2/'`
	no_bison=""
dnl
dnl Now check if the installed bison is sufficiently new.
dnl
	AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int 
main ()
{
  char  *tmp_version;
  
  int    major;
  int    minor;

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_bison_version");
  if (sscanf(tmp_version, "%d.%d", &major, &minor) != 2) {
    printf ("%s, bad version string\n", "$min_bison_version");
    exit (1);
  }

  if (($bison_major_version > major) ||
      (($bison_major_version == major) && ($bison_minor_version >= minor))) {
    return 0;
  } else {
    printf ("\n");
    printf ("*** An old version of bison ($bison_major_version.$bison_minor_version) was found.\n");
    printf ("*** You need a version of bison newer than %d.%d.  The latest version of\n",
	       major, minor);
    printf ("*** bison is always available from ftp://ftp.gnu.org.\n");
    printf ("***\n");
    return 1;
  }
}
],,no_bison=yes,[/bin/true])
	if test "x$no_bison" = x ; then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi

])
dnl
dnl AC_PROG_GPERF (MINIMUM-VERSION)
dnl
dnl Check for availability of gperf.
dnl Abort if not found or if current version is not up to par.
dnl

AC_DEFUN([AC_PROG_GPERF],[
	AC_PATH_PROG(GPERF, gperf, no)
	if test "$GPERF" = no; then
		AC_MSG_ERROR(Could not find gperf)
	fi
	min_gperf_version=ifelse([$1], ,2.7,$1)
	AC_MSG_CHECKING(for gperf - version >= $min_gperf_version)
	gperf_major_version=`$GPERF --version | head -1 | \
		sed 's/GNU gperf \([[0-9]]*\).\([[0-9]]*\)/\1/'`
	gperf_minor_version=`$GPERF --version | head -1 | \
		sed 's/GNU gperf \([[0-9]]*\).\([[0-9]]*\)/\2/'`
	no_gperf=""
dnl
dnl Now check if the installed gperf is sufficiently new.
dnl
	AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int 
main ()
{
  char  *tmp_version;
  
  int    major;
  int    minor;

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_gperf_version");
  if (sscanf(tmp_version, "%d.%d", &major, &minor) != 2) {
    printf ("%s, bad version string\n", "$min_gperf_version");
    exit (1);
  }

  if (($gperf_major_version > major) ||
      (($gperf_major_version == major) && ($gperf_minor_version >= minor))) {
    return 0;
  } else {
    printf ("\n");
    printf ("*** An old version of gperf ($gperf_major_version.$gperf_minor_version) was found.\n");
    printf ("*** You need a version of gperf newer than %d.%d.  The latest version of\n",
	       major, minor);
    printf ("*** gperf is always available from ftp://ftp.gnu.org.\n");
    printf ("***\n");
    return 1;
  }
}
],,no_gperf=yes,[/bin/true])
	if test "x$no_gperf" = x ; then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi

])
