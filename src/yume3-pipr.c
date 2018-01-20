/* yume3-pipr.c
 * jiw - 14 Mar 2011 - Pass arguments via pipe to reader
 * This program is part of the yume3 easy menu system.
 *
 * 19 Jan 2018 - Change from popen(ypath, "w"); for starting yume3, to
 * pipe - fork - exec sequence; to address issue with exported shell
 * functions not appearing in yume3's environment.  popen uses default
 * shell (with Ubuntu, typically dash) to start yume3.  Post-0.5.7
 * dash cleans exported shell functions out of the environment.  Many
 * yume scripts need such functions.
 *
 * Copyright 2009-2018 James Waldby.  Offered without warranty
 * under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
int i, t=2;
enum { BSIZ=1024, debug=0 };
  char ypath[BSIZ];
  FILE *fp;
  int fip=0;
  // Get absolute path to yume3 binary
  fp = popen("which yume3", "r"); // Start "which yume3" in a shell
  fgets (ypath, BSIZ, fp);	  // Read results from "which yume3"
  if (ferror(fp) || !strstr(ypath,"/yume3")) {
    fprintf (stderr, "Error: yumex did not find yume3\n");
    exit (1);
  }

  if (debug) printf ("yumex found yume3 at %s\n", ypath);
  pclose(fp);			// Close that pipe, done with it.

  // Add up the lengths of params to yume
  for (i=0; i<argc; ++i) {
    t += strlen(argv[i])+1;
    if (debug>3)
      printf ("yume3-pipr: %2d. <%s>\n", i, argv[i]);
  }
  if (debug>4)
    fp = popen("cat", "w");
  else {
    int pipefd[2];
    int psts = pipe(pipefd);
    if (psts == -1)
      { perror("Error: problem with yume pipe to yume3");
	exit(EXIT_FAILURE);
      }
    int cpid = fork();
    if (cpid == -1)
      { perror("Error: problem with yume fork");
	exit(EXIT_FAILURE);
      }

    if (cpid > 0) {		// Select code for parent
      fp = fdopen(pipefd[1], "w"); // Open the write end of pipe
      close(pipefd[0]);		   // Close our read-end handle
    } // Now flow to combined code to do either cat or yume
    else {			// Select code for child
    }
  }
   // Tell kid argument count; argument total chars; and input file desc.
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
