#include <cstring>
#include <ctime>
#include "../solution.h"

#ifndef EMENGINE_H
#define EMENGINE_H


#define ENGINENAME "ENGINE1-alpha31"
#define ENGINE_DEBUGMODE_LEVEL -1

#define ENGINE_ERROR_LOADING 500L


const char SPEED_SYM[4]= {'|','/','-','\\'};

class emEngine
{
public:
    emSolution emploi;
    emEngine(char* cf);
    virtual ~emEngine();
    void execute(bool _first ,bool _new,double hltx,double lltx,double mltx);
    bool Release();
    bool  verifySolution(bool b);
    bool  getOptimizeValue(int *hp,int*hc,int*cp,int*cc);
    bool Load(char* cf);
    bool Save(const char* fn);
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
private:
    bool FMatMapArray[MAX_CLASSE_COUNT][MAX_MAT_COUNT][6];
    CBitDayTable *FMapClasseBitDT_A,*FMapSalleBitDT_A,*FMapProfBitDT_A,
    *FMapClasseBitDT_B,*FMapSalleBitDT_B,*FMapProfBitDT_B;

    CProcessLevelRecord *FMatProcessMap;

    int curcindex,curmindex,curpindex,cursindex,curduree,curfday;
    unsigned long FProcessArrayChecksum;
    int max_processed_seances,highlevel_process_count,
    lowlevel_process_count,process_tries_count,
    all_processed_seances,process_last_progession;
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
    double process_percent,progression_tx;
    double process_speed;


};



#endif // EMENGINE_H
