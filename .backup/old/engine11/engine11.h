#include <cstring>
#include <ctime>
#include "solution.h"

#ifndef EMENGINE_H
#define EMENGINE_H


#define ENGINENAME "ENGINE1-alpha1"
#define ENGINE_DEBUGMODE_LEVEL 0

#define ENGINE_ERROR_LOADING 500L

class emEngine
{
    public:
        emSolution emploi;
        emEngine(char* cf);
        virtual ~emEngine();
        void execute(bool _first ,bool _new,double hltx,double lltx);
    protected:
        void NextDay();
        void initStartDay();
        void doFill();
        void doRandomize();
        void doSort(const bool ascendant);
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
        int curcindex,curmindex,curpindex,cursindex,curduree,curfday;

        int max_processed_seances,highlevel_process_count,
        lowlevel_process_count,process_tries_count,all_processed_seances;
        int* FProcessArray;
        int FProcessArrayLength;

        bool process_running,bresult;
        int tmpgw,tmpi,iresult,iresultA,iprocess;
        CFillOption tmpfo;
        bool isForbiddenday;
        int class_se,sse,pse;
        bool bcangroup,byquinz;
        CDayTable* ProfDT,*SalleDT,*ClasseDT;
        int PrSalle,PrDay,PrStartDay,PrHour,PrEndhour;
        int seances_count,salles_count,classes_count,
        prof_count, mat_count;
        time_t ptime,stime;
        double process_percent,progression_tx;
        double process_speed;

};

#endif // EMENGINE_H
