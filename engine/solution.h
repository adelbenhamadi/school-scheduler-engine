#ifndef EMSOLUTION_H
#define EMSOLUTION_H
#include  <windows.h>
#include "classes.h"
#include "cfilestream.h"

class CemSolution
{


private:

    bool FModified;
    int FSeanceTableCount,FSalleTableCount,
    FClasseTableCount,FMatTableCount,FProfTableCount;

    CemConfig*      emConfig;
    CClasse*        FClasseTable;
    CProf*          FProfTable;
    CSalle*         FSalleTable;
    CSeance*        FSeanceTable;
    CMat*           FMatTable;

    COptimizeOption FClassOptions[10] ;
    COptimizeOption FProfOptions[10] ;
    CGlobalOptimizeOption FGlobalOptions[] ;
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
    bool         verifyProcessedSeances(bool b);
    bool         viderSeance2(const int se,bool abool);
    int          getOrphinedIdxByDay(const int AIndex,const int ADay,const CEmploiMode emMode);
    int          getOrphinedIdx(const int AIndex,const CEmploiMode emMode) ;
    int          getCompactIdx(const int AIndex,const CEmploiMode emMode);
    int          getSeancesCount(const int AIndex,const CEmploiMode emMode);

    bool         getOptimizeValue(int *hp,int*hc,int*cp,int*cc);
    bool         CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour);
    CFillOption  getSeanceFillMode(const int Se);
    bool         setLink(const int s1,const int s2,ClinkType Alink);
    bool         parkSeance(const int Sindex,bool ABool);
    bool         RempliSalle(const int ASeance,const int ASalle,const int ADay,const int AHour,
                             const CFillOption fo,const int gw,bool abool);

    bool         LoadFromFile(const char* fn);
    bool         SaveToFile(const char* fn);
    void         clearAllDT();
    void         rebuildSolution();

    void         shellSort();

    void         SetTablesCount();
    int          SeanceTableCount(void);
    int          ClasseTableCount(void);
    int          ProfTableCount(void);
    int          SalleTableCount(void);
    int          MatTableCount(void);
    CClasse*     ClasseTable();
    CSalle*      SalleTable();
    CSeance*     SeanceTable();
    CProf*       ProfTable();
    CMat*        MatTable();
};




#endif // EMSOLUTION_H
