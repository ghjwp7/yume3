/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-p3.h,v 1.4 2011/04/05 19:54:40 j-waldby Exp j-waldby $
  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html

yume3-p3 comprises routines for Phase 3 of yume3 processing.  These
routines include callbacks to handle mouse button clicks and other GUI
events, and routines to execute commands per menu contents.

*/
//void doBLabels_init (void);
void CB_TextEntry (GtkWidget *obj, ListItem *dat);
void CB_DoEE (GtkWidget *obj, ListItem *inx);
void CB_DoEA (GtkWidget *obj, ListItem *inx);
void CB_DoBc (GtkWidget *obj, ListItem *inx);
void CB_DoBe (GtkWidget *obj, ListItem *inx);
void CB_Bu   (GtkWidget *obj, ListItem *inx);
void CB_BA   (GtkWidget *obj, ListItem *dat);
void CB_exit (GtkWidget *obj, ListItem *inx);
void CB_exiA (GtkWidget *obj, ListItem *inx);
void CB_IV   (GtkWidget *obj, ListItem *inx);
void CB_IVA  (GtkWidget *obj, ListItem *dat);
/* end */
