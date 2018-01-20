/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-p3.c,v 1.5 2011/04/05 19:48:31 j-waldby Exp j-waldby $

  Copyright 2009-2016 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

  That is, terms of the GNU General Public License version 3
  published by the Free Software Foundation apply to the yume3
  program, which is embodied in source files yume3.c, yume3.h,
  yume3-init.h, and some other yume3* files.
*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "yume3.h"
#include "yume3-p3.h"
#include "yume3-util.h"

//--------------------------------------------
//-----Static "do:" and "* do: *" labels------
//--------------------------------------------
//static GtkWidget *doClickLab;
//static GtkWidget *doRollLab;

//--------------------------------------------
//--------------Initialization----------------
//--------------------------------------------

// Make "do:" and "* do: *" labels
void doBLabels_init (void) { 
}
//--------------------------------------------
//------------Callback-related Routines-------
//--------------------------------------------
//   If OrFl is 0, require match; clear Aflag.
//   If OrFl is 1, set Aflag to Aflag|match.
//   If OrFl is 2, require (match || Aflag); clear Aflag.
void assertET (EntryType e, int i, int L, int OrFl) {
  EntryType et = items[i].etb;
  if (OrFl==0) Aflag=0;
  if (et == e) {		// Detect match
    Aflag = 1;			// Aflag|match
    if (ShoMode & ShoAssert)	// Check the ShoAssert bit
      printf ("AssertET %3d. %2d %-7s <%s> <%s> [%s] L%03d\n",
	      i, et, ETNames[et], items[i].label[0],
	      items[i].label[1],  items[i].value, L);
  } else {
    if ((OrFl != 1) && !Aflag)
      errorExit (YERR_BADET, __FILE__, L, 1, 3,
		 "Wrong entry type.  Expected, found, & item index =",
		 e, et, i);
  }
  Aflag &= OrFl;		// Leave Aflag clear or set
}
//--------------------------------------------
// waiter -- Callback routine to dispose of children
gboolean waiter (gpointer data) {
  pid_t p;

  do {				// Dispose while children remain
    p = waitpid (0, NULL, WNOHANG);
    if (p > 0) ++Echild;
  } while (p > 0);

  // If children remain to dispose of, continue callbacks
  /* Future: If other waiter's are pending, return false. */
  return Schild != Echild;
}
//--------------------------------------------
// execVal -- execute a value as a command
pid_t execVal (int inx) {
  wordexp_t wep;
  pid_t p=0;

  const char *v = items[inx].value;
  const int ccSiz=99;
  union {	 // item[].value type differs from execv()'s argv type
    const char *ccs[ccSiz+1];	// Consistent with fl_get_input()
    char *const csc[ccSiz+1];	// Consistent with execv's 2nd argument
  } arg;
  int i=0, emode=3, wepa=0;

  // Set up for default case:  Simple command execution
  arg.ccs[i++] = getenv("SHELL");
  if (!arg.ccs[0]) 
    printf ("\nyume3:  error:  getenv(SHELL) was null!\n");
  arg.ccs[i++] = "-c";
  arg.ccs[i++] = v;
  //printf ("execVal: inx=%d  v=%s\n", inx, v);
  // See if some other case (meta-processing)
  if (v[0]=='%') {		// Leading % says meta-process
    arg.ccs[i-1] = v+2;		// Strip off %x; v+2 in question
    switch (v[1]) {		// Interp v+1 (and verify v+2)
    case '#': emode = 0;	// print command, don't execute it
    case '%':			// print command
      printf ("Y Item %2d:  %s\n", inx, v+2);
      break;
    case '+':			// exec without fork
      emode = 2;
      break;
    case '-':			// exec without shell or fork
      emode = 2;
    case ':':			// exec without shell
      wordexp(v+2, &wep, 0);	// v+2 ok
      wepa = 1;
      if (wep.we_wordc > ccSiz)
	wep.we_wordc = ccSiz;	// Limit # of params by ccSiz
      for (i=0; i<wep.we_wordc; ++i) {
	arg.ccs[i] = wep.we_wordv[i];
	if (ShoMode & ShoWExp)	// Check the ShoWExp bit
	  printf ("csc %d : <%s>\n", i, arg.csc[i]);
      }
      break;
    default:
      return 0;			// v+2 bad or %x not recognized
    }
  }

  // Execute command
  arg.ccs[i++] = NULL;		// Null terminate arg list
  if (emode & 2) {
    if (emode & 1)
      p = fork();
    else p = 0;
    if (!p) {			// in child, or not forked
      execv (arg.csc[0],  arg.csc); // execute command
      _exit (YERR_EXECV);	    // exit from child
    }
  }

  // Increment child count and set a callback to dispose of child
  ++Schild;
  g_timeout_add (WAIT_DELAY, waiter, NULL);

  // If wordexp was used, free up wep stuff
  if (wepa) wordfree(&wep);

  return p;
}
//--------------------------------------------
//                Callbacks
//--------------------------------------------
// Relabel a do: button // 
void doBRelabel (ListItem *m) {
  m->etdex = 1 - m->etdex;	// Toggle the type

  // Set new style name and new text of label
  gtk_widget_set_name (m->obj, SNtag[m->etb + m->etdex]);
  gtk_label_set_markup (GTK_LABEL(m->glabel), m->label[m->etdex]);

  if(0)printf ("doBR  dob %2ld  etb %2d.%d %7s <%s> tag %s\n",
	  m-items, m->etb, m->etdex, ETNames[m->etb+m->etdex],
	  m->label[m->etdex], SNtag[m->etb + m->etdex]);
}
//--------------------------------------------
// An EE button *obj has been toggled.
void CB_DoEE (GtkWidget *obj, ListItem *dat) {
  int inx = dat-items, dob = inx+1;
  assertET (DoEE, inx, __LINE__, 0);
  doBRelabel (items+dob);
}
//-------------------------------------------------------
// An EE button *obj has been entered
void CB_DoEA (GtkWidget *obj, ListItem *dat) {
  if (dat->etdex) CB_DoEE (obj, dat);
}
//-------------------------------------------------------
// Handle callback for "do:" button Click-activation mode
void CB_DoBc (GtkWidget *obj, ListItem *dat) {
  int inx=dat-items, pal=items[inx].pal;
  assertET (DoEE,  inx-1, __LINE__, 0);
  assertET (DoTxt, pal,   __LINE__, 0);
  items[pal].value = gtk_entry_get_text
			(GTK_ENTRY (items[pal].obj));
  execVal (pal);
}
//-------------------------------------------------------
// Handle callback for "do:" button in mouseover mode
void CB_DoBe (GtkWidget *obj, ListItem *dat) {
  if (dat->etdex) CB_DoBc (obj, dat);
}
//--------------------------------------------
void CB_Bu  (GtkWidget *obj, ListItem *dat) {
  int inx = dat-items;
  assertET (BuFix, inx, __LINE__, 1);
  assertET (BuGen, inx, __LINE__, 1);
  assertET (BuHid, inx, __LINE__, 2);
  execVal (inx);
}
//--------------------------------------------
void CB_BA  (GtkWidget *obj, ListItem *dat) {
  if (dat->etdex) CB_Bu (obj, dat);
}
//--------------------------------------------
void CB_exit (GtkWidget *obj, ListItem *dat) {
  assertET (Exitor, dat-items, __LINE__, 0);
  if(vv[VVShoMo] & ShoParProc)
    printf ("\nPhase III (main loop) is exiting\n");
  exit(0);
}
//--------------------------------------------
void CB_exiA (GtkWidget *obj, ListItem *dat) {
  if (dat->etdex) CB_exit (obj, dat);
}
//--------------------------------------------
// Process a text-edit-box:
//    Log it (if logging is on) else do nothing.
void CB_TextEntry (GtkWidget *obj, ListItem *dat) {
  int inx = dat-items;
  assertET (InitVal, inx, __LINE__, 1);
  assertET (DoTxt,   inx, __LINE__, 2);
  if (dat->etb == InitVal)
    setenv (items[inx].label[0],
	    gtk_entry_get_text(GTK_ENTRY(items[inx].obj)), 1);
}
//--------------------------------------------
void CB_IV (GtkWidget *obj, ListItem *dat) {
  int inx = dat-items, pal = dat->pal;
  assertET (InitBu, inx, __LINE__, 0);	// InitVal button
  setenv (items[pal].label[0],
	  gtk_entry_get_text(GTK_ENTRY(items[pal].obj)), 1);
  execVal (inx);
}
//--------------------------------------------
void CB_IVA (GtkWidget *obj, ListItem *dat) {
  if (dat->etdex) CB_IV (obj, dat);
}
//--------------------------------------------
