/*  yume3 is by James Waldby, Feb 2011
$Id: yume3-p1.h,v 1.1 2011/03/05 20:45:11 j-waldby Exp j-waldby $
  Copyright 2009-2011 James Waldby.  Offered without warranty
  under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html


process_command_options() does Phase 1 of yume3 processing.

It first tests whether a default-options file .yumerc exists, and if
so loads options from it.

Next, it processes yume3 command-line options as given via argc and
argv.  The main effect of processing options is construction of
ItemList items[], an array containing button and field specifications
that is suitable for input to Phase 2 processing in make_objects().

*/

void process_command_options(int argc, char *argv[]);
