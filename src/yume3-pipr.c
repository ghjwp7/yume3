/* yume3-pipr.c
 * jiw - 14 Mar 2011 - Pass arguments via pipe to reader
 * This program is part of the yume3 easy menu system.
 *
 * 19 Jan 2018 - Change from popen(ypath, "w"); for starting yume3, to
 * pipe - fork - exec sequence; to address issue with exported shell
 * functions not appearing in yume3's environment.  popen uses default
 * shell (with Ubuntu, typically dash) to start yume3.  Post-0.5.7
 * dash cleans exported shell functions out of the environment.  Many
 * yume scripts don't work without such functions.
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
  int i, parLen=2;
  enum { BSIZ=1024 };
  FILE *fp;
  int pipefd[2];
  int psts = pipe(pipefd);	// Get a pipe for sending params
  if (psts == -1)
    { perror("Error: problem with yume pipe to yume3");
      exit(EXIT_FAILURE);
    }
  int cpid = fork();		// Fork to get a new process
  if (cpid == -1)
    { perror("Error: problem with yume fork");
      exit(EXIT_FAILURE);
    }
  // Add up the lengths of params to yume
  for (i=0; i<argc; ++i)
    parLen += strlen(argv[i])+1;
  
  //fprintf (stderr, "yume to tell yume3 %d args with %d chars\n", argc, parLen);

  //-----------------------------------------------------------------
  if (cpid == 0) {		// Select code for child
    close(pipefd[1]);		// Close our pipe write descriptor
    
    // Child needs to exec yume3, so get its path
    char apath[BSIZ];
    fp = popen("which yume3", "r"); // Run "which yume3" in a shell
    //fp = popen("which ../bin/yume3", "r"); // Run "which yume3" in a shell
    char *ypath = fgets (apath, BSIZ, fp); // Read results from "which yume3"
    
    if (!ypath || ferror(fp) || !strstr(ypath,"/yume3")) {
      fprintf (stderr, "Error: yume did not find yume3\n");
      exit (1);
    }
    pclose(fp);			// Done with the popen pipe, close it.
    char *cpath = "/home/j-waldby/yy/yume3/bin/yume3";   // substitute path for debugging
    ypath = cpath;		// use the substitute path
    //fprintf (stderr, "yume found yume3 at %s\n", ypath);
    
    enum { SLEN=11, NLEN=1+SLEN };
    char readfd[NLEN];
    extern char **environ;
    snprintf (readfd,   SLEN, "%d", pipefd[0]); readfd[SLEN]=0;

    //    binary   par0   par1   EOP   environment
    execle(ypath, ypath, readfd, NULL, environ);
  }
  //-----------------------------------------------------------------
  // Code for parent
  fp = fdopen(pipefd[1], "w");	// Open file on our pipe write descriptor
  close(pipefd[0]);		// Close our pipe read descriptor

  // Tell the kid the arguments to yume
  //fprintf (stderr, "yume3-pipr parent piping %d args, %d chars, to y3\n", argc, parLen);
  fprintf (fp, "%d\n", argc);	// Number of Y pars
  fprintf (fp, "%d\n", parLen); // Total number of chars in Y pars
  for (i=0; i<argc; ++i) {
    int j, k, L = strlen(argv[i]);
    if (L) {	// Squeeze out backslash-newline pairs, if any
      for (j=k=0; k<L-1; ++k) {
	if (argv[i][k]=='\\' && argv[i][k+1]=='\n')
	  ++k;			// Skip over backslash-newline
	else argv[i][j++] = argv[i][k];
      }
      argv[i][j++] = argv[i][k];	// Get last character
      argv[i][j] = 0;		// Terminate string
    }
    fprintf (fp, "%s\n", argv[i]);
  }

  //fprintf (stderr, "yume3-pipr: Done\n");
  fflush(stdout);
  fflush(fp);
  _exit(0);			// Use _exit for single exit
}
