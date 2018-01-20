/*  $Id: yume3-init.c,v 1.2 2011/04/05 19:52:47 j-waldby Exp j-waldby $

    Program yume3-init creates yume3-tre*.h, for inclusion by
    yume3-util.c to define Tre- search structures, VVdata struct,
    dedata[] array, and related constants. Generated arrays are used to
    find target strings and associated values when processing yume3's
    -de "t:v" options.  Below, "left side values" refers to t names, and
    "right side values" refers to names that can appear in v.

    Copyright 2011 James Waldby.  Offered without warranty under GPL v3
    terms as at http://www.gnu.org/licenses/gpl.html.  Terms of the GNU
    General Public License version 3 published by the Free Software
    Foundation apply to yume3 programs embodied in yume3*[ch] files.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define z fprintf(stderr,"%s@%3d\n",__FILE__,__LINE__);

//===========================================================
// Make enum for Decoder Values
#define VVLIST  VV(DVzna), VV(DVini), VV(DVatt), VV(DVgrc), \
    VV(DVgtk),  VV(DVona), VV(DVonb), VV(DVww),  VV(DV_DV)
// Assign DV values
#define VV(x) x
enum { VVLIST };
#undef VV
// Make array of names of  Decoders
#define VV(x) #x
char *DDnames[] = { VVLIST };
#undef VV
#undef VVLIST
//===========================================================
/*
 * This section specifies Entry-type names, Style Names, Decoder
 * Numbers, relative field sizes, and initial labels for certain
 * button types.
 */
#define VVLIST						\
  /*     ET     Style   Decoder  Rel.  Initial*/	\
  /*    Name    Name       #     Size  Label  */	\
  VV(  DoPush,  y3db,     DVww,   2,  "do:"),		\
    VV(DoRoll,  y3da,     DVww,   2,  "Do:"),		\
    VV(DoEE,    y3eb,     DVww,   1,   "EE"),		\
    VV(DoEA,    y3ea,     DVww,   1,   "EE"),		\
    VV(BuFix,   y3fb,     DVww,   1,   "%s"),		\
    VV(BuFA,    y3fa,     DVww,   1,   "%s"),		\
    VV(BuGen,   y3gb,     DVww,   1,   "%s"),		\
    VV(BuGA,    y3ga,     DVww,   1,   "%s"),		\
    VV(BuHid,   y3hb,     DVww,   1,   "%s"),		\
    VV(BuHA,    y3ha,     DVww,   1,   "%s"),		\
    VV(InitBu,  y3ib,     DVww,   2,   "%s"),		\
    VV(InitBA,  y3ia,     DVww,   2,   "%s"),		\
    VV(Exitor,  y3xb,     DVww,   1, "exit"),		\
    VV(ExitA,   y3xa,     DVww,   1, "Exit"),		\
    /* For next bunch, intoBox() needs et >= DoTxt */\
    VV(DoTxt,   y3td,     DVww,   3,      0),		\
    VV(InitVal, y3tv,     DVww,   3,      0),		\
    VV(RoList,  y3ro,     DVww,   0,      0),		\
    VV(VBox,    y3vb,     DVww,   0,      0),		\
    VV(ET_ET,   eeee,      0,    -1,      0),	       	\
    /* For the rest, field size and label don't matter */\
    VV(VVatt,   attach,   DVatt,  0,      0),		\
    VV(VVbuq,   buQuanta, DVini,  0,      0),		\
    VV(VVfsl,   FStackLim,DVini,  0,      0),		\
    VV(VVgtk,   gtk,      DVgtk,  0,      0),		\
    VV(VVgrc,   gtkrc,    DVgrc,  0,      0),		\
    VV(VVnog,   nogtkrc,  DVini,  0,      0),		\
    VV(VVxys,   XYshow,   DVini,  0,      0),		\
    VV(VVShoMo, ShowMode, DVini,  0,      0),		\
    VV(VVIconi, wIconic,  DVini,  0,      0),		\
    VV(VVOnTop, wOnTop,   DVini,  0,      0),		\
    VV(VVStick, wStick,   DVini,  0,      0),		\
    VV(VVUndec, wUndec,   DVini,  0,      0),		\
    /* Button auto-or-not Values */			\
    VV(VVba,    ba,       DVona,  0,      0),		\
    VV(VVbb,    bb,       DVonb,  0,      0),		\
    VV(VVda,    da,       DVona,  0,      0),		\
    VV(VVdb,    db,       DVonb,  0,      0),		\
    VV(VVea,    ea,       DVona,  0,      0),		\
    VV(VVeb,    eb,       DVonb,  0,      0),		\
    VV(VVia,    ia,       DVona,  0,      0),		\
    VV(VVib,    ib,       DVonb,  0,      0),		\
    VV(VVxa,    xa,       DVona,  0,      0),		\
    VV(VVxb,    xb,       DVonb,  0,      0),		\
    VV(VVbud,   bud,       0,     0,      0),		\
    VV(VVbum,   bum,       0,     0,      0),		\
    VV(VV_VV,   vvvv,      0,    -1,      0)

// ET values are used by all
#define VV(u,v,w,x,y) u
enum { VVLIST };
#undef VV

// ET names are used by yume3-init 
#define VV(u,v,w,x,y) (char*)#u
char *ETNames[] = { VVLIST};
#undef VV

// yume3-p1 et al will use Style names 
#define VV(u,v,w,x,y) #v
char *StyleNames[] = { VVLIST};
#undef VV

// yume3-util will use Decoder #'s
#define VV(u,v,w,x,y) w
int Decoders[] = { VVLIST};
#undef VV

// yume3-p1 make_entry will use default button sizes
#define VV(u,v,w,x,y) x
int   SpecSiz[] = { VVLIST};
#undef VV

// yume3-util wrapLabel will use wrap formats
#define VV(u,v,w,x,y) y
char  *WrapIni[] = { VVLIST};
#undef VV

#undef VVLIST
int vals[VV_VV];

enum { MAXKEYLEN=13, MAXKEYLEN1};
//===========================================================
// Write out VVdata struct and dedata array to trec
void wriData (FILE *fo, int o1, int o2, int o3) {
  char c, *s, txtBuf[MAXKEYLEN1];
  int i, k, L;
  
  // typedef struct { char val, deco; } VVdata;
  // VVdata dedata[...]={  ... };
  fprintf (fo, "typedef struct { char *key, val, deco; } VVdata;\n");
  fprintf (fo, "static VVdata dedata[ET_ET+VV_VV+1]={\n");

  for (i=0; i<VV_VV; ++i) {

    // Copy de-capitalized text into buffer
    s = StyleNames[i];
    L = 1 + strlen(s);
    if (L > MAXKEYLEN) {
      printf ("Error: Quitting because length of %s exceeds %d\n",
	      StyleNames[i], MAXKEYLEN);
      exit(1);
    }
    for (k=0; k<L; ++k)
      txtBuf[k] = tolower(s[k]);

    if (o1) {
      printf ("%2d. %-9s  %-9s %-9s %4d %4d  %-9s %4d\n",
	      i, ETNames[i], StyleNames[i], txtBuf,
	      Decoders[i],SpecSiz[i], WrapIni[i], vals[i]);
    }
    // Compute next field length L
    L = 17-L-strlen(ETNames[i]); // 17 exceeds MAXKEYLEN.
    c = i<VV_VV-1? ',' : ' ';	// Comma or space
    fprintf (fo, " { \"%s\"%-*c%s, %5s }%c",
	     txtBuf, L, ',', ETNames[i], DDnames[Decoders[i]], c);
    if (i&1) fprintf (fo, "\n");
  }
  fprintf (fo, "};\n\n");
}
//===========================================================
// Write array decls & defs into yume3-trec.h
void wriDeco (FILE *fo) {
  int i;

  // Init vals[]
  for (i=0; i< VV_VV; ++i)
    vals[i] = i>ET_ET? i-ET_ET-1 : i;

  // const int SpecSiz[] = { 2, 2, 3, 1, ... 2, 3 };
  fprintf (fo, "// In Global Scope...\nconst int SpecSiz[] = {");
  for (i=0; i<ET_ET; ++i)
    fprintf (fo, "%d,", SpecSiz[i]);
  fprintf (fo, "};\n");

  // char *WrapIni[] = {"do:","Do:",0, ... "%s", 0, 0};
  fprintf (fo, "\nchar *WrapIni[] = {");
  for (i=0; i<ET_ET; ++i)
    if (WrapIni[i])
      fprintf (fo, "\"%s\",", WrapIni[i]);
    else 
      fprintf (fo, "0%s", i<ET_ET-1? ",":"");
  fprintf (fo, "};\n\n");

  // const char *ETNames[] = {"DoPush", "DoRoll", ...};
  fprintf (fo, "const char *ETNames[] = {\n\t");
  for (i=0; i<ET_ET; ++i) {
    fprintf (fo, "\"%s\", ", ETNames[i]);
    if (i%6==5) fprintf (fo, "\n\t");
  }
  fprintf (fo, "\"%s\" };\n\n", ETNames[i]);

  // const char *SNtag[]={ "y3db", "y3da", ... };
  fprintf (fo, "const char *SNtag[]={   // Style Names\n\t");
  for (i=0; i<ET_ET; ++i) {
    fprintf (fo, "\"%s\",", StyleNames[i]);
    if (i%6==5 && i< ET_ET-1) fprintf (fo, "\n\t");
  }
  fprintf (fo, "};\n\n");

  // enum { DVini, DVmsn, DVbud, DVgtk, DVgrc,  };
  fprintf (fo, "// In File Scope...\nenum { ");

  for (i=0; i<DV_DV; ++i) {
    fprintf (fo, "%s, ", DDnames[i]);
  }
  fprintf (fo, " };\n\n");

  fflush (fo);
}
//===========================================================
// Write various enums into yume3-treh.h
void wriTreh (FILE *fo) {
  int i;

  // Show -de symbols in a /* ... */  comment
  fprintf (fo, "/* Symbols for  -de t:v  use include:\n\n");
  for (i=0; i<VV_VV; ++i) {
    if (SpecSiz[i] >= 0)
      fprintf (fo, "%9s, ", StyleNames[i]);
    if (i%6==5 && i<VV_VV-1) fprintf (fo, "\n");
  }
  fprintf (fo, "\n*/\n");

  // typedef enum { DoPush,    DoRoll, ... ET_ET } EntryType;
  fprintf (fo, "typedef enum {\n\t");
  for (i=0; i<ET_ET; ++i) {
    fprintf (fo, "%9s, ", ETNames[i]);
    if (i%4==3 && i<ET_ET-1) fprintf (fo, "\n\t");
  }
  fprintf (fo, "%9s } EntryType;\n\n", ETNames[i]);

  // enum { VVatt,   VVbuq, ...  VVbum, VV_VV };
  fprintf (fo, "enum {  ");
  for (i=ET_ET+1; i<VV_VV; ++i) {
    fprintf (fo, "%7s, ", ETNames[i]);
    if (i%5==3 && i<VV_VV-1) fprintf (fo, "\n\t");
  }
  fprintf (fo, "%s };\n\n", ETNames[i]);

  // enum { MAXKEYLEN=19, MAXKEYLEN1};
  fprintf (fo, "enum { MAXKEYLEN=%d, MAXKEYLEN1};\n", MAXKEYLEN);

  fflush (fo);
}
//===========================================================
int main(int argc, char *argv[]) {
  int loop, par=0;
  time_t filetimeSec=time(NULL);
  char *filetime = ctime(&filetimeSec);
  char *trec="yume3-trec.h", *treh="yume3-treh.h";
  FILE *fco=fopen(trec, "w");
  FILE *feo=fopen(treh, "w");

  if (!fco || !feo) {
    printf ("%s.%d: File %s or %s\n"
	    "failed to open for writing\n",
	    __FILE__, __LINE__, trec, treh);
    exit(1);
  }

  // Put creation dates into treh & trec
  fprintf (fco, "// File %s, created by %s on %s\n",
	   trec, __FILE__, filetime);
  fprintf (feo, "// File %s, created by %s on %s\n",
	   treh, __FILE__, filetime);

  // Get loop to control left side debugs.  [Obsolete: roop for right side]
  ++par; loop = 0; if (argc > par) loop = atoi(argv[par]);
  //++par; roop = 0; if (argc > par) roop = atoi(argv[par]);

  // Write headers in yume3-treh.h
  wriTreh (feo);

  // Write some array decls & defs in yume3-trec.h
  wriDeco (fco);

  // De-capitalize and write data.  Output control bits are:
  //  1: show raw & decapped input.  2: na  4: na
  wriData (fco, loop&1, loop&2, loop&4);

  fclose (fco);
  fclose  (feo);

  return 0;
}
//===========================================================
