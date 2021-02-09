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

    bool release();
    bool verifySolution(bool b);
    bool getOptimizeValue(int *hp,int*hc,int*cp,int*cc);
    bool load(CEngineConfig cfg);
    bool save(const char* fn);
    int shiftsCount();

    CEnginePlugin   pluginInfo() { return _pluginInfo; }
    CemSolution solution() { return _solution; }
    CEngineConfig config() { return _engineConfig; }
protected:

    void nextDay();
    void initStartDay();
    void doFill();
    void doRandomize();
    void doSort(const bool ascendant);
    //void doShellSort(const bool ascendant);
    void doQuickSort(const bool ascendant,const int p,const int r);
    int doPartition(const int p,const int r);
    unsigned long getProcessArrayChecksum();
    bool doCheck();
    void swapValues(const int v,const int w);
    bool moveIndex(int i1,int i2);
    int checkEmptyDT();
    int checkIsEmpty();

    bool fillCroom(bool ABool=true);

    void initialize(const bool AReset);
    bool process();
    bool startSearching();
    void doFillConstraintMap();
   
private:
    CEngineConfig  _engineConfig;
    CemSolution _solution;
    CEnginePlugin _pluginInfo;
    CMapDayTable *FMatMapArray;
    //bool FMatMapArray[MAX_CLASSE_COUNT][MAX_MAT_COUNT][6];
    CBitDayTable *FMapClasseBitDT_A,*FMapCroomBitDT_A,*FMapProfBitDT_A,
    *FMapClasseBitDT_B,*FMapCroomBitDT_B,*FMapProfBitDT_B;
    CMapDayHoursTable *FmdtMat_b,*FmdtProf_b,*FmdtClasse_b,*FmdtCroom_b;
    CMapDayHoursTable *FmdtMat_e,*FmdtProf_e,*FmdtClasse_e,*FmdtCroom_e;
    CMapHourTable *FmhtLength_b,*FmhtLength_e;
    CDayTable*FTypeCroomMapArray;
    CProcessLevelRecord *FMatProcessMap;

    int curcindex,curmindex,curpindex,cursindex,curlength,curfday;
    unsigned long FProcessArrayChecksum;
    int max_processed_shifts,last_max_processed_shifts,highlevel_process_count,
    lowlevel_process_count,process_tries_count,all_processed_shifts,process_last_progession;
    int* FProcessArray;
    int FProcessArrayLength;

    bool process_running,bresult;
    int tmpgw,tmpi,tmpj,tmpk,tmpf,iresult,iresultA,iprocess;
    DWORD tmpbitset;
    CFillOption tmpfo;
    bool isForbiddenday;
    int class_se,sse,pse;
    bool bcangroup,every2weeks;
    //CDayTable* ProfDT,*CroomDT,*ClasseDT;
    CBitDayTable * ProfBitDT,*CroomBitDT,*ClasseBitDT;
    CShift*curShift;
    /* CCroom*curCroom;
     CProf*curProf;
     CMat*curMat;
     CClasse*curClasse;*/
    int PrCroom,PrDay,PrStartDay,PrHour,PrEndhour;
    int shifts_count,crooms_count,classes_count,
    prof_count, mat_count;
    time_t ptime,stime;
    double process_percent;
    double process_speed;
    int cursor_y;
    int classe_hc,prof_hc,max_classe_hc,max_prof_hc;
    bool after_hc;


};



#endif // EMENGINE_H
