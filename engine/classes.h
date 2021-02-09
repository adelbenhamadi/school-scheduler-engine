#include<cstdlib>

#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

const unsigned int MAX_CLASSE_COUNT = 1000;
const unsigned int MAX_MAT_COUNT = 64;
const unsigned int MAX_SHIFT_COUNT = 100000;
const unsigned int MAX_CROOMTYPE_COUNT = 32;

typedef int CDayTable[11][16];
typedef DWORD CBitDayTable[11];
typedef unsigned int CMapDayHoursTable[11][16];
typedef bool CMapDayTable[MAX_MAT_COUNT][6];
typedef bool CMapHourTable[11][16];

enum CEncryptmode{ ecmM1, ecmM2 };
enum CCompressionMode{ crmMode1, crmMode2 };
enum COptimizeOption{
	ooNoOrphShift, ooNoBreakHour, ooNoHeurePointe,
	ooNoSportAfterClass, ooPrefMorning, ooPrefEvening, ooNoSportAfter17h
};

enum CShiftOptimizeOption{ soPrefMorning, soPrefEvening, soNoBusyHour };
enum CDrawOption{ doHighLighted, doLocked, doMarked, doParked };

enum CFillOption{ foWeekA, foWeekB, foMixte, foNoWhere };
enum CGlobalOptimizeOption{
	goClassOptimize, goProfOptimize, goNotIncludedCroom,
	goNotIncludedClasse, goNotIncludedProf, goNotIncludedMat,
	goRandomizeShifts, goAllwaysRandomizeShifts, goNotProcessLockedShifts,
	goSortByShiftLength
};

enum CEmploiMode{ emCroom, emClasse, emProf, emMat };
enum ClinkType{ ltClasse, ltCroom, ltProf, ltGroup, ltClear };

struct CemConfig {
	char magic[255];
	int magic_len, version_maj, version_min;
	char version_lbl[64];
	char magic_separator[2];
	char license[255];


	WORD compresssion_mode;

	char compression_key[255];
	char lbl_1[128];
	char lbl_2[128];
	char lbl_3[128];
	char lbl_4[128];
	char lbl_year[128];
	char lbl_footer[255];

	char etab_code[64];
	char etabl_name[255];
	char etabl_email[255];
	char etabl_phone[16];
	char etabl_fax[16];
	char etab_country[64];
	char etabl_loc_city[128];
	char etabl_loc_road[255];
	char etabl_loc_zip[6];

};

struct CMat{
	char        symbol[64];
	char        name[64];
	int      fdayC1, fdayC2;
	bool        notincluded;

};

//classe type
struct CClasse{
	char    name[9];
	int      rank;
	int      niveau;
	int      nb_eleve;
	CDayTable	weeka, weekb;
	WORD    options;
	bool        notincluded;
	int      attachedClasses[10];

};
//prof type
struct CProf{
	char       symbol[9];
	char       name[255];
	int     rank;
	int     mIndex;
	CDayTable  weeka, weekb;
	WORD     options;
	int         fday;
	bool        notincluded;

};

//shift type
struct CShift{
	int         rank;
	int      	hour;// start hour
	int    		day;//day
	int       	length;//length
	int         delta;
	int         mindex;
	int         cindex;
	int         saindex;
	int         type_croom;
	int         pindex;
	int         groupedwith;
	int         dogroupwith;
	int         doAlternatewith; //weekly alternate ..
	WORD		options;//CShiftOptimizeOption;

	bool     every2weeks;
	bool     bygroup;
	WORD     drawOption;
};
//croom type
 struct CCroom{
	char     name[9];
	int      rank;
	int      stype;
	CDayTable	weeka;
	CDayTable weekb;
	bool        notincluded;
};

 struct CFillRecord{
	int fShift;
	int fCroom;
	int fDay, fHour;
	CFillOption ffillOption;
	size_t fGroupW;
	CEmploiMode fMode;
	size_t fViewindex;

};

struct CProcessLevelRecord{
	int shiftCount;
	int level;
	double tx;
	int iprocess;
};

typedef struct {
	int version_maj;
	int version_min;
	char name[32];
	char description[255];
}CEnginePlugin, * PCEnginePlugin, * LPCEnginePlugin;

struct CEngineConfig{
	char* configfile;
	double hltx;
	double mltx;
	double lltx;
	bool optimizebranching;
	bool slowprinting;
};
#endif // CLASSES_H_INCLUDED
