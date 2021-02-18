
#include "../3rdparty/Console/console.h"
#include "classes.h"
#include "engine.h"
#include "utils.h"



ScheduleEngine::~ScheduleEngine()
{
    //dtor
}
bool ScheduleEngine::release()
{
    delete this;
    return true;
}

ScheduleEngine::ScheduleEngine(): _solution(),_pluginInfo(),_engineConfig(){
    //  pluginInfo.description="\0";
    //  pluginInfo.name=&ENGINENAME;
    _pluginInfo.version_maj = 1;
    _pluginInfo.version_min = 0;
    _engineConfig.slowprinting = true;
    _engineConfig.optimizebranching = false;
}

bool ScheduleEngine::load(CEngineConfig cfg){
  try
    {
        _engineConfig = cfg;
        _solution.load(_engineConfig.configfile);
       
        const auto& st = _solution.stats();
        _matMapDT=new CMapDayTable[st.classes];

        _matProcessLevel.reserve(st.mats);

        _mapClassesBitDT_A=new CBitDayTable[st.classes];
        _mapCroomBitDT_A=new CBitDayTable[st.crooms];
        _mapProfBitDT_A=new CBitDayTable[st.profs];

        _mapClassesBitDT_B=new CBitDayTable[st.classes];
        _mapCroomBitDT_B=new CBitDayTable[st.crooms];
        _mapProfBitDT_B=new CBitDayTable[st.profs];
        FTypeCroomMapArray=new  CDayTable[MAX_CROOMTYPE_COUNT];

        #if  OPTIMIZE_BRANCHING ==1
        FmhtLength_b=new CMapHourTable[9];
        FmhtLength_e=new CMapHourTable[9];

        FmdtMat_b=new CMapDayHoursTable[st.mats];
        FmdtMat_e=new CMapDayHoursTable[st.mats];
        #endif
    }
    catch(...)
    {
       
        Console::WriteEx("\nError loading engine!",ColorRed);
        return false;
    }
    return true;
}

bool ScheduleEngine::save(const char* fn){

   return _solution.save(fn);

}


bool  ScheduleEngine::checkSolution(const bool b){
   return _solution.checkProcessedShifts();
}

void ScheduleEngine::execute(bool _first ,bool _new)
{
    _processStats.max_shifts=0;
    _profs_bh.high=0;
    _classes_bh.high=0;
    time(&ptime);
    if (_first) {

        stime=ptime;
    }
   
    
    _processStats.highlevel_count=((int)_engineConfig.hltx*_solution.stats().shifts / 100);
    _processStats.lowlevel_count=((int) _engineConfig.lltx*_solution.stats().shifts / 100);
    printf("\nhltx=%2.2f%% %d/%d\nlltx=%2.2f%% %d/%d\nmltx=%2.2f%%\n\n",
        _engineConfig.hltx,_processStats.highlevel_count,_solution.stats().shifts,
        _engineConfig.lltx,_processStats.lowlevel_count,_solution.stats().shifts,
        _engineConfig.mltx

          );

    for(int i=0; i<_solution.stats().mats; i++)
    {
        int sc = _solution.getShiftsCount(i, emMat);
        ScheduleEngine::ProcessLevel pv(
            sc,
            (int)(_engineConfig.mltx * sc / 100),
            _engineConfig.mltx,
            0
            );
        _matProcessLevel.emplace_back(pv);
       
#if ENGINE_DEBUGMODE_LEVEL >0
        printf("mindex=%d   count=%d    level=%d\n",
               i,
               _matProcessLevel.back().shiftCount,
               _matProcessLevel.back().level);
#endif
    }

    initialize(_first || _new);
    cursor_y=Console::GetCursorY();
    startSearching();
   //TODO Optimize;

}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ScheduleEngine::initProcesses()
{
 
    //_dProcesses = new int[_solution.stats().shifts];
    _dProcesses.clear();
    //int k=0;
    for (int j=0 ; j< _solution.stats().shifts; j++)
    {

       
        //_dProcesses[k]=j ;
        _dProcesses.push_back(j);
        //fix rank
        _solution.ShiftTable()[j].rank=j;
        //reset delat
        _solution.ShiftTable()[j].delta=8-_solution.ShiftTable()[j].length;//(1+_solution.ShiftTable()[j]._current.every2weeks);
      
    }

    _processesCount= _dProcesses.size();

}

bool ScheduleEngine::doCheck()
{

    for (int p=0 ; p<_processesCount; p++)
        for (int j=0 ; j<_processesCount; j++)
            if ((j!=p)&&(_dProcesses[p]==_dProcesses[j]))
            {

                Console::WriteEx("   ->check _dProcesses duplication error for index %d && %d =>%d!",ColorRed);
                // Console::Pause("press p to continue","p");
                return false;
            }
    return true;
}

unsigned long ScheduleEngine::processesCheckSum()
{

    unsigned long sum = _adler32((unsigned char*)&(_dProcesses[0]),_processesCount);
    //sum =sum^0xFFFFFFFF;
    // printf("\n checksum=%ld\n\n",sum);
    return unsigned(sum);

}

bool ScheduleEngine::moveIndex(int i1,int i2)
{
    int  pi1,tmpI2;

    if ((i2>=i1)||(i1<1)||(i2<-1)||(i1>=_solution.stats().shifts)||(i2>=_solution.stats().shifts))
        return false;
    pi1=_dProcesses[i1];//save i1
    ( _solution.ShiftTable()[pi1].delta)++;
    tmpI2=i2;

    if (tmpI2==-1)
        for (int j=0 ; j< i1; j++)
            if ( _solution.ShiftTable()[pi1].delta> _solution.ShiftTable()[_dProcesses[j]].delta)
            {
                tmpI2=j;
                break;
            }


    if (tmpI2<0)
        return false;
    //translation
    for (int i=i1; i> tmpI2; i--)
        _dProcesses[i]=_dProcesses[i-1];
    _dProcesses[tmpI2]=pi1; // fill i2 with i1
    return true;
}

void ScheduleEngine::swapValues(const int v,const int w)
{
    int tmp=_dProcesses[w] ;
    _dProcesses[w]=_dProcesses[v];
    _dProcesses[v]=tmp;

}

int ScheduleEngine::doPartition(const int p,const int r)
{

    //random pivot or not!!
    //int x=_dProcesses[rand() % _processesCount];
    int x=_dProcesses[r];

    int i=p-1;

    for(int j=p; j<r; j++)
    {
        if( _solution.ShiftTable()[_dProcesses[j]].delta<= _solution.ShiftTable()[x].delta)
        {
            i++;
            swapValues(i,j);
        }
    }
    swapValues(i+1,r);
    return i+1;

}

void ScheduleEngine::doQuickSort(const bool ascendant,const int p,const int r)
{

    if(p<r)
    {
        int part=doPartition(p,r);//pivot
        doQuickSort(ascendant,p, part -1);
        doQuickSort(ascendant, part +1,r);
    }

}

void ScheduleEngine::doSort(const bool ascendant)
{

    doQuickSort(ascendant ,0,_processesCount-1);

#if ENGINE_DEBUGMODE_LEVEL >0
    int i;
    for (i=0; i<_processesCount; i++)
    {
        printf("%d   [%d]     rank=%d     length=%d   _current.every2weeks=%d  delta=%d\n",
               i,
               _dProcesses[i],
               _solution.ShiftTable()[_dProcesses[i]].rank,
               _solution.ShiftTable()[_dProcesses[i]].length,
               _solution.ShiftTable()[_dProcesses[i]]._current.every2weeks,
               _solution.ShiftTable()[_dProcesses[i]].delta
              );
    }
#else

#endif


}

void ScheduleEngine::doRandomize()
{
  
    for (int j=0 ; j<_processesCount; j++)
    {
        srand(time(0));
        swapValues(j,rand() % _processesCount);
        // moveIndex(j,rand()% _processesCount);
        
    }
    //printf("\nnew checksum: %ul", processesCheckSum());
}

void ScheduleEngine::doFillConstraintMap(){
    int j,d,h;
#if  OPTIMIZE_BRANCHING ==1
 for (d=0 ; d<6; d++)
  for (h=0 ; h<=15; h++)
   {
     FmhtLength_b[0][d][h]=0;
     FmhtLength_b[0][d][h]=0;
     FmhtLength_b[1][d][h]=0;
     FmhtLength_b[1][d][h]=0;

    for (j=2 ; j<9; j++){
    FmhtLength_b[j][d][h]=1;
    FmhtLength_e[j][d][h]=1;

    if((j!=3)&&( h % 2!=0))
        FmhtLength_b[j][d][h]=0;

    if((d>3)&&(h>=8)){
        FmhtLength_b[j][d][h]=0;
        FmhtLength_e[j][d][h]=0;
    }

   }


 /*
        FmhtLength_b[2][d][h]=((h % 2==0)&&( (h<=6)||((h>=8)&&(h<=13)) ));
        FmhtLength_e[2][d][h]=((h % 2==0)&&( ((h>=2)&&(h<=8))||((h>=10)&&(h<=15)) ));
 */
        FmhtLength_b[3][d][h]=(h % 4==0);
        FmhtLength_b[4][d][h]=(h % 4==0);
        FmhtLength_b[6][d][h]=(h==0)||(h==2)||(h==8)||(h==10);
        FmhtLength_b[8][d][h]=(h % 8==0);

        FmhtLength_b[5][d][h]=0;
        FmhtLength_e[5][d][h]=0;

   }

  for (d=0 ; d<6; d++)
   for (h=0 ; h<=15; h++)
   {

   for (j=0 ; j<_solution.stats().mats; j++)
    {
      FmdtMat_b[j][d][h]=0;
      FmdtMat_e[j][d][h]=0;
    }

    if(h<6){
            FmdtMat_b[7][d][h]=_matProcessLevel[7].level;//HG >11h


        }
    if(h<4){

            FmdtMat_b[8][d][h]=_matProcessLevel[8].level;//
            FmdtMat_b[9][d][h]=_matProcessLevel[9].level;//
            FmdtMat_b[10][d][h]=_matProcessLevel[10].level;//

        }
    if(h>8){
        FmdtMat_e[0][d][h]=_matProcessLevel[0].level;//math <12h
      if(d==0)
        FmdtMat_e[3][d][h]=_matProcessLevel[3].level;//arabe lundi <12h
      if(d<=1)
        FmdtMat_e[4][d][h]=_matProcessLevel[4].level;//francais lundi,mardi <12h
     if(d==3)
        FmdtMat_e[5][d][h]=_matProcessLevel[5].level;//
    }

    if(h>12){
        FmdtMat_e[0][d][h]=_solution.stats().shifts;
        FmdtMat_e[13][d][h]=_solution.stats().shifts; //ph <14h 100%
        }

    if(h>14){
        FmdtMat_e[6][d][h]=MAX_SHIFTS_COUNT;//sport <17h
    }

   }
#endif
}

void ScheduleEngine::initialize(const bool AReset)
{
    if (AReset)
    {
        _processStats.percent=0;

        initProcesses();
        if (doCheck() == false) {
            perror("Engine initialize error: bad check!");
        }
        //  if (goRandomizeShifts in _solution.GlobalOptions)
        doRandomize();
        FProcessArrayChecksum=processesCheckSum();
        #if  OPTIMIZE_BRANCHING ==1
        doFillConstraintMap();
        #endif


    }
    if (
        (   (_processStats.speed>=1000) && (_processStats.tries_count > (50000-(int)_processStats.speed*10) )  )
        || ((_processStats.tries_count-_processStats.last_progession)>(1000+(int)_processStats.percent*500))
    )
    {

        Console::WriteEx("\n\tbad search path, reprocessing..",ColorRed);
        cursor_y++;
        Sleep(1000);
         /*if(_processStats.percent<99.0 ){
         doRandomize();
         }
         else {
          doSort(true);
         }*/
        doRandomize();
        _processStats.tries_count=0;
        _processStats.last_progession=_processStats.tries_count;
        _processStats.max_shifts=0;
        _profs_bh.high=0;
        _classes_bh.high=0;
        time(&ptime);
        FProcessArrayChecksum=processesCheckSum();
    }

    for (int i=0 ; i< MAX_CROOMTYPE_COUNT; i++){
      int c=_solution.CroomCountByType(i);
      // printf("type croom:%d        count:%d\n",i,tmpf);
       for (int j=0 ; j<10; j++)
          for (int k=0 ; k<16; k++)
            FTypeCroomMapArray[i][j][k]=c ;
    }



    for (int i=0 ; i<_solution.stats().classes; i++)
        for (int j=0 ; j<_solution.stats().mats; j++)
            for (int k=0 ; k<6; k++)
                _matMapDT[i][j][k]=0;

    if (_processesCount<_solution.stats().shifts)
    {
        for (int i=0 ; i<_processesCount; i++)
            _solution.clearShift(_dProcesses[i],true);
    }
    else
    {
        //doit for all!

        for (int k=0 ; k<=10; k++)
        {
            for (int i=0; i<_solution.stats().classes; i++)
            {
                _mapClassesBitDT_A[i][k]=0;
                _mapClassesBitDT_B[i][k]=0;
            }

            for (int i=0; i<_solution.stats().profs; i++)
            {
                _mapProfBitDT_A[i][k]=0;
                _mapProfBitDT_B[i][k]=0;

            }

            for (int i=0; i<_solution.stats().crooms; i++)
            {
                _mapCroomBitDT_A[i][k]=0;
                _mapCroomBitDT_B[i][k]=0;
            }

        }


        for (int i=0; i<_solution.stats().shifts; i++)
        {
           // CShift& sh = _solution.ShiftTable()[i];
            _solution.ShiftTable()[i].day=-1;
            _solution.ShiftTable()[i].hour=-1;
            _solution.ShiftTable()[i].doAlternatewith=(int)foNoWhere;
            _solution.ShiftTable()[i].dogroupwith=-1;
            _solution.ShiftTable()[i].groupedwith=-1;
        }

    }
    for (int i=0; i<_solution.stats().mats; i++)
    {
        _matProcessLevel[i].iprocess =0;

    }
    _current.cindex=-1;
    _current.mindex=-1;
    _current.pindex=-1;

    _profs_bh.count=0;
    _classes_bh.count=0;
    _processHour.day=-1;
    _processHour.start=-1;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */

bool ScheduleEngine::startSearching()
{

    SYSTEMTIME st0,st1;
    int valp,valc,osp,osc;
    double diff_time;
    _processStats.running=true;
    _processStats.tries_count=0;
    _processStats.last_progession=0;
  
    _processHour.index=0;;
    _processHour.start=0;
    initStartDay();
    while (_processHour.index<_processesCount)
    {

#if ENGINE_DEBUGMODE_LEVEL >1
        printf("\r[%d] Shift[%d] processing",_processHour.index,_dProcesses[_processHour.index]);
#else

#endif


        if (process())
        {
            // NextShift
            _processHour.index++;
            _processHour.start=0;
        }
        else //not processed
        {
            //next process hour
            _processHour.start++;
            if (_processHour.start>15) //reach last hour
            {

                _processHour.start=0;
                if (!moveIndex(_processHour.index, -1/*max(0,2*i-_dShifts.size())*/)) {
                    printf("\n moveIndex failure: %d", _processHour.index);
                }
                initialize(false);
                _processStats.last_max_shifts=_processStats.max_shifts;
                _processStats.max_shifts=(_processHour.index>=_processStats.max_shifts)?_processHour.index:_processStats.max_shifts;
                 if(_processStats.max_shifts>_processStats.last_max_shifts)
                    _processStats.last_progession= _processStats.tries_count;
                _processHour.index=0;

                _processStats.tries_count++;
        #if SLOW_PRINTING
              if (_processStats.tries_count % 10==0){
        #endif
                //_solution.getOptimizeInfo(&valp,&valc,&osp,&osc);
                diff_time=difftime(time(0),ptime);
                _profs_bh.high=(_profs_bh.high>_profs_bh.count)?_profs_bh.high:_profs_bh.count;
                _classes_bh.high=(_classes_bh.high>_classes_bh.count)?_classes_bh.high:_classes_bh.count;

                //progression_tx=_processStats.percent;
                _processStats.percent=(double)_processStats.max_shifts/(double)_solution.stats().shifts*100;

                time_t2SystemTime(time(0)-ptime,&st0);
                time_t2SystemTime(time(0)-stime,&st1);

                _processStats.speed=(double)(_processStats.tries_count/diff_time);

                Console::SetCursorPosition(3,cursor_y);
                printf("%05d #%03d %c %02.3f%% %d/%d %d/s  BH (p:%d , c:%d)",
                       _processStats.tries_count+1,
                       _processStats.tries_count-_processStats.last_progession,
                       SPEED_SYM[_processStats.tries_count % 4],

                       _processStats.percent,
                       _processStats.max_shifts,
                       _processStats.all_shifts,
                       _processStats.speed,
                       _profs_bh.count,
                       _classes_bh.count
                      
                       
                      );
                //printf(" crc%012ld", FProcessArrayChecksum);
                printf(" %02d:%02d:%02d/%02d:%02d:%02d",
                       st0.wHour, st0.wMinute,st0.wSecond,
                       st1.wHour, st1.wMinute,st1.wSecond);

            #if SLOW_PRINTING ==1
                }
            #endif
            }//end  if (_processHour.start>15) 
        } //end not processed


    }  //while
    _processStats.all_shifts=_processHour.index;
    if(_processStats.all_shifts==_processesCount)
    {
        _solution.rebuildSolution();
        return true;
    }
    return false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool ScheduleEngine::process()
{
    bool result=false;
    //_current.isForbidden=false;
    _processHour.groupWith=-1;
    _current.fillMode= EFillMode::foMixte;
    _current.sindex=_dProcesses[_processHour.index];
    _currentShift=&_solution.ShiftTable()[_current.sindex];
    _current.length=_currentShift->length;

    _current.mindex=_currentShift->mindex;
    _current.cindex=_currentShift->cindex;
    _current.pindex=_currentShift->pindex;
    _current.fday=_solution.ProfTable()[_current.pindex].fday;

    if (_current.pindex==-1)
    {
        /* ShowMessage(format('FATAL ERROR: No "PROF" assigned for "Shift":%d "Mat":%d "Classe":%d',
        [_current.sindex,_current.mindex,curCindex])); */
        Console::Write("FATAL ERROR: No 'PROFESSOR' assigned for 'SHIFT' n:");
        Console::Write(_current.sindex);
        return false;
    }


    _processHour.day = _processHour.startDay-1;
    nextDay();
    _processHour.croom=0;

    _current.bitset=(1 << _current.length)-1;
    _current.bitset=_current.bitset << (32- _current.length-_processHour.start);

    while (_processHour.croom<_solution.stats().crooms)
    {
        if (
            (!_current.isForbidden)
            &&(_currentShift->croom_type== _solution.CroomTable()[_processHour.croom].stype)
            &&(checkIsEmpty()!=-1)
        )
        {
            //TODO:check next && previous Shift
            result = fillCroom();
            if (result)
                break;
            else
                fillCroom(false) ;


        }

        _processHour.croom++;
        if (_current.isForbidden ||(_processHour.croom>=_solution.stats().crooms))
        {
            _processHour.croom=0;
            nextDay();
            if (_processHour.day==_processHour.startDay)
                break;


        }


    }  //end  while
#if ENGINE_DEBUGMODE_LEVEL >2
    printf("    ->[%d] process()  result=[%d]\n", _current.sindex,result);
#else

#endif


    return result;

}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool ScheduleEngine::fillCroom(bool abool)
{
    bool result=false;
    if (abool)
    {
        //if we have to setLink 
          if (_processHour.groupWith!=-1)
          {
            _solution.setLink(_processHour.groupWith, _current.sindex,ltGroup);
          }
          _currentShift->crindex=_processHour.croom;
          _currentShift->hour=_processHour.start;
          _currentShift->day=_processHour.day;
          _currentShift->doAlternatewith=(int)(_current.fillMode);

       // _solution.fillCroom(_current.sindex,_processHour.croom,_processHour.day,_processHour.start,_current.fillMode,_processHour.groupWith,true);

        if ((_current.fillMode==foMixte)||(_current.fillMode==foWeekA))
        {
            if (_processHour.groupWith==-1)
                _mapClassesBitDT_A[_current.cindex][_processHour.day]|= _current.bitset;
            _mapCroomBitDT_A[_processHour.croom][_processHour.day] |= _current.bitset;
            _mapProfBitDT_A[_current.pindex][_processHour.day]|= _current.bitset;

        }
        if ((_current.fillMode==foMixte)||(_current.fillMode==foWeekB))
        {
            if (_processHour.groupWith==-1)
                _mapClassesBitDT_B[_current.cindex][_processHour.day]|= _current.bitset;
            _mapCroomBitDT_B[_processHour.croom][_processHour.day] |= _current.bitset;
            _mapProfBitDT_B[_current.pindex][_processHour.day]|= _current.bitset;
        }

        //update _matMapDT
        _matMapDT[_current.cindex][_current.mindex][_processHour.day]=1;
        /* if(_processHour.groupWith!=-1)
             _matMapDT[_current.cindex][_solution.ShiftTable()[_processHour.groupWith].mindex][_processHour.day]=1;*/
        _matProcessLevel[_current.mindex].iprocess++;
        FTypeCroomMapArray[_currentShift->croom_type][_processHour.day][_processHour.start]--;
       /* if (_after_break_hour==true)
            _profs_bh.count++;
        if((_profs_bh.count>0)&&(_processHour.end!=8)&&(_processHour.end!=16)&&(_solution.ProfTable()[curpindex].weeka[_processHour.day][_processHour.end]!=-1))
            _profs_bh.count--;
*/
        //dec delta
        //_currentShift->delta--;
        result=true;
    }
    else
    {
        /* _currentShift->crindex=-1;
         _currentShift->hour=-1;
         _currentShift->day=-1;
         _currentShift->doAlternatewith=(int)(foNoWhere);
          if (_currentShift->groupedwith!=-1)
         {
            _solution.setLink(_current.sindex,_currentShift->groupedwith,ltClear);

         }

         */
        //_solution.fillCroom(_current.sindex,_processHour.croom,_processHour.day,_processHour.start,_current.fillMode,_processHour.groupWith,false);

        //update _matMapDT
        // _matMapDT[_current.cindex][_current.mindex][_processHour.day]=0;

        result=true;
    }


    if (!result)
        Console::WriteEx("fillCroom() failed!",ColorRed);
#if ENGINE_DEBUGMODE_LEVEL >2
    else  printf("\t->[%d] fillCroom() n:%d ....[ok]\n", _current.sindex,_processHour.croom);
#else

#endif


    return result;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int ScheduleEngine::checkIsEmpty()
{
    int iresultA=-1;
    int iresult=-1;
    _processHour.groupWith=-1;

    _current.every2w= _currentShift->every2weeks;

    _current.fillMode=foWeekA;

    CroomBitDT=&_mapCroomBitDT_A[_processHour.croom];
    ProfBitDT=&_mapProfBitDT_A[_current.pindex];
    ClasseBitDT=&_mapClassesBitDT_A[_current.cindex];
    _current.class_shift = _solution.ClasseTable()[_current.cindex].weeka[_processHour.day][_processHour.start] ;//class  shift
    iresult=checkEmptyDT();
    if (((iresult==-1)&& _current.every2w)|| ((iresult!=-1) && !_current.every2w))
    {
        if (_current.every2w)
            _current.fillMode=foWeekB;
        else
            _current.fillMode=foMixte;

        CroomBitDT=&_mapCroomBitDT_B[_processHour.croom];
        ProfBitDT=&_mapProfBitDT_B[_current.pindex];
        ClasseBitDT=&_mapClassesBitDT_B[_current.cindex];
        _current.class_shift = _solution.ClasseTable()[_current.cindex].weekb[_processHour.day][_processHour.start] ;//class  shift
        iresultA=iresult;
        iresult=checkEmptyDT();
        if (!_current.every2w && (iresult!=iresultA))
            iresult=-1 ;
    }


    if ((iresult!=-1) && (_current.class_shift !=-1) && _current.canBeGrouped)
        _processHour.groupWith = _current.class_shift;
#if ENGINE_DEBUGMODE_LEVEL >3
    printf("\t->[%d] checkIsEmpty  result=[%d] _processHour.start=[%d]\n", _current.sindex,iresult,_processHour.start);
#else

#endif


    return iresult;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
inline int ScheduleEngine::checkEmptyDT()
{

    _current.canBeGrouped=_solution.CanBeByGroup(_current.sindex, _current.class_shift ,_processHour.day,_processHour.start);

    if(  (( ( (*ClasseBitDT)[_processHour.day] & _current.bitset) !=0 ) && ! _current.canBeGrouped )||
            ( ( (*CroomBitDT)[_processHour.day] & _current.bitset) !=0 )  ||
            ( ( (*ProfBitDT)[_processHour.day] & _current.bitset) !=0 )
      )
        return -1;


    return _processHour.start;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ScheduleEngine::initStartDay()
{
    srand(time(0))  ;
    _processHour.startDay=rand() % 5;
    //_processHour.day=_processHour.startDay;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ScheduleEngine::nextDay()
{
    _processHour.day++;
    _processHour.day=_processHour.day % 6;
    _processHour.end=_processHour.start+ _current.length;
   // _after_break_hour=(_processHour.start>0) && (_processHour.start!=8) && (_solution.ProfTable()[curpindex].weeka[_processHour.day][_processHour.start-1]==-1);
    _current.isForbidden=
        (_processHour.day== _current.fday)
        || (_matMapDT[_current.cindex][_current.mindex][_processHour.day]==1)//no 2 mat in same day
        ||( _processHour.end>16) //evening
        || ((_processHour.start<8)&&( _processHour.end>8))//morning

#if  OPTIMIZE_BRANCHING
        || (FmhtLength_b[_current.length][_processHour.day][_processHour.start] == false)
        || (FmhtLength_b[_current.length][_processHour.day][_processHour.end - 1] == false)
        || (_matProcessLevel[_current.mindex]._processHour.index < FmdtMat_b[_current.mindex][_processHour.day][_processHour.start])
        || (_matProcessLevel[_current.mindex]._processHour.index < FmdtMat_e[_current.mindex][_processHour.day][_processHour.end - 1]) 
        
#else
       ||( ((_current.length==4)||(_current.length==3)) &&(_processHour.start % 4!=0)    )           //for 2h/1.5h length shifts
        ||( (_current.length==8) &&(_processHour.start % 8!=0) )                                     //.... 4h 4h
        ||( (_processHour.start % 2!=0) && (_current.length!=3)    )                                 // begin with half hour
        ||( (_processHour.day>3)&& (_processHour.start>=8) )                                                   //no course for friday & saturday evening

#endif      
      /*                                                                                  
        //sport
        || ( (_current.mindex==6) && (_processHour.end>14)    )
       
         // busy hours
        || ( (_processHour.index<_processStats.lowlevel_count)&& (_current.length==2) &&(_processHour.end>14)  )
        //heure creuse
        || ( (_processHour.index<_processStats.lowlevel_count)&& ((_processHour.start>0) && (_solution.ClasseTable()[_current.cindex].weeka[_processHour.day][_processHour.start-1]==-1))  )
        || ( (_profs_bh.count>10)&& _after_break_hour  )
        */
      

        ;

}
