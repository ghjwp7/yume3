/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-util.h,v 1.3 2011/03/05 20:45:11 j-waldby Exp j-waldby $

  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

  That is, terms of the GNU General Public License version 3
  published by the Free Software Foundation apply to the yume3
  program, which is embodied in source files yume3.c, yume3.h,
  yume3-init.h, and some other yume3* files.
*/
#include <stdlib.h>

typedef unsigned char UC;


//================================================

// errorExit (err#, line#, #strings, #ints, strings... ints...)
void errorExit (ErrorCode e, char *F, int L, int nS, int nI, ...);

void malloc_errorExit (char *F, int L, size_t S);

void *Ymalloc(size_t size, char *F, int L);

void *Yrealloc(void *ptr, size_t size, char *F, int L);

// setTxtBuf: Copy txt into txtBuf (ie copy into heap)
char *setTxtBuf(char *txt);

// wrapLabel puts a label in a widget format wrapper.
// initWidgetWraps sets initial values of wrappers.
char *wrapLabel (char *fmt, char *txt, int L);
void initWidgetWraps();

// decode_default:  Decode string oc into an offset and a value.
int decode_default (const char *oc);

//end
