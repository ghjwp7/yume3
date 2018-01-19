/* $Id: yume3-pipr.c,v 1.2 2011/03/21 04:38:06 j-waldby Exp j-waldby $
 *
 * jiw - 14 Mar 2011 - Pass arguments via pipe to reader
 * This program is part of the yume3 easy menu system.
 *
 * Copyright 2009-2011 James Waldby.  Offered without warranty
 * under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int i, t=2;
  enum {BSIZ=1024, debug=0 };
  char ypath[BSIZ];
  FILE *fp;

  fp = popen("which yume3", "r");
  fgets (ypath, BSIZ, fp);
  if (ferror(fp) || !strstr(ypath,"/yume3")) {
    fprintf (stderr, "Error: yumex did not find yume3\n");
    exit (1);
  }

  if (debug) printf ("yumex found yume3 at %s\n", ypath);
  pclose(fp);

  for (i=0; i<argc; ++i) {
    t += strlen(argv[i])+1;
    if (debug>3)
      printf ("yume3-pipr: %2d. <%s>\n", i, argv[i]);
  }
  if (debug>4)
    fp = popen("cat", "w");
  else
    fp = popen(ypath, "w");
  fprintf (fp, "%d %d\n", argc, t);
  if (debug) printf ("yumex tells yume3 %d args with %d chars\n",
		     argc, t);

  for (i=0; i<argc; ++i) {
    int k, L = strlen(argv[i]);
    if (debug>2)printf ("yume3-pipr> %2d. <%s> %d\n", i, argv[i], L);
    if (L) {	// Squeeze out backslash-newline pairs, if any
      for (k=t=0; k<L-1; ++k) {
	if (argv[i][k]=='\\' && argv[i][k+1]=='\n')
	  ++k;			// Skip over backslash-newline
	else argv[i][t++] = argv[i][k];
      }
      argv[i][t++] = argv[i][k];	// Get last character
      argv[i][t] = 0;		// Terminate string
    }
    fprintf (fp, "%s\n", argv[i]);
    if (debug>1)printf ("yume3-pipr: %2d. <%s>\n", i, argv[i]);
  }

  if (debug) printf ("yumex is done\n");
  fflush(stdout);
  fflush(fp);
  _exit(0);			// Use _exit for single exit
}
