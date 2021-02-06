#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "solution.h"
#include "I_engine.h"

#ifndef EMENGINE_H
#define EMENGINE_H


#define ENGINENAME "ENGINE1-alpha31"
#define ENGINE_DEBUGMODE_LEVEL -1

#define ENGINE_ERROR_LOADING 500L

#define OPTIMIZE_BRANCHING 0
#define SLOW_PRINTING      1

const char SPEED_SYM[4]= {'|','/','-','\\'};

class emEngine:public I_Engine_Intf
{
public:

    emEngine();
    virtual ~emEngine();
    void execute(bool _first ,bool _new);

    bool Release();
    bool verifySolution(bool b);
    bool getOptimizeValue(int *hp,int*hc,int*cp,int*cc);
    bool Load(CEngineConfig *ecfg);
    bool Save(const char* fn);
    CEnginePlugin*   getPluginInfo();

    int SeancesCount();
protected:

    void NextDay();
    void initStartDay();
    void doFill();
    void doRandomize();
    void doSort(const bool ascendant);
    void doShellSort(const bool ascendant);
    void doQuickSort(const bool ascendant,const int p,const int r);
    int doPartition(const int p,const int r);
    unsigned long getProcessArrayChecksum();
    bool doCheck();
    void swapValues(const int v,const int w);
    bool moveIndex(int i1,int i2);
    int checkEmptyDT();
    int CheckIsEmpty();

    bool RempliSalle(bool ABool=true);

    void Initialize(const bool AReset);
    bool Process();
    bool StartSearching();
    void doFillConstraintMap();
private:
    CEngineConfig *econfig;
    CemSolution solution;
    CEnginePlugin* pluginInfo;
    CMapDayTable *FMatMapArray;
    //bool FMatMapArray[MAX_CLASSE_COUNT][MAX_MAT_COUNT][6];
    CBitDayTable *FMapClasseBitDT_A,*FMapSalleBitDT_A,*FMapProfBitDT_A,
    *FMapClasseBitDT_B,*FMapSalleBitDT_B,*FMapProfBitDT_B;
    CMapDayHoursTable *FmdtMat_b,*FmdtProf_b,*FmdtClasse_b,*FmdtSalle_b;
    CMapDayHoursTable *FmdtMat_e,*FmdtProf_e,*FmdtClasse_e,*FmdtSalle_e;
    CMapHourTable *FmhtDuree_b,*FmhtDuree_e;
    CDayTable*FTypeSalleMapArray;
    CProcessLevelRecord *FMatProcessMap;

    int curcindex,curmindex,curpindex,cursindex,curduree,curfday;
    unsigned long FProcessArrayChecksum;
    int max_processed_seances,last_max_processed_seances,highlevel_process_count,
    lowlevel_process_count,process_tries_count,all_processed_seances,process_last_progession;
    int* FProcessArray;
    int FProcessArrayLength;

    bool process_running,bresult;
    int tmpgw,tmpi,tmpj,tmpk,tmpf,iresult,iresultA,iprocess;
    DWORD tmpbitset;
    CFillOption tmpfo;
    bool isForbiddenday;
    int class_se,sse,pse;
    bool bcangroup,byquinz;
    //CDayTable* ProfDT,*SalleDT,*ClasseDT;
    CBitDayTable * ProfBitDT,*SalleBitDT,*ClasseBitDT;
    CSeance*curSeance;
    /* CSalle*curSalle;
     CProf*curProf;
     CMat*curMat;
     CClasse*curClasse;*/
    int PrSalle,PrDay,PrStartDay,PrHour,PrEndhour;
    int seances_count,salles_count,classes_count,
    prof_count, mat_count;
    time_t ptime,stime;
    double process_percent;
    double process_speed;
    int cursor_y;
    int classe_hc,prof_hc,max_classe_hc,max_prof_hc;
    bool after_hc;


};



#endif // EMENGINE_H
