/* yume3 --  By James Waldby, Feb 2011
$Id: yume3.c,v 1.4 2011/04/05 19:49:26 j-waldby Exp j-waldby $
Copyright 2009-2011 James Waldby.  Offered without warranty
under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

[That is, terms of the GNU General Public License version 3 published
 by the Free Software Foundation apply to the yume3 program, which is
 embodied in source files yume3.c, yume3.h, yume3-p[123].[ch], and
 some other yume3* files.]

 yume is an easy-to-use menu system with buttons and editable
 roll-over user commands, for X Window System, formerly based on
 xforms and now based on gtk.  Given a set of arguments, yume creates
 and runs a corresponding menu system.  The argument items (commands
 and switches) shown in the synopsis may appear in any desired order
 to create a particular menu.  See man pages yume(1) and
 yume-examples(1); also see yume examples directory.

 SYNOPSIS [as extracted from yume3.1]

.B yume  [commands] [-bu] [-do] [-dl] [-ex] [-at geometry] [-in file]
.B  [-iv value] [-la label] [-ti title] [-de valpair]
.B  [-bd buttondata] [-bw widthcodes] [-Y[adeilnotuvwx]] [-Y<d>]

 */
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>

#include "yume3.h"		// Structs for yume3
#include "yume3-p1.h"
#include "yume3-p2.h"
#include "yume3-p3.h"
#include "yume3-util.h"
//--------------------------------------------
// Global vars

ListItem *items;		// Array of elements of menu
int nitems;			// # of items in menu at moment
int decode_ErrorFlag;		// Set if a name lookup fails
int inDepth=0;			// Current nest-depth of includes
int ShoMode;			// 3 bits control 3 prints
int buHi, doHi, ddHi, eeHi;	// Char heights per process_command_options
int doM1, buM1;			// Width of M's in do- & bu-text fonts
int hasAt;			// 1 if -at appeared with > 2 terms 
int hasSize;			// 1 if -at appeared
int xat, yat, wat, hat;		// Form-location per -at param
int y3pid;			// yume3 process id #
EntryType BuCur;		// Current button type
int Aflag;			// assertion-flag
unsigned int Schild;		// number of active children
unsigned int Echild;		// number of ended children
int vv[VV_VV] = {0};		// Default value settings
char *wraps[ET_ET];		// wrapper pointers
char *wrapt[ET_ET];		// wrapped-string pointers


char *atPar=NULL;		// -at param value
char *buDefs=NULL;		// define special submenu
char *formTitle=NULL;		// Title of menu window
char *gtkrc=NULL;		// gtkrc file name
char *gtkPar=NULL;		// gtk parameter string
char *grcFileName;		// Path to default gtk resource file
char *yrcFileName;		// Path to default yume resource file

enum {fpidTiSize=128};
char fpidTitle[fpidTiSize];	// Alternate title of menu window
//--------------------------------------------
void startingError(int ss) {
  fprintf (stderr, "Startup error %d:  yume3 was miscalled?\n", ss);
  _exit(0);
}
//--------------------------------------------
//--------Main Program------------------------
//--------------------------------------------
int main (int argc, char *argv[]) {
  // Set vv[] to all zeroes
  memset (vv, 0, VV_VV * sizeof vv[0]);
  // Set non-zeroes in vv[]
  vv[VVfsl] = 400;		// Default value of FStackLim
  vv[VVbuq] = 5;		// Default value of buQuanta
  vv[VVxys] = 1;		// Default value of XYshow
  // Default values of Push vs Roll modes for b,d,e,i,x
  vv[VVbb]=vv[VVdb]=vv[VVeb]=vv[VVib]=vv[VVxb]=1;
    
  FILE *fin;  
  int i, L, at=0;
  enum { InLen=10 };
  char *c, *endptr, **targv, *pargv, inNum[InLen+1];
  
  if (argc != 2) startingError(1); // Need exactly 2 arguments
  
  // Our param is fid, the file descriptor for our input pipe or file.
  const long int fid   = strtol(argv[1], &endptr, 10); // 10 = base 10
  fin = fdopen(fid, "r");	// Read from pipe
  if (!fin) startingError(2);	// Is pipe ok?
  // Read numYPars (number of yume params) and totChars (param-sizes total)
  c = fgets (inNum, InLen, fin);  inNum[InLen] = 0;
  if (!c) startingError(3);	// Did argct read ok?
  const long int argct   = strtol(inNum, &endptr, 10);
  c = fgets (inNum, InLen, fin);  inNum[InLen] = 0;
  if (!c) startingError(4);	// Did argvt read ok?
  const long int argvt   = strtol(inNum, &endptr, 10);
  //printf ("yume3: fid=%ld,  argct=%ld,  argvt=%ld\n", fid, argct, argvt);
  targv = Ymalloc ((1+argct)*sizeof(char *), __FILE__, __LINE__);
  pargv = Ymalloc (argvt, __FILE__, __LINE__);
  for (i=0; i < argct; ++i) {
    c = fgets (pargv+at, argvt-at, fin);
    targv[i] = pargv+at;
    L = strlen(targv[i]);
    at += L;
    pargv[at-1] = 0;
    //printf ("yume3: %2d.  L%-2d  <%s>  c:%s\n", i, L, targv[i], c);
  }
  //printf ("yume3: Start Phase I.\n");

  y3pid = getpid();
  snprintf (fpidTitle, fpidTiSize, "yume3 %d", y3pid);

  // Construct paths & names of default resource files
#define grcDefName "/.yume-gtkrc"
#define yrcDefName "/.yume-rc"
  const char *home = getenv("HOME");
  if (home) {
    grcFileName = Ymalloc (strlen(home) + strlen(grcDefName), __FILE__, __LINE__);
    strcpy (grcFileName, home);
    strcat (grcFileName, grcDefName); // Default gtk resource file
    yrcFileName = Ymalloc (strlen(home) + strlen(yrcDefName), __FILE__, __LINE__);
    strcpy (yrcFileName, home);
    strcat (yrcFileName, yrcDefName); // Default yume resource file
  } else
    grcFileName = yrcFileName = NULL;

  // Phase I.  Build ListItems items[].
  process_command_options(argct, targv); // Parse parameters of yume
  if(vv[VVShoMo] & ShoParProc)
    printf ("\nPhase I (process parameters) is done\n");

  // Phase II.  Build windows structure from items[]
  Phase2 ();
  if(vv[VVShoMo] & ShoParProc)
    printf ("\nPhase II (make display) is done\n");

  // Phase III.  Run event loop & process events via callbacks
  gtk_main();
  if(vv[VVShoMo] & ShoParProc)
    printf ("\nPhase III (main loop) is done\n");

  return 0;
}
