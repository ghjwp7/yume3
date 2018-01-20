/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-util.c,v 1.4 2011/04/05 19:48:56 j-waldby Exp j-waldby $

Contents include:

  errorExit (err#, file, line#, #strings, #ints, strings... ints...)
  int getNextVal (char **s, TreSet *H);
  int decode_default (const char *oc);
  void malloc_errorExit (char *F, int L, size_t S);
  void *Ymalloc(size_t size, char *F, int L);
  void *Yrealloc(void *ptr, size_t size, char *F, int L);

  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

  That is, terms of the GNU General Public License version 3
  published by the Free Software Foundation apply to the yume3
  program, which is embodied in source files yume3.c, yume3.h,
  yume3-init.h, and some other yume3* files.
*/
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
// Note, yume3.h includes yume3-treh.h
#include "yume3.h"
#include "yume3-util.h"
#include "yume3-trec.h"

// Constants for lookups in Tre
enum { TOPDELTA=255, Branch=0x20, CMask=0x7F, Termi=0x80};
// Structure of Tre entries
typedef struct {
  int narr;			// # of array entries
  int *hVal;			// Array of hash values
  short int *ocVal;		// Array of offset or constant values
} TreSet;

//--------------------------------------------
//
// errorExit (err#, fi, line#, #strings, #ints, strings... ints...)
//
// errorExit accepts five required arguments, 
//     e    error number
//     fi   file name where L is
//     L    line number where error was detected
//     nS   number of string arguments
//     nI   number of integer arguments,
// followed by nS string arguments and nI integer arguments.
//
// All string arguments, if any, precede any integer arguments.
//
void errorExit (ErrorCode e, char *F, int L, int nS, int nI, ...) {
  va_list ap;
  int i;
  printf ("\n");		// Write a newline on stdout
  fflush (stdout);	        // Finish output on stdout
  fprintf (stderr, "*** Error %d at F:%s L:%d D:%d  ",
	   e, F, L, inDepth);

  va_start(ap, nI);		// Point at arg after nI
  for (i=0; i < nS; ++i)	// Print nS strings
    fprintf (stderr, " %s", va_arg(ap, char*));
  for (i=0; i < nI; ++i)	// Print nI ints
    fprintf (stderr, " %d", va_arg(ap, int));
  va_end(ap);

  fprintf (stderr, "\n");
  exit (e);
}
//--------------------------------------------
void malloc_errorExit (char *F, int L, size_t S) {
  errorExit (YERR_MALLOC, F, L, 1, 1, "malloc() failed, S:", S);
}
//--------------------------------------------
void *Ymalloc(size_t size, char *F, int L) {
  void *t = malloc(size);
  if (!t) malloc_errorExit(F, L, size);
  return t;
}
//--------------------------------------------
void *Yrealloc(void *ptr, size_t size, char *F, int L) {
  void *t = realloc(ptr, size);
  if (!t) malloc_errorExit(F, L, size);
  return t;
}
//--------------------------------------------
char *setTxtBuf(char *txt) {
  enum { TxtBufSize = 2048 };
  static int txtBufAt = 0;
  static char *txtBuf=NULL;	// malloc'd space for strings
  int L, t=txtBufAt;

  if (!txt) return NULL;
  if (!txtBuf)			// See if init needed
    txtBuf = Ymalloc (TxtBufSize, __FILE__, __LINE__);

  // Alloc more text buffer if necessary
  L = strlen(txt);
  if (t+L+2 > TxtBufSize) {
    txtBuf = Ymalloc (TxtBufSize, __FILE__, __LINE__);
    t = txtBufAt = 0;
    if (L >= TxtBufSize)
      errorExit (YERR_BADSTR, __FILE__, __LINE__, 1, 2,
		 "String exceeds buffer", L, TxtBufSize);
  }
  txtBufAt += L+1;	// account for string + nil
  memcpy (txtBuf+t, txt, L+1);
  //printf ("sTB add %2d chars @ %3d-%3d : <%s>\n", L, t, txtBufAt, txt);
  return  txtBuf+t;
}
//-------------------------------------------------
//---------------------Wrappers--------------------
//-------------------------------------------------
// Apply format to text.  Store result in Text Buffer.
char *wrapLabel (char *fmt, char *txt, int L) {
  enum { BSiz = 1023, BSiz1};
  char wBuf[BSiz1];
  int n;
  n = snprintf (wBuf, BSiz1, fmt, txt);
  if (n > BSiz)
    errorExit (YERR_WRAPSiz, __FILE__, L, 1, 3,
	       "Text length > limit", n, BSiz);
  // Copy formatted txt into txt buffer
  return setTxtBuf(wBuf);
}
//-------------------------------------------------
void setWidgetWrap(int i) {
  // Put wrapped txt into txt buffer
  if (WrapIni[i])
    wrapt[i] = wrapLabel(wraps[i], WrapIni[i], __LINE__);
  else
    wrapt[i] = NULL;
}
//-----------------------------------------------------
void initWidgetWraps() {
  int i;
  for (i=0; i<ET_ET; ++i) {
    wraps[i] = "%s";		// Default wrappers
    setWidgetWrap(i);
  }
}
//-------------------------------------------------
//---------Lookups for -de processing---------
//--------------------------------------------
/*  vvLookup(n, *tre) ---  Search for string n in VVdata table
 */
int vvLookup(char *n, VVdata *H, int nent) {
  char c[MAXKEYLEN1];
  int i, L;

  if (!n) return -1;		// If pointer is null, scat
  L = strlen(n);
  for (i=0; i<=L; ++i)		// copy nul also
    c[i] = tolower(n[i]);

  for (i=0; i<nent; ++i)	// Look for exact match of L chars
    if (strncmp(c, H[i].key, L) == 0)
      return i;

  return -1;			// Return not-found
}
//--------------------------------------------
/* getNextVal(): Look at string of letters or digits after skipping
   whitespace.  If word begins with a digit, treat it as a positive
   decimal number and return numeric value.  Otherwise, lookup word
   via vvLookup().  

   Returns: (a) Sets *s to beginning of next word. (b) Returns numeric
   value or vvLookup() result.  A negative result indicates that an
   error occurred.
*/
int getNextVal (char **s) {
  char *c=*s, *e;
  //printf ("gNV:  s = <%s>\n", *s);
  while (*c==' '||*c=='\t') ++c; // Skip leading spaces or tabs

  e = c;			 // Now get a name string:
  while (isalnum(*e) || (*e == '_')) ++e;
  // Now *c is first character of word, and *e is after last.
  *s = e + (*e != 0);	// Go to next char, but not past nil
  *e = 0;				// Null-terminate the word

  if (isdigit(*c)) {
    int h=0;
    for (; isdigit(*c); ++c) // Evaluate decimal #
	h = 10*h + (*c & 0xF);
    return h;
  }

  return vvLookup (c, dedata, ET_ET+VV_VV);	// Get value from Lookup
}
//--------------------------------------------
/*
  decode_default (char *oc):

  Decode a "t:v" string oc into an item number and a value.  t is a
  target name that will be looked up in TreLVS, while value may be
  numbers, names (looked up in TreRVS), or a string.

  On success, d_d (ie, decode_default) stores numeric results in vv[]
  and string results as dictated by LVS[].code, then returns 0.

  If d_d fails, it returns 1, after which the caller can report an
  error.

  
  As noted above, d_d decodes a "t:v" string, and stores numeric
  and/or string values according to decode keys contained in
  LVS[].code entries.

  After finding t and its index inx in TreRVS, d_d gets deco and val
  from dedata[inx].deco and dedata[inx].val.  deco has a value from
  set { DVini, DVmsn, DVbud, DVgtk, DVgrc }.  The different types of
  decoding are handled via cases within switch (deco) {...}.

  For example, numerics are treated in case DVini, and resulting
  numbers are stored in vv[val].  Allowed numeric forms are "t" or
  "t:v" or "t:v w..."  where v, w, ... are "value words", composed of
  letters, numbers, and underline characters.  A value word can be a
  numeric constant, or the name of a constant defined in KVTre.  Words
  that are looked up in TreRSV (or TreLSV) can be abbreviated to
  shortest distinct forms.  When multiple words appear in Right Side
  Values, their values are OR'd together.  When no words appear (form
  "t") 1 is returned.

  When deco is non-numeric, input is of form "t:s", where s is a
  string.  The string begins with the first character after the ':'
  and continues to the end of the parameter string.

  A space, equal, or other non-letter/non-underline character may be
  used where colons are shown, above.  Spaces or tabs should be used
  between words.  On the left side and in numeric results, white space
  (blanks or tabs) is ignored except for its use to separate words
  from each other.

  Note: decode_default allows up to MaxSubStr characters in its input.

  Note: yume-init creates file yume3-trec.h, containing definitions of
  arrays dedata[], TreRSV[], and TreLSV[], constants Ndedata, NTreRSV,
  and NTreLSV, and enum of DV- constants..
*/
int decode_default (const char *oc) {
  enum { MaxSubStr=1023, MaxSubStr1};
  char s[MaxSubStr1], *e=s, *be=NULL;
  int inx, val, num=1, deco, t;

  if (!oc) errorExit (YERR_NULLSTR, __FILE__, __LINE__, 1, 0,
		      "*** Null string to decode");

  strncpy(s, oc, MaxSubStr);
  s[MaxSubStr] = 0;
  // getNextVal looks up next word, then advances e in string
  inx = getNextVal (&e); // Lookup item key
  if (inx < 0) return 1;	 // Report failure
  deco = dedata[inx].deco;
  val  = dedata[inx].val;	 // Need int for vv[] subscript
  //printf ("d_d: <%s> deco:%3d  val:%3d\n", oc, deco, val);//debug

  // For strings, treat rest as Right Side Values
  if (deco != DVini)
    be = setTxtBuf(e);	// Copy string to heap

  switch (deco) {
  case DVini:			// Numeric type
    while (*e && !isalnum(*e)) ++e; // Skip non-alphanumerics
    if (*e) num = 0;
    while (*e) {
      t = getNextVal (&e); // Lookup value keys
      num |= t;
      if (t<0) return 1;
    }
    //printf ("in DVini case,  deco %d  val %d  num %d\n", deco, val, num);
    if (val < VV_VV)
      vv[val] = num;		      // Store number in vv[]
    else
      errorExit (YERR_DECODE, __FILE__, __LINE__, 2, 1,
		 "Lookup error TL in ", be, val);
    return 0;			      // Report success

  case DVww:			// Widget label wrapper
    wraps[val] = be;
    setWidgetWrap(val);
    return 0;
  case DVatt:			// attach
    printf ("attach: future yume3 option\n");
    exit(0);
    return 0;
  case DVgtk:			// gtk params
    gtkPar = be;
    return 0;
  case DVgrc:			// gtkrc file name
    gtkrc = be;
    return 0;
  case DVona:
    vv[val+0] = 1;		    // Set current
    vv[val+1] = 0;		    // Clear next
    //printf ("DVona set vv %2d, cleared %2d\n", val, val+1);
    return 0;
  case DVonb:
    vv[val-0] = 1;		    // Set current
    vv[val-1] = 0;		    // Clear previous
    //printf ("DVonb set vv %2d, cleared %2d\n", val, val-1);
    return 0;
  default:
    printf ("Oops, No decoder #%d for %s\n", deco, be);
  }
  return 1;
}
