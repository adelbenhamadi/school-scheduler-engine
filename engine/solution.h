#ifndef _EMSOLUTION_H
#define _EMSOLUTION_H
#include  <windows.h>
#include "classes.h"
#include "cfilestream.h"
class CemSolution
{


    private:

        bool FModified;
        int FShiftTableCount,FCroomTableCount,
            FClasseTableCount,FMatTableCount,FProfTableCount;

        CemConfig*      emConfig;
        CClasse*        FClasseTable;
        CProf*          FProfTable;
        CCroom*         FCroomTable;
        CShift*        FShiftTable;
        CMat*           FMatTable;

        COptimizeOption FClassOptions[10] ;
        COptimizeOption FProfOptions[10] ;
        CGlobalOptimizeOption FGlobalOptions[10] ;
        bool  checkDT(const int Sindex,CDayTable *dt);
        bool  CheckAllDT(const int Sindex);

        int getIdxByDaytable(CDayTable *Adt);
    protected:
        bool saveHeader(CFileStream* stream);
        CemConfig* loadHeader(CFileStream* stream);
        int readData(CFileStream* stream,WORD phase);
        bool writeData(CFileStream* stream,WORD phase);
    public:
       CemSolution();
       virtual ~CemSolution();
       bool         verifyProcessedShifts(bool b);
       bool         clearShift(const int se,bool abool);
       int          getOrphinedIdxByDay(const int AIndex,const int ADay,const CEmploiMode emMode);
       int          getOrphinedIdx(const int AIndex,const CEmploiMode emMode) ;
       int          getCompactIdx(const int AIndex,const CEmploiMode emMode);
       int          getShiftsCount(const int AIndex,const CEmploiMode emMode);

       bool         getOptimizeValue(int *hp,int*hc,int*cp,int*cc);
       bool         CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour);
       CFillOption  getShiftFillMode(const int Se);
       bool         setLink(const int s1,const int s2,ClinkType Alink);
       bool         parkShift(const int Sindex,bool ABool);
       bool         FillCroom(const int AShift,const int ACroom,const int ADay,const int AHour,
                             const CFillOption fo,const int gw,bool abool);

       bool         LoadFromFile(const char* fn);
       bool         SaveToFile(const char* fn);
       void         clearAllDT();
       void         rebuildSolution();

       void         shellSort();

       void         SetTablesCount();
       int          ShiftTableCount(void);
       int          ClasseTableCount(void);
       int          ProfTableCount(void);
       int          CroomTableCount(void);
       int          MatTableCount(void);
       int          CroomCountByType(int stype);
       CClasse*     ClasseTable();
       CCroom*      CroomTable();
       CShift*     ShiftTable();
       CProf*       ProfTable();
       CMat*        MatTable();
};




#endif // _EMSOLUTION_H
