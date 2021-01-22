// File yume3-treh.h, created by ./src/yume3-init.c on Mon Mar 25 15:58:05 2019

/* Symbols for  -de t:v  use include:

     y3db,      y3da,      y3eb,      y3ea,      y3fb,      y3fa, 
     y3gb,      y3ga,      y3hb,      y3ha,      y3ib,      y3ia, 
     y3xb,      y3xa,      y3td,      y3tv,      y3ro,      y3vb, 
   attach,  buQuanta, FStackLim,       gtk,     gtkrc, 
  nogtkrc,    XYshow,  ShowMode,   wIconic,    wOnTop,    wStick, 
   wUndec,        ba,        bb,        da,        db,        ea, 
       eb,        ia,        ib,        xa,        xb,       bud, 
      bum, 
*/
typedef enum {
	   DoPush,    DoRoll,      DoEE,      DoEA, 
	    BuFix,      BuFA,     BuGen,      BuGA, 
	    BuHid,      BuHA,    InitBu,    InitBA, 
	   Exitor,     ExitA,     DoTxt,   InitVal, 
	   RoList,      VBox,     ET_ET } EntryType;

enum {    VVatt,   VVbuq,   VVfsl,   VVgtk,   VVgrc, 
	  VVnog,   VVxys, VVShoMo, VVIconi, VVOnTop, 
	VVStick, VVUndec,    VVba,    VVbb,    VVda, 
	   VVdb,    VVea,    VVeb,    VVia,    VVib, 
	   VVxa,    VVxb,   VVbud,   VVbum, VV_VV };

enum { MAXKEYLEN=13, MAXKEYLEN1};
