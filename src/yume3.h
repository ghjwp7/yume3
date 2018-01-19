/* yume3.h --  By James Waldby, Feb 2011
$Id: yume3.h,v 1.5 2011/04/05 19:54:08 j-waldby Exp j-waldby $
Copyright 2009-2011 James Waldby.  Offered without warranty
under GPL v3 terms as at http://www.gnu.org/licenses/gpl.html
[That is, terms of the GNU General Public License version 3 published
 by the Free Software Foundation apply to the yume3 program, which is
 embodied in source files yume3.c, yume3.h, yume3-init.h, and some
 other yume3* files.]

This file contains typedefs of ErrorCode, ParamMode, EntryType, and
 Sho- enums; typedef for struct ListItem; #defines of WAIT_DELAY,
 deftName, and z; and declarations of global variables.
*/
#ifndef YUME3_STRUCT
#define YUME3_STRUCT

//================ typedefs of enums ===================

#include "yume3-treh.h"		// enum {...} EntryType;

typedef enum {YERR_EXECV=10, YERR_SEGV,   YERR_FILE,  YERR_MALLOC,
	      YERR_MISSARG,  YERR_GEOM,   YERR_WEXP,  YERR_DECODE,
	      YERR_NULLSTR,  YERR_BADSTR, YERR_BADET, YERR_WRAPSiz,
	      YERR_INImV=50, YERR_INIocV, YERR_2INImHx } ErrorCode;

typedef enum { ModeBu,  ModeDo,  ModeDl,  ModeNu }       ParamMode;

enum {ShoWExp=1, ShoParProc=2, ShoAddObj=4, ShoAssert=8};

//================= ListItem struct ====================

typedef struct {
  GtkWidget *obj;	       // The widget of this ListItem
  GtkWidget *glabel;	       // Label widget, a child of obj
  char *label[2];	       // Inactive and active label texts 
  const char *value;	       // gtk_entry_get_text gives const char*
  EntryType etb;	       // base EntryType value
  short etdex;		       // 0 or 1 for Push or Roll mode
  short pal;		       // For do: groups, index of entry field
  short w;		       // Units of width (1,2,3...) for item
  short xL, xR;		       // Left & right table attach column #s
} ListItem;

//==================== #defines ========================

// Time delay in milliseconds, before waitpid() callback
#define WAIT_DELAY 3000
#define deftName "/.yumerc"
// debugging aid z
#define z fprintf(stderr,"%s@%3d\n",__FILE__,__LINE__);

//================ Global variables ====================

extern ListItem *items;		// Array of elements of menu
extern int nitems;		// # of items in menu at moment
extern int inDepth;		// Current nest-depth of includes
extern char *formTitle;		// Title of menu window
extern int ShoMode;		// 3 bits control 3 prints
extern int buHi, doHi;	   // Char heights per process_command_options
extern int ddHi, eeHi;	   // Char heights per process_command_options
extern int doM1, buM1;		// Width of M's in do- & bu-text fonts
extern int hasAt;		// 1 if -at appeared with > 2 terms 
extern int hasSize;		// 1 if -at appeared
extern int xat, yat, wat, hat;	// Form-location per -at param
extern int y3pid;		// yume3 process id #
extern EntryType BuCur;		// Current button type
extern int Aflag;		// assertion-flag
extern unsigned int Schild;	// number of active children
extern unsigned int Echild;	// number of ended children
extern int vv[];		// Default value settings
extern const int SpecSiz[];	// Rel. field sizes; y3-trec def, y3-p1 use
extern char *WrapIni[];		// Initial formats;  y3-trec def, y3-ut use
extern const char *SNtag[];	// Style names;      y3-trec def, y3-p2 use
extern const char *ETNames[];	// Entry type names; y3-trec def, y3-p2 use
extern char *wraps[];		// wrapper pointers
extern char *wrapt[];		// wrapped-string pointers
extern char *atPar;		// -at param value
extern char *gtkrc;		// gtkrc file name
extern char *gtkPar;		// gtk parameter string
extern char *buDefs;		// define special submenu
extern char fpidTitle[];	// Alternate title of menu window
extern char *grcFileName;	// Path to default gtk resource file
extern char *yrcFileName;	// Path to default yume resource file

#endif
