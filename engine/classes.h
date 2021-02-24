#include<cstdlib>

#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#define DEBUG_VERBOSE 0

const unsigned int MAX_CLASSE_COUNT = 1000;
const unsigned int MAX_MAT_COUNT = 64;
const unsigned int MAX_SHIFT_COUNT = 100000;
const unsigned int MAX_CROOMTYPE_COUNT = 32;

const unsigned int HOUR_TICK_COUNT = 16;
const unsigned int WORKABLE_DAY_COUNT = 6;
const unsigned int PARK_DAY_COUNT = 5;
const unsigned int MAX_DAY_COUNT = WORKABLE_DAY_COUNT + PARK_DAY_COUNT;

typedef int CDayTable[MAX_DAY_COUNT][HOUR_TICK_COUNT];
typedef DWORD CBitDayTable[MAX_DAY_COUNT];
typedef unsigned int CMapDayHoursTable[MAX_DAY_COUNT][HOUR_TICK_COUNT];
typedef bool CMapDayTable[MAX_MAT_COUNT][WORKABLE_DAY_COUNT];
typedef bool CMapHourTable[MAX_DAY_COUNT][HOUR_TICK_COUNT];

enum EEncryptMode { ecmM1, ecmM2 };
enum ECompressionMode { crmMode1, crmMode2 };
enum EDrawMode { doHighLighted, doLocked, doMarked, doParked };
enum EFillMode { foWeekA, foWeekB, foMixte, foNoWhere };

enum EOptimizeOption {
	ooNoOrphShift, ooNoBreakHour, ooNoHeurePointe,
	ooNoSportAfterClass, ooPrefMorning, ooPrefEvening, ooNoSportAfter17h, _ooLast
};
typedef  EOptimizeOption EOptimizeOptions[EOptimizeOption::_ooLast];

enum EShiftOptimizeOption { soPrefMorning, soPrefEvening, soNoBusyHour };

enum EGlobalOptimizeOption {
	goClassOptimize, goProfOptimize, goNotIncludedCroom,
	goNotIncludedClasse, goNotIncludedProf, goNotIncludedMat,
	goRandomizeShifts, goAllwaysRandomizeShifts, goNotProcessLockedShifts,
	goSortByShiftLength, _goLast
};
typedef  EGlobalOptimizeOption EGlobalOptimizeOptions[EGlobalOptimizeOption::_goLast];

enum EScheduleMode { emCroom, emClasse, emProf, emMat };
enum ELinkType { ltClasse, ltCroom, ltProf, ltGroup, ltClear };

struct OptimizeInfo {
	int orphanedIdx;
	int compactIdx;
	OptimizeInfo() : orphanedIdx(0), compactIdx(0) {}
	OptimizeInfo(const int oi, const int ci) : orphanedIdx(oi), compactIdx(ci) {}
};
struct ScheduleConfig {
	char magic[255];
	int magic_len, version_maj, version_min;
	char version_lbl[64];
	char magic_separator[2];
	char license[255];
	char licence_key[255];
	int licence_date;
	int licence_expire;

	WORD encrypt_mode;
	char encrypt_key[255];

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
	//char etab_country[64];
	char etabl_loc_city[128];
	char etabl_loc_road[255];
	char etabl_loc_zip[6];

};

struct CMat {
	char        symbol[64];
	char        name[64];
	int         fdayC1, fdayC2;
	bool        notIncluded;

	
};

//classe 
struct CClasse {

	int      rank;
	char     name[9];
	int      level;
	int      reserved0;
	CDayTable	weeka, weekb;
	WORD    options;
	bool        notIncluded;
	int      attachedClasses[10];
private:
	
};
//prof 
struct CProf {
	char       symbol[9];
	char       name[255];
	int     rank;
	int     mIndex;
	CDayTable  weeka, weekb;
	WORD     options;
	 int         fday;
	 bool        notIncluded;

	
};

//shift 
struct CShift {
	int         rank;
	int      	hour;// start hour
	int    		day;//day
	const int   length;//length
	int         delta;
	const int   mindex;
	const int   cindex;
	int         crindex;
	int         croom_type;
	const int   pindex;
	int         groupedwith;
	int         dogroupwith;
	int         fillMode; 
	WORD		options;//EShiftOptimizeOption;

	const bool     every2weeks;
	const bool     bygroup;
	WORD     drawOption;
	CShift():length(0), mindex(-1), cindex(-1), pindex(-1), every2weeks(0), bygroup(0) {};
	CShift(const int l, const int mati, const int classei, const int profi, const bool e2w, const bool bg) :
		length(l), mindex(mati), cindex(classei), pindex(profi), every2weeks(e2w), bygroup(bg) {};
	CShift operator=(const CShift& rh) {
	 return CShift(rh.length, rh.mindex, rh.cindex, rh.pindex, rh.every2weeks, rh.bygroup);
}
};
//croom 
struct CCroom {
	
	char     name[9];
	int      rank;
	 int      stype;
	CDayTable	weeka;
	CDayTable weekb;
	 bool        notIncluded;

};

struct CFillInfo {
	int fShift;
	int fCroom;
	int fDay, fHour;
	EFillMode ffillOption;
	size_t fGroupW;
	EScheduleMode fMode;
	size_t fViewindex;

};


typedef struct {
	int version_maj;
	int version_min;
	char name[32];
	char description[255];
}CEnginePlugin, * PCEnginePlugin, * LPCEnginePlugin;

struct CEngineConfig {
	char* configfile;
	double hltx;
	double mltx;
	double lltx;
	bool optimizebranching;
	bool slowprinting;
};
#endif // CLASSES_H_INCLUDED
