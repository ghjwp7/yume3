/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-p2.h,v 1.2 2011/03/05 20:45:11 j-waldby Exp j-waldby $
  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html


yume3-p2 comprises make_objects() and related routines.

make_objects() does Phase 2 of yume3 processing.  It sets up Gtk
objects for button and field data per ItemList items[].  It also sets
up event-processing callbacks to handle mouse button clicks and to
execute commands per menu contents.

make_objects() returns a pointer to the top-level-window widget that
contains the menu structure.

*/

void Phase2 (void);
