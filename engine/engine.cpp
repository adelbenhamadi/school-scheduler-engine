
#include "../3rdparty/Console/console.h"
#include "classes.h"
#include "engine.h"
#include "utils.h"



emEngine::~emEngine()
{
    //dtor
}
bool emEngine::release()
{
    delete this;
    return true;
}

emEngine::emEngine(): _solution(),_pluginInfo(),_engineConfig(){
    //  pluginInfo.description="\0";
    //  pluginInfo.name=&ENGINENAME;
    _pluginInfo.version_maj = 1;
    _pluginInfo.version_min = 0;
    _engineConfig.slowprinting = true;
    _engineConfig.optimizebranching = false;
}

bool emEngine::load(CEngineConfig cfg){
  try
    {
        _engineConfig = cfg;
        solution().LoadFromFile(_engineConfig.configfile);
        shifts_count= _solution.ShiftTableCount();
        crooms_count= _solution.CroomTableCount();
        classes_count= _solution.ClasseTableCount();
        prof_count= _solution.ProfTableCount();
        mat_count= _solution.MatTableCount();

        FMatMapArray=new CMapDayTable[classes_count];

        FMatProcessMap=new CProcessLevelRecord[mat_count];

        FMapClasseBitDT_A=new CBitDayTable[classes_count];
        FMapCroomBitDT_A=new CBitDayTable[crooms_count];
        FMapProfBitDT_A=new CBitDayTable[prof_count];

        FMapClasseBitDT_B=new CBitDayTable[classes_count];
        FMapCroomBitDT_B=new CBitDayTable[crooms_count];
        FMapProfBitDT_B=new CBitDayTable[prof_count];
        FTypeCroomMapArray=new  CDayTable[MAX_CROOMTYPE_COUNT];

        #if  OPTIMIZE_BRANCHING ==1
        FmhtLength_b=new CMapHourTable[9];
        FmhtLength_e=new CMapHourTable[9];

        FmdtMat_b=new CMapDayHoursTable[mat_count];
        FmdtMat_e=new CMapDayHoursTable[mat_count];
        #endif
    }
    catch(...)
    {
       
        Console::WriteEx("\nError loading engine!",ColorRed);
        return false;
    }
    return true;
}

bool emEngine::save(const char* fn){

   return solution().SaveToFile(fn);

}

int emEngine::shiftsCount(){
    return shifts_count;

}
bool  emEngine::verifySolution(bool b){
   return solution().verifyProcessedShifts(b);
}
bool  emEngine::getOptimizeValue(int *hp,int*hc,int*cp,int*cc){
   return solution().getOptimizeValue(hp,hc,cp,cc);
}

void emEngine::execute(bool _first ,bool _new)
{
    max_processed_shifts=0;
    max_prof_hc=0;
    max_classe_hc=0;
    time(&ptime);
    if (_first)
        stime=ptime;
    #if  OPTIMIZE_BRANCHING == 1
        printf("Optimize branching .....[enabled]\n");
    #else
        printf("Optimize branching .....[disabled]\n");
    #endif

        solution().SetTablesCount();
    highlevel_process_count=((int)_engineConfig.hltx*shifts_count / 100);
    lowlevel_process_count=((int) _engineConfig.lltx*shifts_count / 100);
    printf("\nhltx=%2.2f%% %d/%d\nlltx=%2.2f%% %d/%d\nmltx=%2.2f%%\n\n",
        _engineConfig.hltx,highlevel_process_count,shifts_count,
        _engineConfig.lltx,lowlevel_process_count,shifts_count,
        _engineConfig.mltx

          );

    for(tmpi=0; tmpi<mat_count; tmpi++)
    {
        FMatProcessMap[tmpi].iprocess=0;
        FMatProcessMap[tmpi].tx= _engineConfig.mltx;
        FMatProcessMap[tmpi].shiftCount= solution().getShiftsCount(tmpi,emMat);
        FMatProcessMap[tmpi].level=(int)(FMatProcessMap[tmpi].tx*FMatProcessMap[tmpi].shiftCount / 100);
#if ENGINE_DEBUGMODE_LEVEL >=0
        printf("mindex=%d   count=%d    level=%d\n",
               tmpi,
               FMatProcessMap[tmpi].shiftCount,
               FMatProcessMap[tmpi].level);
#endif
    }



    initialize(_first || _new);
    cursor_y=Console::GetCursorY();
    startSearching();
// Optimize;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void emEngine::doFill()
{
    int j,k,l;

    l=shifts_count;
    FProcessArray=new int[l];

    k=0;
    for (j=0 ; j<l; j++)
    {

        /*  if  ((goNotProcessLockedShifts in solution().GlobalOptions) &&
                    (doLocked in solution().shiftTable[j].Drawopt))
                 || (solution().ClasseTable[solution().shiftTable[j].CIndex].NotIncluded &&(goNotIncludedClasse in solution().GlobalOptions))
                 || (solution().ProfTable[solution().shiftTable[j].PIndex].NotIncluded &&(goNotIncludedProf in solution().GlobalOptions))
                 || (solution().MatTable[solution().shiftTable[j].MIndex].NotIncluded &&(goNotIncludedMat in solution().GlobalOptions))

               continue;

        */
        FProcessArray[k]=j ;
        //fix rank
        solution().ShiftTable()[j].rank=k;
        //reset delat
        solution().ShiftTable()[j].delta=8-solution().ShiftTable()[j].length;//(1+solution().ShiftTable()[j].every2weeks);
        k++;
    }

    // setlength(FProcessArray,k);
    FProcessArrayLength=k;

}

bool emEngine::doCheck()
{
    int j,p;

    for (p=0 ; p<FProcessArrayLength; p++)
        for (j=0 ; j<FProcessArrayLength; j++)
            if ((j!=p)&&(FProcessArray[p]==FProcessArray[j]))
            {

                Console::WriteEx("   ->check FProcessArray duplication error for index %d && %d =>%d!",ColorRed);
                // Console::Pause("press p to continue","p");
                return false;
            }
    return true;
}

unsigned long emEngine::getProcessArrayChecksum()
{

    unsigned long sum = _adler32((unsigned char*)FProcessArray,FProcessArrayLength);
    //sum =sum^0xFFFFFFFF;
    // printf("\n checksum=%ld\n\n",sum);
    return unsigned(sum);

}

bool emEngine::moveIndex(int i1,int i2)
{
    int  i,j, tmp,tmpI2;

    if ((i2>=i1)||(i1<1)||(i2<-1)||(i1>=shifts_count)||(i2>=shifts_count))
        return false;
    tmp=FProcessArray[i1];//save i1
    (solution().ShiftTable()[tmp].delta)++;
    tmpI2=i2;

    if (tmpI2==-1)
        for (j=0 ; j< i1; j++)
            if (solution().ShiftTable()[tmp].delta>solution().ShiftTable()[FProcessArray[j]].delta)
            {
                tmpI2=j;
                break;
            }


    if (tmpI2<0)
        return false;
    //translation
    for (i=i1; i> tmpI2; i--)
        FProcessArray[i]=FProcessArray[i-1];
    FProcessArray[tmpI2]=tmp; // fill i2 with i1
    return true;
}

void emEngine::swapValues(const int v,const int w)
{
    int tmp=FProcessArray[w] ;
    FProcessArray[w]=FProcessArray[v];
    FProcessArray[v]=tmp;

}

int emEngine::doPartition(const int p,const int r)
{

    //random pivot or not!!
    //int x=FProcessArray[rand() % FProcessArrayLength];
    int x=FProcessArray[r];

    tmpi=p-1;

    for(tmpj=p; tmpj<r; tmpj++)
    {
        if(solution().ShiftTable()[FProcessArray[tmpj]].delta<=solution().ShiftTable()[x].delta)
        {
            tmpi++;
            swapValues(tmpi,tmpj);
        }
    }
    swapValues(tmpi+1,r);
    return tmpi+1;

}

void emEngine::doQuickSort(const bool ascendant,const int p,const int r)
{

    if(p<r)
    {
        tmpk=doPartition(p,r);//pivot
        doQuickSort(ascendant,p,tmpk-1);
        doQuickSort(ascendant,tmpk+1,r);
    }

}

void emEngine::doSort(const bool ascendant)
{

    doQuickSort(true,0,FProcessArrayLength-1);

#if ENGINE_DEBUGMODE_LEVEL >=0
    int i;
    for (i=0; i<FProcessArrayLength; i++)
    {
        printf("%d   [%d]     rank=%d     length=%d   every2weeks=%d  delta=%d\n",
               i,
               FProcessArray[i],
               solution().ShiftTable()[FProcessArray[i]].rank,
               solution().ShiftTable()[FProcessArray[i]].length,
               solution().ShiftTable()[FProcessArray[i]].every2weeks,
               solution().ShiftTable()[FProcessArray[i]].delta
              );
    }
#else

#endif


}

void emEngine::doRandomize()
{
  
    for (int j=0 ; j<FProcessArrayLength; j++)
    {
        srand(time(0));
        swapValues(j,rand() % FProcessArrayLength);
        // moveIndex(j,rand()% FProcessArrayLength);
        // printf("random=%d\n",rand());
    }
}

void emEngine::doFillConstraintMap(){
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

   for (j=0 ; j<mat_count; j++)
    {
      FmdtMat_b[j][d][h]=0;
      FmdtMat_e[j][d][h]=0;
    }

    if(h<6){
            FmdtMat_b[7][d][h]=FMatProcessMap[7].level;//HG >11h


        }
    if(h<4){

            FmdtMat_b[8][d][h]=FMatProcessMap[8].level;//
            FmdtMat_b[9][d][h]=FMatProcessMap[9].level;//
            FmdtMat_b[10][d][h]=FMatProcessMap[10].level;//

        }
    if(h>8){
        FmdtMat_e[0][d][h]=FMatProcessMap[0].level;//math <12h
      if(d==0)
        FmdtMat_e[3][d][h]=FMatProcessMap[3].level;//arabe lundi <12h
      if(d<=1)
        FmdtMat_e[4][d][h]=FMatProcessMap[4].level;//francais lundi,mardi <12h
     if(d==3)
        FmdtMat_e[5][d][h]=FMatProcessMap[5].level;//
    }

    if(h>12){
        FmdtMat_e[0][d][h]=shifts_count;
        FmdtMat_e[13][d][h]=shifts_count; //ph <14h 100%
        }

    if(h>14){
        FmdtMat_e[6][d][h]=MAX_SHIFTS_COUNT;//sport <17h
    }

   }
#endif
}

void emEngine::initialize(const bool AReset)
{
    if (AReset)
    {
        process_percent=0;

        doFill();
        doCheck();
        //  if (goRandomizeShifts in solution().GlobalOptions)
        doRandomize();
        doSort(true);
        //FProcessArrayChecksum=0;
        FProcessArrayChecksum=getProcessArrayChecksum();
        #if  OPTIMIZE_BRANCHING ==1
        doFillConstraintMap();
        #endif


    }
    if (
        (   (process_speed>=1000) && (process_tries_count > (50000-(int)process_speed*10) )  )
        || ((process_tries_count-process_last_progession)>(1000+(int)process_percent*500))
    )
    {

        Console::WriteEx("\n        bad search path reprocessing..",ColorRed);
        cursor_y++;
        Sleep(1000);
        /* if(true || (process_percent<99.0)){
         doRandomize();

         }*/
        //doRandomize();
        doSort(true);

        process_tries_count=0;
        process_last_progession=process_tries_count;
        max_processed_shifts=0;
        max_prof_hc=0;
        max_classe_hc=0;
        time(&ptime);
        FProcessArrayChecksum=getProcessArrayChecksum();
    }

    for (tmpi=0 ; tmpi< MAX_CROOMTYPE_COUNT; tmpi++){
       tmpf=solution().CroomCountByType(tmpi);
      // printf("type croom:%d        count:%d\n",tmpi,tmpf);
       for (tmpj=0 ; tmpj<10; tmpj++)
          for (tmpk=0 ; tmpk<16; tmpk++)
            FTypeCroomMapArray[tmpi][tmpj][tmpk]=tmpf ;
    }



    for (tmpi=0 ; tmpi<classes_count; tmpi++)
        for (tmpj=0 ; tmpj<mat_count; tmpj++)
            for (tmpk=0 ; tmpk<6; tmpk++)
                FMatMapArray[tmpi][tmpj][tmpk]=0;

    if (FProcessArrayLength<shifts_count)
    {
        for (tmpi=0 ; tmpi<FProcessArrayLength; tmpi++)
            solution().clearShift(FProcessArray[tmpi],true);
    }
    else
    {
        //doit for all!

        for (tmpk=0 ; tmpk<=10; tmpk++)
        {
            for (tmpi=0; tmpi<classes_count; tmpi++)
            {
                FMapClasseBitDT_A[tmpi][tmpk]=0;
                FMapClasseBitDT_B[tmpi][tmpk]=0;
            }

            for (tmpi=0; tmpi<prof_count; tmpi++)
            {
                FMapProfBitDT_A[tmpi][tmpk]=0;
                FMapProfBitDT_B[tmpi][tmpk]=0;

            }

            for (tmpi=0; tmpi<crooms_count; tmpi++)
            {
                FMapCroomBitDT_A[tmpi][tmpk]=0;
                FMapCroomBitDT_B[tmpi][tmpk]=0;
            }

        }


        for (tmpi=0; tmpi<shifts_count; tmpi++)
        {

            solution().ShiftTable()[tmpi].day=-1;
            solution().ShiftTable()[tmpi].hour=-1;
            solution().ShiftTable()[tmpi].doAlternatewith=(int)foNoWhere;
            solution().ShiftTable()[tmpi].dogroupwith=-1;
            solution().ShiftTable()[tmpi].groupedwith=-1;
        }

    }
    for (tmpi=0; tmpi<mat_count; tmpi++)
    {
        FMatProcessMap[tmpi].iprocess=0;

    }
    curcindex=-1;
    curmindex=-1;
    curpindex=-1;

    prof_hc=0;
    classe_hc=0;
    PrDay=-1;
    PrHour=-1;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */

bool emEngine::startSearching()
{

    SYSTEMTIME st0,st1;
    int valp,valc,osp,osc;
    double diff_time;
    process_running=true;//! result;
    process_tries_count=0;
    process_last_progession=0;
    // NextShift
    iprocess=0;;
    PrHour=0;
    initStartDay();
    while (iprocess<FProcessArrayLength)
    {

#if ENGINE_DEBUGMODE_LEVEL >=1
        printf("[%d] Shift[%d] entering process()\n",iprocess,FProcessArray[iprocess]);
#else

#endif


        if (process()==true)
        {
            // NextShift
            iprocess++;
            PrHour=0;
        }
        else //! processed
        {
            PrHour++;
            if (PrHour>15) //reach last hour
            {

                PrHour=0;
                moveIndex(iprocess,-1/*max(0,2*i-FShiftTableCount)*/);
                initialize(false);
                last_max_processed_shifts=max_processed_shifts;
                max_processed_shifts=(iprocess>=max_processed_shifts)?iprocess:max_processed_shifts;
                 if(max_processed_shifts>last_max_processed_shifts)
                    process_last_progession= process_tries_count;
                iprocess=0;

                process_tries_count++;
        #if SLOW_PRINTING ==1
              if (process_tries_count % 101==0){
        #endif
                //solution().getOptimizeValue(&valp,&valc,&osp,&osc);
                diff_time=difftime(time(0),ptime);
                max_prof_hc=(max_prof_hc>prof_hc)?max_prof_hc:prof_hc;
                max_classe_hc=(max_classe_hc>classe_hc)?max_classe_hc:classe_hc;
                //if(diff_time>=2){



                //progression_tx=process_percent;
                process_percent=(double)max_processed_shifts/(double)shifts_count*100;

               /* progression_tx=(process_percent-progression_tx)/diff_time*100;
                if(progression_tx>0.0001)
                    process_last_progession = process_tries_count;
                */

                time_t2SystemTime(time(0)-ptime,&st0);
                time_t2SystemTime(time(0)-stime,&st1);

                process_speed=(double)(process_tries_count/diff_time);

                Console::SetCursorPosition(3,cursor_y);
                // printf("\n",FProcessArrayChecksum);
                printf("%06d  %c %02.3f%% %04d/%04d %04.0f/s p:%d %04d crc%012ld",
                       process_tries_count+1,
                       SPEED_SYM[process_tries_count % 4],

                       process_percent,
                       max_processed_shifts,
                       shifts_count,
                       process_speed,
                       max_prof_hc,
                       process_tries_count-process_last_progession,
                       FProcessArrayChecksum

                      );
                //Console::Write(s);
                //printf("",difftime(time(0),ptime),difftime(time(0),stime));
                printf(" %02d:%02d:%02d/%02d:%02d:%02d",
                       st0.wHour, st0.wMinute,st0.wSecond,
                       st1.wHour, st1.wMinute,st1.wSecond);

            #if SLOW_PRINTING ==1
                }
            #endif
            }//else
        } //if else  end ! processed


    }  //while
    all_processed_shifts=iprocess;
    if(all_processed_shifts==FProcessArrayLength)
    {
        solution().rebuildSolution();
        return true;
    }
    return false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool emEngine::process()
{
    bresult=false;
    //isForbiddenday=false;
    tmpgw=-1;
    tmpfo=foMixte;
    cursindex=FProcessArray[iprocess];
    curShift=&solution().ShiftTable()[cursindex];
    curlength=curShift->length;

    curmindex=curShift->mindex;
    curcindex=curShift->cindex;
    curpindex=curShift->pindex;
    curfday=solution().ProfTable()[curpindex].fday;

    if (curpindex==-1)
    {
        /* ShowMessage(format('FATAL ERROR: No "PROF" assigned for "Shift":%d "Mat":%d "Classe":%d',
        [cursindex,curmindex,curCindex])); */
        Console::Write("FATAL ERROR: No 'PROF' assigned for 'Shift' n:");
        Console::Write(cursindex);
        return false;
    }


    PrDay=PrStartDay-1;
    nextDay();
    PrCroom=0;

    tmpbitset=(1 << curlength)-1;
    tmpbitset=tmpbitset << (32-curlength-PrHour);

    while (PrCroom<crooms_count)
    {
        if (
            (!isForbiddenday)
            &&(curShift->type_croom==solution().CroomTable()[PrCroom].stype)
            &&(checkIsEmpty()!=-1)
        )
        {
            //TODO:check next && previous Shift
            bresult=fillCroom();
            if (bresult)
                break;
            else
                fillCroom(false) ;


        }

        PrCroom++;
        if (isForbiddenday ||(PrCroom>=crooms_count))
        {
            PrCroom=0;
            nextDay();
            if (PrDay==PrStartDay)
                break;


        }


    }  //end  while
#if ENGINE_DEBUGMODE_LEVEL >=2
    printf("    ->[%d] process()  result=[%d]\n",cursindex,bresult);
#else

#endif


    return bresult;

}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool emEngine::fillCroom(bool abool)
{
    bresult=false;
    if (abool)
    {

          if (tmpgw!=-1)
          {
            solution().setLink(tmpgw,cursindex,ltGroup);
          }
          curShift->saindex=PrCroom;
          curShift->hour=PrHour;
          curShift->day=PrDay;
          curShift->doAlternatewith=(int)(tmpfo);

       // solution().FillCroom(cursindex,PrCroom,PrDay,PrHour,tmpfo,tmpgw,true);

        if ((tmpfo==foMixte)||(tmpfo==foWeekA))
        {
            if (tmpgw==-1)
                FMapClasseBitDT_A[curcindex][PrDay]|= tmpbitset;
            FMapCroomBitDT_A[PrCroom][PrDay] |= tmpbitset;
            FMapProfBitDT_A[curpindex][PrDay]|= tmpbitset;

        }
        if ((tmpfo==foMixte)||(tmpfo==foWeekB))
        {
            if (tmpgw==-1)
                FMapClasseBitDT_B[curcindex][PrDay]|= tmpbitset;
            FMapCroomBitDT_B[PrCroom][PrDay] |= tmpbitset;
            FMapProfBitDT_B[curpindex][PrDay]|= tmpbitset;
        }

        //update FMatMapArray
        FMatMapArray[curcindex][curmindex][PrDay]=1;
        /* if(tmpgw!=-1)
             FMatMapArray[curcindex][solution().ShiftTable()[tmpgw].mindex][PrDay]=1;*/
        FMatProcessMap[curmindex].iprocess++;
        FTypeCroomMapArray[curShift->type_croom][PrDay][PrHour]--;
       /* if (after_hc==true)
            prof_hc++;
        if((prof_hc>0)&&(PrEndhour!=8)&&(PrEndhour!=16)&&(solution().ProfTable()[curpindex].weeka[PrDay][PrEndhour]!=-1))
            prof_hc--;
*/
        //dec delta
        //curShift->delta--;
        bresult=true;
    }
    else
    {
        /* curShift->saindex=-1;
         curShift->hour=-1;
         curShift->day=-1;
         curShift->doAlternatewith=(int)(foNoWhere);
          if (curShift->groupedwith!=-1)
         {
            solution().setLink(cursindex,curShift->groupedwith,ltClear);

         }

         */
        //solution().FillCroom(cursindex,PrCroom,PrDay,PrHour,tmpfo,tmpgw,false);

        //update FMatMapArray
        // FMatMapArray[curcindex][curmindex][PrDay]=0;

        bresult=true;
    }


    if (!bresult)
        Console::WriteEx("RepmliCroom failed!",ColorRed);
#if ENGINE_DEBUGMODE_LEVEL >=2
    else  printf("\t->[%d] RepmliCroom n:%d ....[ok]\n",cursindex,PrCroom);
#else

#endif


    return bresult;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emEngine::checkIsEmpty()
{
    iresultA=-1;
    iresult=-1;
    tmpgw=-1;

    every2weeks= curShift->every2weeks;

    tmpfo=foWeekA;

    CroomBitDT=&FMapCroomBitDT_A[PrCroom];
    ProfBitDT=&FMapProfBitDT_A[curpindex];
    ClasseBitDT=&FMapClasseBitDT_A[curcindex];
    class_se=solution().ClasseTable()[curcindex].weeka[PrDay][PrHour] ;//class  shift
    iresult=checkEmptyDT();
    if (((iresult==-1)&& every2weeks)|| ((iresult!=-1) && !every2weeks))
    {
        if (every2weeks)
            tmpfo=foWeekB;
        else
            tmpfo=foMixte;

        CroomBitDT=&FMapCroomBitDT_B[PrCroom];
        ProfBitDT=&FMapProfBitDT_B[curpindex];
        ClasseBitDT=&FMapClasseBitDT_B[curcindex];
        class_se=solution().ClasseTable()[curcindex].weekb[PrDay][PrHour] ;//class  shift
        iresultA=iresult;
        iresult=checkEmptyDT();
        if (!every2weeks && (iresult!=iresultA))
            iresult=-1 ;
    }


    if ((iresult!=-1) && (class_se!=-1) && bcangroup)
        tmpgw=class_se;
#if ENGINE_DEBUGMODE_LEVEL >=3
    printf("\t->[%d] checkIsEmpty  result=[%d] PrHour=[%d]\n",cursindex,iresult,PrHour);
#else

#endif


    return iresult;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
inline int emEngine::checkEmptyDT()
{

    bcangroup=solution().CanBeByGroup(cursindex,class_se,PrDay,PrHour);

    if(  (( ( (*ClasseBitDT)[PrDay] & tmpbitset) !=0 ) && ! bcangroup )||
            ( ( (*CroomBitDT)[PrDay] & tmpbitset) !=0 )  ||
            ( ( (*ProfBitDT)[PrDay] & tmpbitset) !=0 )
      )
        return -1;


    return PrHour;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void emEngine::initStartDay()
{
    srand(time(0))  ;
    PrStartDay=rand() % 5;
    //PrDay=PrStartDay;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void emEngine::nextDay()
{
    PrDay++;
    PrDay=PrDay % 6;
    PrEndhour=PrHour+curlength;
   // after_hc=(PrHour>0) && (PrHour!=8) && (solution().ProfTable()[curpindex].weeka[PrDay][PrHour-1]==-1);
    isForbiddenday=
        (PrDay==curfday)
        || (FMatMapArray[curcindex][curmindex][PrDay]==1)//no 2 mat in same day
        ||(FTypeCroomMapArray[curShift->type_croom][PrDay][PrHour]<=0)
        ||( PrEndhour>16) //soir
        || ((PrHour<8)&&( PrEndhour>8))//matinee

       #if  OPTIMIZE_BRANCHING ==0

        ||( ((curlength==4)||(curlength==3)) &&(PrHour % 4!=0)    )           //pour les shifts de 2h ou 1.5h
        ||( (curlength==8) &&(PrHour % 8!=0) )                            //.... de 4h
        ||( (PrHour % 2!=0) && (curlength!=3)    )                          // begin with half hour
        ||( (PrDay>3)&& (PrHour>=8) )                                   //no course for ven & sam soir
         //math-physique
        ||(  (curmindex==0) && (FMatProcessMap[0].iprocess<FMatProcessMap[0].level) && (PrEndhour>8)     )
        || ( ((curmindex==0)||(curmindex==13)||(curmindex==2)) && (PrEndhour>12)     )
        //prof
        ||(    (curpindex==0)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour>=8)
              // && (PrDay==0)
           )
         ||(    (curpindex==55)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrEndhour>12)
              // && (PrDay==0)
           )
         ||(    (curpindex==33)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour>=8)
              // && (PrDay==0)
           )
         ||(    (curpindex==49)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour>=8)
              // && (PrDay==0)
           )
        ||(    (curpindex==16)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour<8)
              // && (PrDay==0)
           )
         ||(    (curpindex==45)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour>=8)
             //  && (PrDay==0)
           )
        ||(    (curpindex==67)
               && (FMatProcessMap[curmindex].iprocess<FMatProcessMap[curmindex].level)
               && (PrHour>=8)
             //  && (PrDay==0)
           )
       /* */
        //arabe-francais-anglais
      /*  ||(  (curmindex==3) && (FMatProcessMap[3].iprocess<FMatProcessMap[3].level) && (PrEndhour>8) && (PrDay==0)    )
        ||(  (curmindex==4) && (FMatProcessMap[4].iprocess<FMatProcessMap[4].level) && (PrEndhour>8) && (PrDay==1)    )
        ||(  (curmindex==5) && (FMatProcessMap[5].iprocess<FMatProcessMap[5].level) && (PrEndhour>8) && (PrDay==0)    )

        //education nat/religion ap 10h
         || ( (FMatProcessMap[8].iprocess<FMatProcessMap[8].level)  && (((curmindex ==8)) && (PrEndhour<=8) && (PrDay==1) )   )
         || ( (FMatProcessMap[9].iprocess<FMatProcessMap[9].level)  && (((curmindex ==9)) && (PrEndhour<=8))  && (PrDay==2) )
         || ( (FMatProcessMap[10].iprocess<FMatProcessMap[10].level)  && (((curmindex ==10)) && (PrHour<4))   )

        //histoire-geo apres 10h
        || ( (curmindex==7) && (FMatProcessMap[7].iprocess<FMatProcessMap[7].level) && (PrHour<4)     )
        || ( (curmindex==7) && (FMatProcessMap[7].iprocess<FMatProcessMap[7].level)&& (PrEndhour<=8) && (PrDay<=1)     )
*/
        //sport
        || ( (curmindex==6) && (PrEndhour>14)    )
        //test
        // || ( (iprocess<highlevel_process_count) &&  (curpindex==33) && (PrDay<=2)&& (PrEndhour>8)        )

        #else
        ||(FmhtLength_b[curlength][PrDay][PrHour]==false)
        ||(FmhtLength_e[curlength][PrDay][PrEndhour-1]==false)
        ||(FMatProcessMap[curmindex].iprocess<FmdtMat_b[curmindex][PrDay][PrHour])
        ||(FMatProcessMap[curmindex].iprocess<FmdtMat_e[curmindex][PrDay][PrEndhour-1])

        #endif
         // heure de  pointe
        || ( (iprocess<lowlevel_process_count)&& (curlength==2) &&(PrEndhour>14)  )
        //heure creuse
      //  || ( (iprocess<lowlevel_process_count)&& ((PrHour>0) && (solution().ClasseTable()[curcindex].weeka[PrDay][PrHour-1]==-1))  )
      //  || ( (prof_hc>10)&& after_hc  )
        ;

}
