/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-p1.c,v 1.6 2011/04/05 19:49:11 j-waldby Exp j-waldby $

  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

  That is, terms of the GNU General Public License version 3
  published by the Free Software Foundation apply to the yume3
  program, which is embodied in source files yume3.c, yume3.h,
  yume3-init.h, and some other yume3* files.
*/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <wordexp.h>
#include <gtk/gtk.h>

#include "yume3.h"
#include "yume3-p1.h"
#include "yume3-p2.h"
#include "yume3-util.h"
//------------------------------------------------
// Variables with file scope and internal linkage:

static int labelsIn = 0;	// Number of labels in stack
static int labelsLim = 32;	// Initial size of label stack
static char **label;		// Button label stack
static ParamMode mode =ModeDo;	// Parameter processing mode
static ParamMode mode2=ModeDo;	// Secondary mode, -do vs -dl
static int bun;			// Current button-in-line #
static char *widthString=0;	// Button-width codes
static int   widthStringLen=0;	// # of button-width codes
static int itemsSiz;		// initial size of items array

//--------------------------------------------
//---------Fix-options Routine----------------
//--------------------------------------------
void fixup_option_codes(int argc, char *argv[]) {
  // Convert yume exact option codes like -pq to -Yq, to avoid
  // conflict with xforms option codes.  Also, convert -geo... to -Yt.

  char *opx[] = {"-la", "-de", "-bd", "-dl", "-ti", 
		 "-in", "-do", "-at", "-bu", "-iv", "-bw", "-ex", 0};
  int i, j;

  for (i=1; i < argc; ++i)
    if (argv[i][0]=='-') {
      if (argv[i][1]=='g' && argv[i][2]=='e' && argv[i][3]=='o') {
	argv[i] = "-Yt";
	continue;
      } else 
	for (j=0; opx[j]; ++j)
	  if (!strcmp(argv[i], opx[j])) {
	    argv[i][1] = 'Y';
	    break;
	  }
    }
}
//--------------------------------------------
//-------Option-Processing Routines-----------
//--------------------------------------------
void process_option_file(char *fn, int mustHave);
//--------------------------------------------
void up_nitems (void) {
  ++nitems;
  if ((nitems+3)*sizeof(ListItem) > itemsSiz) {
    // Double size of items array
    itemsSiz *= 2;
    items = Yrealloc(items, itemsSiz, __FILE__, __LINE__);
  }
}
//--------------------------------------------
int need_option (char *sw, char *item, int L) {
  if (item) return 2;
  errorExit (YERR_MISSARG, __FILE__, L, 2, 0,
	     "Switch argument is missing for", sw);
  return YERR_MISSARG;		// avoid C warning
}
//--------------------------------------------
void pushLabel (char *L, int n) {
  char *B = NULL;
  // Push label L while fewer than n labels are on stack
  //printf ("pL <%s> %d\n", L, n);
  while (labelsIn < n) {
    if (!B) B = setTxtBuf(L);	    // Make copy in text buffer
    label[labelsIn] = B;	    // Push label on stack
    if (++labelsIn > labelsLim-2) { // Need to grow the stack?
      // Double label stack size
      labelsLim *= 2;
      label = Yrealloc(label, labelsLim * sizeof(char*), __FILE__, __LINE__);
    }
  }
}
//--------------------------------------------
/* Make an entry of type et.  Set width units per Size notes.
  Parameters:
  	etb,   Base EntryType value;
	etdex, 0 or 1 for initial state of Push or Roll mode 
	valu,  command string;
	labels Lb and La for Push and Roll modes, resp.

  Labels Lb and La get formatted via wraps[etb] and wraps[eta],
  respectively.  In phase II, make_objects will create a GtkLabel for
  each label, if applicable.  (If Lb or La is null it's NA.)

  Size notes: specSiz[] contains default widths for the various et-
  EntryType values.  Widths other than 1 are used directly as button
  widths.  (Width 0 means no button.)  For items with width-spec = 1,
  the larger number of label characters in Lb or La is divided by
  number of buQ, a number which defaults to 5.  For example, a button
  labeled 'Serendipity' (11 letters) is allocated 3 units, and one
  labeled 'Lorelei' (7 letters) is allocated 2.  -bw settings override
  specSiz settings and buQ calculations.  In a row with a total width
  of t units, a unit-width button is given 1/t'th of the row.  The
  default value of buQ can be overridden via -de BuQuanta:n, where n
  is a number.  Eg: -de buQ:6 sets Q to 6.  */

int make_entry (EntryType etb, int etdex, char *valu, char *Lb, char *La) {

  static int ShoHeading=1;
  // SpecSiz is defined in yume3-trec.h by yume3-init
  int LaLen = La? strlen(La) : 0;
  int LbLen = Lb? strlen(Lb) : 0;
  int LL = LaLen > LbLen? LaLen : LbLen;
  ListItem *m = items+nitems;
  int w=SpecSiz[etb];		  // Get default spec width
  if (w && widthStringLen)	{ // 0-width items not relevant
    char b = (bun <= widthStringLen)?
      widthString[bun-1] :	// Get specified letter of width codes
      widthString[widthStringLen-1]; // Last letter repeats indefinitely
    w = 31 & b;			     // Set -bw user-specified width
  } else {
    if (w==1) {			// Scale button by string length?
      w *= (vv[VVbuq]-1+LL)/vv[VVbuq]; // Q = vv[VVbuq]
      if (w<1) w = 1;		       // Require positive width
    }
  }
  m->w = w;			// Set width for addup_sizes() to see
  m->etb   = etb;
  m->etdex = etdex;
  m->value = setTxtBuf(valu);	// Save copy for make_objects()
  m->xL = m->xR = 0;		// y3p2 fills these in
  m->label[0] = Lb? wrapLabel (wraps[etb], Lb, __LINE__) : 0;
  m->label[1] = La? wrapLabel (wraps[etb], La, __LINE__) : 0;

  if(vv[VVShoMo] & ShoParProc) { // -Y2 display
    if (ShoHeading)
      printf ("\nItem# ETyp..........   B#  width  <Labels> and [Command]\n");
    ShoHeading = 0;
    printf ("%3d.  et%2d.%d %-7s   %2d  w%3d  <%s> <%s> [%s]\n",
	    nitems, etb, etdex, ETNames[etb+etdex],
	    bun, w, La?La:"", Lb?Lb:"", valu?valu:"");
  }
  ++bun;			// Advance button-in-row #
  up_nitems ();			// Advance item-in-form #
  return nitems-1;
}
//--------------------------------------------
void startRow (ParamMode modeX, ParamMode modeY, ParamMode modeZ) {
  if (mode != modeX) {
    bun = 0;
    make_entry (RoList, 0, 0, 0, 0); // Start a row list
  }
  mode  = modeY;	// Set parameter state
  mode2 = modeZ;	// Set secondary state
}
//--------------------------------------------
int process_option_pair(int parNumber, char *ai, char *aj) {
  /*
   Process an option singleton or an option pair
   Params:
      ai   = current parameter, like argv[parNumber]
      aj   = next parameter or NULL if none

   Within switch (parcode) { ... }, ai is a dash-code, like
   -bu, -do, etc,  while  aj is either another dash-code or
   the value required by the ai dash-code.

   Return:
      # program params used; 1 if singleton option, else 2
  */
  int used=1;			// Default to 1 param used up
  char *ivcom, *ivnam, *ivlab, parcode=ai[2], *aLabel, *bLabel;
  static int ShoHeading=1;

  if (ShoMode & ShoParProc) {	// Check the ShoParProc bit
    if (ShoHeading)
      printf ("\n         Item#   Par# \tPar \tPar\n");
    ShoHeading = 0;
    printf ("ParProc  %3d.   %3d \t<%s> \t<%s>\n", nitems, parNumber, ai, aj);
  }

  // Look for Y[aefghilnotuvwx] and Y<d>, d a digit
  if (ai[0] == '-' && ai[1] == 'Y' && ai[3] == 0) {
    // Now ai is a dash-code and aj is a required value, if needed
    switch (parcode) {
    case 'a':			// -la label, add a label to labelstack
      used = need_option (ai, aj, __LINE__); // Need a label string
      pushLabel (aj, labelsIn+1);	     // Push 1 copy onto stack
      break;
    case 'e': 			// -de, override a default
      used = need_option (ai, aj, __LINE__); // Need a value string
      // Call decode_default() to get value and put it into vv array, etc
      if (decode_default (aj))	// Reports 0 on success
	errorExit (YERR_DECODE, __FILE__, __LINE__, 2, 0,
		   "Lookup error in ", aj);
      break;
    case 'i':			// -ti title
      used = need_option (ai, aj, __LINE__); // Need a title string
      formTitle = setTxtBuf(aj);	     // Copy to text buffer
      break;	
    case 'l':			// -dl
      startRow (ModeBu, ModeBu, ModeDl); // Enable Bu-Dl line mode
      break;
    case 'n':			// -ti
      used = need_option (ai, aj, __LINE__); // Need a filename string
      process_option_file(aj, 1); // Read options from file
      break;
    case 'o':			// -do
      startRow (ModeNu, ModeDo, ModeDo); // Enable Do-Do mode
      break;
    case 't':				 // -at
      used = need_option (ai, aj, __LINE__); // Need a geometry spec
      atPar = setTxtBuf(aj);	// atPar in txtBuf is copy of argv[]
      hasAt = 1;
      break;
    case 'u':			// -bu
      startRow (ModeNu, ModeBu, ModeBu); // Enable Bu-Bu mode
      break;

    case 'v':			// -iv  -- Make label and edit-box
      used = need_option (ai, aj, __LINE__); // Need an initial value
      startRow (ModeBu, ModeBu, ModeBu); // Enable Bu-Bu mode
      pushLabel (aj, 1);	 // Require at least 1 label on stack
      ivnam = label[--labelsIn]; // Pull iv shell var name
      pushLabel (ivnam, 1);	 // Require at least 1 label on stack
      ivlab = label[--labelsIn]; // Pull iv button label
      // If stack is empty, use command "true" 
      ivcom = labelsIn? label[--labelsIn] : "true"; // Pull iv opt. command
      make_entry (InitBu, vv[VVib]==0, ivcom, ivlab, ivlab); // -iv button
      items[nitems-1].pal = nitems;		 // Link to next item
      make_entry (InitVal, 0, aj, ivnam, ivnam); // -iv entry box + shell var name 
      items[nitems-1].pal = nitems-2; 		 // Link to previous item
      break;

    case 'w':			// -bw, button-width setting
      used = need_option (ai, aj, __LINE__); // Need list of widths
      widthString = setTxtBuf(aj); // make a copy of argv[] aj;
      break;
    case 'x':				 // -ex, exit button
      startRow (ModeBu, ModeBu, ModeBu); // Enable Bu-Bu mode
      aLabel = "exit";  bLabel = "Exit";
      if (labelsIn)			 // Is  label available on stack?
	aLabel = bLabel = label[--labelsIn]; // Yes, pull it from stack
      make_entry (Exitor, vv[VVxb]==0, NULL, aLabel, bLabel);
      break;
    default:			// Is it a digit?
      if (parcode >= '0' && parcode <= '9') vv[VVShoMo] = parcode & 0xF;
      break;
    }
  } else {
    // Current param is not an option code, so it is button or do-group
    int ro = (mode2 == ModeDl);	// Set ro=1 if doing -dl vs -do.
    int dat=-1;
    #define QQ printf ("y3p1  L%3d  P#%2d  ni %2d  dat %2d   ai<%s>  aj<%s>\n",\
		       __LINE__, parNumber, nitems, dat, ai, aj)
    if (mode == ModeDo || ro) { // do-group
      startRow  (ModeBu,  mode, mode2); // In Do-Do mode, EE & do: are a row
      dat = nitems;
      make_entry (DoEE,   vv[VVea], NULL, "EE", "EE");	// Make 'EE' button
      make_entry (DoPush, vv[VVda], NULL, "do:", "Do:"); // Make 'do:' button
      startRow  (ModeBu,  mode, mode2); // In Do-Do mode, edit box is a row
      make_entry (DoTxt, 0, ai, NULL, NULL); // Make entry box
      items[dat+1].pal =	   // Record entry item# in do: button
        items[dat].pal = nitems-1; // Record entry item# in EE: button
    } else { // button
      pushLabel (ai, 1);	// Require at least 1 label on stack
      aLabel = bLabel = label[--labelsIn]; // Pull label from stack
      make_entry (BuCur, vv[VVba], ai, aLabel, bLabel); // Make button of current kind
    }
  }
  return used;
}
//--------------------------------------------
void process_option_array(int argc, char *argv[]) {
  // Go thru param list, finding option codes and their values
  // and build field list.
  int i;
  for (i=1; i<argc-1; )
    i += process_option_pair(i, argv[i], argv[i+1]);
  if (argc > i) process_option_pair(i, argv[i], NULL);
}
//--------------------------------------------
void process_option_file(char *fn, int mustHave) {
  // Go thru param list from a file, finding option codes
  // and their values and build field list.

  // process_option_file will allocate memory to store text
  // contents of file, and memory to store the split up arguments.
  // After wordexp, we can free up ptext, to get memory use
  // down before any forks occur.

  int i;
  FILE *fi = fn? fopen (fn, "r") : NULL;
  struct stat buf;
  wordexp_t wep;
  int s = fn? stat (fn, &buf) : 1;
  char *ptext, *c, *e, *t;

  // If file is required, but isn't ok, give an error.
  if (s || !fi || inDepth >= vv[VVfsl]) {
    //printf ("", s, fi, inDepth, vv[VVfsl]);
    if (mustHave)		// File required?
      errorExit (YERR_FILE, __FILE__, __LINE__, 2, 0,
		 "Unable to open file", fn);
    return; // File not found, but not required
  }
  ++inDepth;			// Add 1 to include-depth
  c = ptext = Ymalloc (1+buf.st_size, __FILE__, __LINE__);
  e = c+buf.st_size;

  // Slurp file (of up to buf.st_size bytes) into ptext:
  do {
    // Read one line from fi to location c
    t = fgets (c, e-c, fi);	// e-c = # of bytes left in ptext
    if (t) {
      if (c[0]=='#')
	c[0] = 0;		// Suppress #comment lines 
      else {
	s = strlen (c);		// Suppress \n and \ at end of line:
	if (c[s-1]=='\n') {
	  c[s-1] = ' ';		// -- Convert newline to space
	  if (c[s-2]=='\\')
	    c[s-2] = 0;		// -- Convert \ at end to null
	}
      }
    }
    c += strlen(c);		// Advance to end of string
  } while (t);
  fclose (fi);			// Done with file, close it

  // wordexp does word expansion like a (posix) shell
  i = wordexp(ptext, &wep, 0);
  if (i)
    errorExit (YERR_WEXP, __FILE__, __LINE__, 1, 1, "wordexp failure", i);
  free (ptext);			// Free file text string

  // Show included file contents if ShoMode & ShoWExp are on.
  if (vv[VVShoMo] & ShoWExp)
    for (i=0; i<wep.we_wordc; ++i)
      printf ("wep wordv %3d. <%s>\n", i, wep.we_wordv[i]);

  // The next two routines skip element [0], so need -1 offset
  fixup_option_codes(wep.we_wordc+1, wep.we_wordv-1);
  process_option_array (wep.we_wordc+1, wep.we_wordv-1);
  wordfree(&wep);		// Free the expanded-values area
  --inDepth;
}
//--------------------------------------------
void process_command_options(int argc, char *argv[]) {
  initWidgetWraps();	// Initialize widget style wrappers
  itemsSiz = 128*sizeof(ListItem); // Initial size of items
  items = Ymalloc(itemsSiz, __FILE__, __LINE__);
  nitems = 0;
  memset (items, 0, itemsSiz);
  fixup_option_codes(argc, argv); // Remove xforms-param-names conflicts

  label = Ymalloc(labelsLim * sizeof (char*), __FILE__, __LINE__);
  BuCur = BuGen;		/* Set default button type */

  // If default yume-resource file exists, load it up.
  process_option_file(yrcFileName, 0); // Not a required file

  // Process command-line options
  process_option_array (argc, argv);
  free (label);		       // Free up label stack
}
//--------------------------------------------
