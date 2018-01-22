// File yume3-trec.h, created by ./src/yume3-init.c on Thu Jan 18 21:34:28 2018

// In Global Scope...
const int SpecSiz[] = {2,2,1,1,1,1,1,1,1,1,2,2,1,1,3,3,0,0,};

char *WrapIni[] = {"do:","Do:","EE","EE","%s","%s","%s","%s","%s","%s","%s","%s","exit","Exit",0,0,0,0};

const char *ETNames[] = {
	"DoPush", "DoRoll", "DoEE", "DoEA", "BuFix", "BuFA", 
	"BuGen", "BuGA", "BuHid", "BuHA", "InitBu", "InitBA", 
	"Exitor", "ExitA", "DoTxt", "InitVal", "RoList", "VBox", 
	"ET_ET" };

const char *SNtag[]={   // Style Names
	"y3db","y3da","y3eb","y3ea","y3fb","y3fa",
	"y3gb","y3ga","y3hb","y3ha","y3ib","y3ia",
	"y3xb","y3xa","y3td","y3tv","y3ro","y3vb",};

// In File Scope...
enum { DVzna, DVini, DVatt, DVgrc, DVgtk, DVona, DVonb, DVww,  };

typedef struct { char *key, val, deco; } VVdata;
static VVdata dedata[ET_ET+VV_VV+1]={
 { "y3db",     DoPush,  DVww }, { "y3da",     DoRoll,  DVww },
 { "y3eb",       DoEE,  DVww }, { "y3ea",       DoEA,  DVww },
 { "y3fb",      BuFix,  DVww }, { "y3fa",       BuFA,  DVww },
 { "y3gb",      BuGen,  DVww }, { "y3ga",       BuGA,  DVww },
 { "y3hb",      BuHid,  DVww }, { "y3ha",       BuHA,  DVww },
 { "y3ib",     InitBu,  DVww }, { "y3ia",     InitBA,  DVww },
 { "y3xb",     Exitor,  DVww }, { "y3xa",      ExitA,  DVww },
 { "y3td",      DoTxt,  DVww }, { "y3tv",    InitVal,  DVww },
 { "y3ro",     RoList,  DVww }, { "y3vb",       VBox,  DVww },
 { "eeee",      ET_ET, DVzna }, { "attach",    VVatt, DVatt },
 { "buquanta",  VVbuq, DVini }, { "fstacklim", VVfsl, DVini },
 { "gtk",       VVgtk, DVgtk }, { "gtkrc",     VVgrc, DVgrc },
 { "nogtkrc",   VVnog, DVini }, { "xyshow",    VVxys, DVini },
 { "showmode",VVShoMo, DVini }, { "wiconic", VVIconi, DVini },
 { "wontop",  VVOnTop, DVini }, { "wstick",  VVStick, DVini },
 { "wundec",  VVUndec, DVini }, { "ba",         VVba, DVona },
 { "bb",         VVbb, DVonb }, { "da",         VVda, DVona },
 { "db",         VVdb, DVonb }, { "ea",         VVea, DVona },
 { "eb",         VVeb, DVonb }, { "ia",         VVia, DVona },
 { "ib",         VVib, DVonb }, { "xa",         VVxa, DVona },
 { "xb",         VVxb, DVonb }, { "bud",       VVbud, DVzna },
 { "bum",       VVbum, DVzna } };

