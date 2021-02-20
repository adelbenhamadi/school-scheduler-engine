#ifndef _EMSOLUTION_H
#define _EMSOLUTION_H
#include  <windows.h>
#include <vector>

#include "classes.h"
#include "cfilestream.h"

class ScheduleSolution
{
public:
    ScheduleSolution();
    virtual ~ScheduleSolution();

    private:

        bool _modified;
        struct Stats {
            int shifts, crooms, classes, profs, mats;
            Stats(): shifts(0),crooms(0),profs(0),mats(0){}
        };
        Stats _stats;
        ScheduleConfig              _scheduleConfig;
        std::vector<CClasse>        _dClasses;
        std::vector<CProf>          _dProfs;
        std::vector<CCroom>         _dCRooms;
        std::vector<CShift>         _dShifts;
        std::vector<CMat>           _dMats;

        EOptimizeOptions            _classesOptimizeOptions ;
        EOptimizeOptions            _profOptimizeOptions ;
        EGlobalOptimizeOptions      _globalOptimizeOptions ;
        bool        checkDT(const int Sindex,CDayTable *dt);
        bool        checkAllDT(const int Sindex);

        int         getIdxByDaytable(CDayTable *Adt);
    protected:
        bool        saveHeader(CFileStream* stream);
        ScheduleConfig loadHeader(CFileStream* stream);
        int         readData(CFileStream* stream,WORD phase);
        bool        writeData(CFileStream* stream,WORD phase);
public:
       bool         checkProcessedShifts();
       bool         clearShift(const int se,bool abool);
       int          getOrphinedShiftByDay(const int AIndex,const int ADay,const EScheduleMode emMode);
       int          getOrphinedShift(const int AIndex,const EScheduleMode emMode) ;
       int          getCompactIdx(const int AIndex,const EScheduleMode emMode);
       int          getShiftsCount(const int AIndex,const EScheduleMode emMode);
       OptimizeInfo         getOptimizeInfo(const EScheduleMode emMode);
      
       bool         CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour);
       EFillMode  getShiftFillMode(const int Se);
       bool         setLink(const int s1,const int s2,ELinkType Alink);
       bool         parkShift(const int Sindex,bool ABool);
       bool         fillCroom(const int AShift,const int ACroom,const int ADay,const int AHour,
                             const EFillMode fo,const int gw,bool abool);

       bool         load(const char* fn);
       bool         save(const char* fn);
       void         clearAllDT();
       void         rebuildSolution();

       void         shellSort();

     
       int          ProfTableCount(void);
       int          CroomTableCount(void);
       int          MatTableCount(void);
       int          CroomCountByType(int cr_type);
       std::vector<CClasse>&     ClasseTable();
       std::vector <CCroom>&      CroomTable();
       std::vector <CProf>&       ProfTable();
       std::vector <CMat>&        MatTable();
      
       CShift*      Shift(const int ind);
       CProf*       Prof(const int ind);
       CMat*        Mat(const int ind);
       ScheduleSolution::Stats&        stats() { return _stats; }
};




#endif // _EMSOLUTION_H
