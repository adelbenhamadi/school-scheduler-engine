#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include<stdlib.h>

const int MAX_CLASSE_COUNT=100;
const int MAX_MAT_COUNT=100;

typedef int CDayTable[11][16];
typedef DWORD CBitDayTable[11];


typedef enum {ecmM1,ecmM2} CEncryptmode;
typedef enum {crmMode1,crmMode2} CCompressionMode;
typedef enum {ooNoOrphSeance,ooNoHeureCreuse,ooNoHeurePointe,
              ooNoSportAfterClass,ooPrefMatin,ooPrefSoir,ooNoSportAfter17h
             } COptimizeOption;

typedef enum {soPrefMatin,soPrefSoir,soNoHeurePointe} CSeanceOptimizeOption;
typedef enum {doHighLighted,doLocked,doMarked,doParked} CDrawOption;

typedef enum {foSemaineA,foSemaineB,foMixte,foNoWhere} CFillOption;
typedef enum {goClassOptimize,goProfOptimize,goNotIncludedSalle,
              goNotIncludedClasse,goNotIncludedProf,goNotIncludedMat,
              goRandomizeSeances,goAllwaysRandomizeSeances,goNotProcessLockedSeances,
              goSortByDureeSeances
             } CGlobalOptimizeOption;

typedef enum {emSalle,emClasse,emProf,emMat} CEmploiMode;
typedef enum {ltClasse,ltSalle,ltProf,ltGroup,ltClear} ClinkType;

typedef struct
{
    char magic[255];
    int magic_len,version_maj,version_min;
    char version_lbl[64];
    char magic_separator[2];
    char licence[255];
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
    char lbl_annee[128];
    char lbl_footer[255];

    char etab_code[64];
    char etabl_name[255];
    char etabl_email[255];
    char etabl_tel[16];
    char etabl_fax[16];
    char etabl_loc_ville[128];
    char etabl_loc_rue[255];
    char etabl_loc_cp[6];

} CemConfig;

typedef struct
{
    char        symbol[64];
    char        name[64];
    int      fdayC1,fdayC2;
    bool        notincluded;

} CMat;

//classe type
typedef struct
{
    char    name[9];
    int      rang;
    int      niveau ;
    int      nb_eleve;
    CDayTable	weeka,weekb;
    WORD    options;
    bool        notincluded;
    int      attachedClasses[10];

} CClasse;
//prof type
typedef struct
{
    char       symbol[9];
    char       name[255];
    int     rang;
    int     mIndex;
    CDayTable  weeka,weekb;
    WORD     options;
    int         fday;
    bool        notincluded;

} CProf;

//seance type
typedef struct
{
    int         rang;
    int      	hour;//hour
    int    		day;//day
    int       	duree;//duree
    int         delta;
    int         mindex;
    int         cindex;
    int         saindex;
    int         type_salle;
    int         pindex;
    int         groupedwith;
    int         dogroupwith;
    int         doquinzwith;
    WORD options;//CSeanceOptimizeOption;

    bool     byquinz;
    bool     bygroup;
    WORD     drawopt;
} CSeance;
//salle type
typedef struct
{
    char     name[9];
    int      rang;
    int      stype;
    CDayTable	weeka;
    CDayTable weekb;
    bool        notincluded;
} CSalle;

typedef struct
{
    int fSeance;
    int fSalle;
    int fDay,fHour;
    CFillOption ffillOption;
    size_t fGroupW;
    CEmploiMode fMode;
    size_t fViewindex;

} CFillRecord;

typedef struct
{
    int seanceCount;
    int level;
    double tx;
    int iprocess;
} CProcessLevelRecord;

typedef struct{
    int version_maj;
    int version_min;
    char name[32];
    char description[255];
 }CEnginePlugin;

#endif // CLASSES_H_INCLUDED
