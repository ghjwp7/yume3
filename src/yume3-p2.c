/*  yume3 is by James Waldby, Feb 2011
    $Id: yume3-p2.c,v 1.6 2011/04/05 22:12:32 j-waldby Exp j-waldby $

    Copyright 2009-2011 James Waldby.  Offered without warranty
    under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

    That is, terms of the GNU General Public License version 3
    published by the Free Software Foundation apply to the yume3
    program, which is embodied in source files yume3.c, yume3.h,
    yume3-init.h, and some other yume3* files.

    yume3-p2: Window- or widget-sizing phase

*/

#include <wordexp.h>
#include <gtk/gtk.h>
#include "yume3.h"
#include "yume3-p2.h"
#include "yume3-p3.h"
#include "yume3-util.h"

//------------------------------------------------------
// Helper function:  Show current item before error exit
//------------------------------------------------------
void showItem (int i) {
  ListItem *m = items+i;
  fprintf (stderr,
	   "At item# %d, type %-7s, labels <%s> <%s>, value [%s], width %d:",
	   i, ETNames[m->etb + m->etdex],
	   m->label[0], m->label[1], m->value, m->w);
}
//--------------------------------------------
//-------Compute width of each element--------
//--------------------------------------------
void addup_sizes () {
  /* For each row, record in its Rolist entry the row's total of size
     units.  Compute left and right column attach numbers for each
     display item.  Recall that process_command_options() stored spec
     sizes or -bw sizes in each item's ListItem data structure.
  */
  int i, ro=-1, roSiz=0;

  for (i=0; i < nitems; ++i) {
    if (items[i].etb == RoList) { // Front-of-row?
      if (ro >= 0)
	items[ro].w = roSiz;
      roSiz = 0;		// Zero roSiz before row
      ro = i;			// Remember RoList location
    } else {			// Element-of-row
      items[i].xL = roSiz;	// Set left and right attach #'s
      items[i].xR = roSiz+items[i].w;
      roSiz += items[i].w;	// Add width to total
      if (items[i].w<1) {
	showItem (i);
	errorExit (YERR_GEOM, __FILE__, __LINE__, 1, 1, 
		   "Bad width of", items[i].w);
      }
    }
  }
  if (ro >= 0)			// Save last-row data
    items[ro].w = roSiz;
}
//--------------------------------------------
//--------------------------------------------
// Routine to pack item into table.
GtkWidget * intoBox(int inx, GtkWidget *item, GtkWidget *htab) {
  enum { HOpts = GTK_EXPAND|GTK_SHRINK|GTK_FILL };
  enum { VOpts = GTK_EXPAND|GTK_SHRINK|GTK_FILL };
  ListItem *m = items + inx;
  int dex = m->etdex;
  m->obj = item;		// Add widget to ListItem
  gtk_widget_set_name(item, SNtag[m->etb + m->etdex]);	// Set name

  // Add widget to line of menu
  gtk_table_attach (GTK_TABLE(htab), item, m->xL,
		    m->xR, 0, 1, HOpts, VOpts, 0,0);

  // Markup label text and add it to widget
  if (m->label[dex] && m->etb < DoTxt) {
    m->glabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(m->glabel), m->label[dex]);
    // Install label for current selection
    gtk_container_add(GTK_CONTAINER(item), m->glabel);
  } else
    m->glabel = NULL;

  // Debug print ?
  if (0) printf ("iBox i#%2d et%2d.%d %-7s %s L%d R%d  <%s> <%s>\n",
		 inx, m->etb, m->etdex,
		 ETNames[m->etb + m->etdex],
		 SNtag[m->etb + m->etdex], m->xL, m->xR,
		 m->label[0], m->label[1]);
  return item;
}
//--------------------------------------------
//----------Make-the-form Routine-------------
//--------------------------------------------
GtkWindow *make_objects (GtkWindow *form, GtkWidget *vbox) {
  // Create a form that is w wide, h high,
  // with an object created for each element of items[]
  int inx;
  GtkWidget *obj, *htab=NULL;

  // Loop: Add an object for each item in items[]
  for (inx=0; inx<nitems; ++inx) {
    ListItem *m = items + inx;
    void *CBA =			// Callbacks for enter-buttons
      (m->etb == Exitor)? CB_exiA :
      (m->etb == InitBu)? CB_IVA : CB_BA;
    void *CBB =			// Callbacks for click-buttons
      (m->etb == Exitor)? CB_exit :
      (m->etb == InitBu)? CB_IV : CB_Bu;

    switch (m->etb) {
      //.................   Text entry fields
    case DoTxt:
    case InitVal:
      obj = intoBox (inx, gtk_entry_new (), htab);
      gtk_entry_set_text (GTK_ENTRY (obj), m->value);
      g_signal_connect (G_OBJECT(obj), "changed",
			G_CALLBACK(CB_TextEntry), (gpointer)(items+inx));
      if (m->etb == InitVal) // Set shell variable's initial value
	setenv (items[inx].label[0], // Either label is ok
		items[inx].value, 1); 
      break;

      //.................    -do's 'do:' button
    case DoPush:
      obj = intoBox (inx, gtk_button_new (), htab);
      g_signal_connect (G_OBJECT (obj), "enter",
			G_CALLBACK(CB_DoBe), (gpointer)(items+inx));
      g_signal_connect (G_OBJECT (obj), "clicked",
			G_CALLBACK(CB_DoBc), (gpointer)(items+inx));
      break;

      //.................   -do's 'EE' button 
    case DoEE:
      obj = intoBox (inx, gtk_toggle_button_new (), htab);
      g_signal_connect (G_OBJECT (obj), "toggled",
			G_CALLBACK(CB_DoEE), (gpointer)(items+inx));
      g_signal_connect (G_OBJECT (obj), "enter",
			G_CALLBACK(CB_DoEA), (gpointer)(items+inx));
      break;

      //.................  New row of menu
    case RoList:
      if (m->w) {
	htab = gtk_table_new (1, m->w, TRUE);
	gtk_box_pack_start (GTK_BOX (vbox), htab, TRUE, TRUE, 0);
      }
      break;

      //.................  Click buttons:
    case InitBu:		// -iv button
    case Exitor:		// -ex button
    case BuFix:			// -bu buttons
    case BuGen:
    case BuHid:      
      obj = intoBox (inx, gtk_button_new(), htab);
      g_signal_connect (G_OBJECT (obj), "enter",
			G_CALLBACK(CBA), (gpointer)(items+inx));
      g_signal_connect (G_OBJECT (obj), "clicked",
			G_CALLBACK(CBB), (gpointer)(items+inx));
      break;
      //.................   Other item types
    default:
      showItem (inx);
      errorExit (YERR_BADET, __FILE__, __LINE__, 1, 3, 
		 "Wrong entry type.  type & dex & inx are ", m->etb, m->etdex, inx);
    }

    // Show added object if ShoAddObj is on
    if (vv[VVShoMo] & ShoAddObj) {
      ListItem s = items[inx];
      static int ShoHeading=1;
      if (ShoHeading)
	fprintf (stdout,"\n      Item#  ETyp........   Pal  w  xL  xR  <Labels> and [Command]\n");
      ShoHeading = 0;
      fprintf (stdout,"AddObj  %3d. %2d.%d %-7s  %3d %3d %3d %3d  <%s><%s>[%s]\n",
	       inx, s.etb, s.etdex, ETNames[s.etb+s.etdex], s.pal, s.w,
	       s.xL, s.xR, s.label[0]?s.label[0]:"", s.label[1]?s.label[1]:"",
	       s.value?s.value:"");
      fflush (stdout);
    }
  }
  return form;
}
//--------------------------------------------
// Following should run after window manager places the menu
gboolean XYtestCoords(void *data) {
  GdkScreen *gds;
  int  h, w, x, y;
  gtk_window_get_position(GTK_WINDOW(data), &x, &y);
  gds = gdk_screen_get_default ();
  w = gdk_screen_get_width (gds);
  h = gdk_screen_get_height (gds);
  if (vv[VVxys] > 1 || x<0 || y<0 || x>w || y>h)
    printf ("yume3 menu for pid %d appears at %d %d\n", y3pid, x, y);
  return FALSE;			// Terminate g_timeout
}
//--------------------------------------------
void Phase2 (void) {
  GtkWindow *form;
  GtkWidget *vbox;
  gboolean g;

  /* First, treat gtk, gtkrc, and nogtkrc options that may have been
     set in Phase I.  Add user's .yume3-gtkrc to list of files to be
     parsed by gtk_init(), unless we were told to not do so; and add a
     file to the list of files to be parsed at the end of gtk_init(), if
     we were told to add a file.  */
  if (!vv[VVnog] && grcFileName) // Has -de nogtkrc occurred?
    gtk_rc_add_default_file(grcFileName);

  if(0)printf ("\n%5d.  re nogtkrc vs %s : %d, %d,  %d = %d && %d\n",
	  __LINE__, grcFileName, VVnog, vv[VVnog], 
	  !vv[VVnog] && grcFileName, !vv[VVnog], 1 && grcFileName);

  if (gtkrc)			// Has -de gtkrc occurred?
    gtk_rc_add_default_file(gtkrc);

  if(0)printf ("re gtkrc vs %s : %d\n", gtkrc, 1 && gtkrc);

  // Compute relative widths in form
  addup_sizes ();

  // Let gtk_init process any gtk options user has specified.
  // wordexp does word expansion like a (posix) shell.
  if (gtkPar) {
    wordexp_t wep;
    int a1, i=wordexp(gtkPar, &wep, 0); 

    if (i) {
      //showItem (inx);
      errorExit (YERR_WEXP, __FILE__, __LINE__, 2, 1,
		      "wordexp failure: ", gtkPar, i);
    }
    a1 = (int)(wep.we_wordc);
    gtk_init (&a1, &(wep.we_wordv)); // Give args to gtk_init
  } else
    gtk_init (NULL,NULL); // Give no args to gtk_init

  // Start form and set some of its display properties
  form = (GtkWindow*)gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (G_OBJECT(form), "destroy",
                    G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_decorated(form, !vv[VVUndec]); // Decorated or not
  gtk_window_set_keep_above(form, vv[VVOnTop]); // Keep-Above or not
  if (vv[VVStick]) gtk_window_stick(form);	// Stick or not
  if (vv[VVIconi]) gtk_window_iconify(form);	// Iconic or not

  // Make main vbox and add to menu
  vbox = gtk_vbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (form), vbox);
  // Populate form with an object for each items[] element
  form = make_objects (form, vbox);

  // Treat -at and -ti options.
  if (atPar) {			// Set geometry, if given by -at
    g = gtk_window_parse_geometry ((GtkWindow*)form, atPar);
    if (!g) errorExit (YERR_GEOM, __FILE__, __LINE__, 3, 0,
		       "Geometry code", atPar, "not recognized");
  }
  gtk_widget_show_all ((GtkWidget*)form); // Make all widgets visible
  if (formTitle)		// Set title, if given by -ti
    gtk_window_set_title ((GtkWindow*)form, formTitle);
  else 
    gtk_window_set_title (form, fpidTitle);

  // Schedule a test for menu-on-screen, a few ms from now
  if (vv[VVxys])
    g_timeout_add(350, XYtestCoords, form);
}
//--------------------------------------------
