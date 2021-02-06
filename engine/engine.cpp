
#include "../3rdparty/Console/console.h"
#include "engine.h"
#include "utils.h"



emEngine::~emEngine()
{
    //dtor
}
bool emEngine::Release()
{
    delete this;
}
emEngine::emEngine()
{
    pluginInfo=new CEnginePlugin;
    pluginInfo=getPluginInfo();
}

bool emEngine::Load(CEngineConfig *ecfg){
  try
    {
        solution=CemSolution();
        econfig=( CEngineConfig *) malloc(sizeof(CEngineConfig));
        *econfig=*ecfg;
        solution.LoadFromFile(econfig->configfile);
        seances_count=solution.SeanceTableCount();
        salles_count=solution.SalleTableCount();
        classes_count=solution.ClasseTableCount();
        prof_count=solution.ProfTableCount();
        mat_count=solution.MatTableCount();

        FMatMapArray=new CMapDayTable[classes_count];

        FMatProcessMap=new CProcessLevelRecord[mat_count];

        FMapClasseBitDT_A=new CBitDayTable[classes_count];
        FMapSalleBitDT_A=new CBitDayTable[salles_count];
        FMapProfBitDT_A=new CBitDayTable[prof_count];

        FMapClasseBitDT_B=new CBitDayTable[classes_count];
        FMapSalleBitDT_B=new CBitDayTable[salles_count];
        FMapProfBitDT_B=new CBitDayTable[prof_count];
        FTypeSalleMapArray=new  CDayTable[MAX_SALLETYPE_COUNT];

        #if  OPTIMIZE_BRANCHING ==1
        FmhtDuree_b=new CMapHourTable[9];
        FmhtDuree_e=new CMapHourTable[9];

        FmdtMat_b=new CMapDayHoursTable[mat_count];
        FmdtMat_e=new CMapDayHoursTable[mat_count];
        #endif
    }
    catch(...)
    {
        delete &solution;
        Console::WriteEx("Error loading engine!",ColorRed);
        return false;
    }
    return true;
}
bool emEngine::Save(const char* fn){

   return solution.SaveToFile(fn);

}
CEnginePlugin*  emEngine::getPluginInfo(){

  //  pluginInfo->description="\0";
  //  pluginInfo->name=&ENGINENAME;
    pluginInfo->version_maj=1;
    pluginInfo->version_min=0;
    return pluginInfo;
}
int emEngine::SeancesCount(){
    return seances_count;

}
bool  emEngine::verifySolution(bool b){
   return solution.verifyProcessedSeances(b);
}
bool  emEngine::getOptimizeValue(int *hp,int*hc,int*cp,int*cc){
   return solution.getOptimizeValue(hp,hc,cp,cc);
}

void emEngine::execute(bool _first ,bool _new)
{
    max_processed_seances=0;
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

    solution.SetTablesCount();
    highlevel_process_count=(econfig->hltx*seances_count / 100);
    lowlevel_process_count=(econfig->lltx*seances_count / 100);
    printf("\nhltx=%2.2f%% %d/%d\nlltx=%2.2f%% %d/%d\nmltx=%2.2f%%\n\n",
           econfig->hltx,highlevel_process_count,seances_count,
           econfig->lltx,lowlevel_process_count,seances_count,
           econfig->mltx

          );

    for(tmpi=0; tmpi<mat_count; tmpi++)
    {
        FMatProcessMap[tmpi].iprocess=0;
        FMatProcessMap[tmpi].tx=econfig->mltx;
        FMatProcessMap[tmpi].seanceCount=solution.getSeancesCount(tmpi,emMat);
        FMatProcessMap[tmpi].level=(int)(FMatProcessMap[tmpi].tx*FMatProcessMap[tmpi].seanceCount / 100);
#if ENGINE_DEBUGMODE_LEVEL >=0
        printf("mindex=%d   count=%d    level=%d\n",
               tmpi,
               FMatProcessMap[tmpi].seanceCount,
               FMatProcessMap[tmpi].level);
#endif
    }



    Initialize(_first || _new);
    cursor_y=Console::GetCursorY();
    StartSearching();
// Optimize;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void emEngine::doFill()
{
    int j,k,l;

    l=seances_count;
    FProcessArray=new int[l];

    k=0;
    for (j=0 ; j<l; j++)
    {

        /*  if  ((goNotProcessLockedSeances in solution.GlobalOptions) &&
                    (doLocked in solution.seanceTable[j].Drawopt))
                 || (solution.ClasseTable[solution.seanceTable[j].CIndex].NotIncluded &&(goNotIncludedClasse in solution.GlobalOptions))
                 || (solution.ProfTable[solution.seanceTable[j].PIndex].NotIncluded &&(goNotIncludedProf in solution.GlobalOptions))
                 || (solution.MatTable[solution.seanceTable[j].MIndex].NotIncluded &&(goNotIncludedMat in solution.GlobalOptions))

               continue;

        */
        FProcessArray[k]=j ;
        //fix rang
        solution.SeanceTable()[j].rang=k;
        //reset delat
        solution.SeanceTable()[j].delta=8-solution.SeanceTable()[j].duree;//(1+solution.SeanceTable()[j].byquinz);
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

    if ((i2>=i1)||(i1<1)||(i2<-1)||(i1>=seances_count)||(i2>=seances_count))
        return false;
    tmp=FProcessArray[i1];//save i1
    (solution.SeanceTable()[tmp].delta)++;
    tmpI2=i2;

    if (tmpI2==-1)
        for (j=0 ; j< i1; j++)
            if (solution.SeanceTable()[tmp].delta>solution.SeanceTable()[FProcessArray[j]].delta)
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
        if(solution.SeanceTable()[FProcessArray[tmpj]].delta<=solution.SeanceTable()[x].delta)
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
        printf("%d   [%d]     rang=%d     duree=%d   byquinz=%d  delta=%d\n",
               i,
               FProcessArray[i],
               solution.SeanceTable()[FProcessArray[i]].rang,
               solution.SeanceTable()[FProcessArray[i]].duree,
               solution.SeanceTable()[FProcessArray[i]].byquinz,
               solution.SeanceTable()[FProcessArray[i]].delta
              );
    }
#else

#endif


}

void emEngine::doRandomize()
{
    int j;
    for (j=0 ; j<FProcessArrayLength; j++)
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
     FmhtDuree_b[0][d][h]=0;
     FmhtDuree_b[0][d][h]=0;
     FmhtDuree_b[1][d][h]=0;
     FmhtDuree_b[1][d][h]=0;

    for (j=2 ; j<9; j++){
    FmhtDuree_b[j][d][h]=1;
    FmhtDuree_e[j][d][h]=1;

    if((j!=3)&&( h % 2!=0))
        FmhtDuree_b[j][d][h]=0;

    if((d>3)&&(h>=8)){
        FmhtDuree_b[j][d][h]=0;
        FmhtDuree_e[j][d][h]=0;
    }

   }


 /*
        FmhtDuree_b[2][d][h]=((h % 2==0)&&( (h<=6)||((h>=8)&&(h<=13)) ));
        FmhtDuree_e[2][d][h]=((h % 2==0)&&( ((h>=2)&&(h<=8))||((h>=10)&&(h<=15)) ));
 */
        FmhtDuree_b[3][d][h]=(h % 4==0);
        FmhtDuree_b[4][d][h]=(h % 4==0);
        FmhtDuree_b[6][d][h]=(h==0)||(h==2)||(h==8)||(h==10);
        FmhtDuree_b[8][d][h]=(h % 8==0);

        FmhtDuree_b[5][d][h]=0;
        FmhtDuree_e[5][d][h]=0;

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
        FmdtMat_e[0][d][h]=seances_count;
        FmdtMat_e[13][d][h]=seances_count; //ph <14h 100%
        }

    if(h>14){
        FmdtMat_e[6][d][h]=MAX_SEANCES_COUNT;//sport <17h
    }

   }
#endif
}

void emEngine::Initialize(const bool AReset)
{
    if (AReset)
    {
        process_percent=0;

        doFill();
        doCheck();
        //  if (goRandomizeSeances in solution.GlobalOptions)
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
        max_processed_seances=0;
        max_prof_hc=0;
        max_classe_hc=0;
        time(&ptime);
        FProcessArrayChecksum=getProcessArrayChecksum();
    }

    for (tmpi=0 ; tmpi<MAX_SALLETYPE_COUNT; tmpi++){
       tmpf=solution.SalleCountByType(tmpi);
      // printf("type salle:%d        count:%d\n",tmpi,tmpf);
       for (tmpj=0 ; tmpj<10; tmpj++)
          for (tmpk=0 ; tmpk<16; tmpk++)
            FTypeSalleMapArray[tmpi][tmpj][tmpk]=tmpf ;
    }



    for (tmpi=0 ; tmpi<classes_count; tmpi++)
        for (tmpj=0 ; tmpj<mat_count; tmpj++)
            for (tmpk=0 ; tmpk<6; tmpk++)
                FMatMapArray[tmpi][tmpj][tmpk]=0;

    if (FProcessArrayLength<seances_count)
    {
        for (tmpi=0 ; tmpi<FProcessArrayLength; tmpi++)
            solution.viderSeance2(FProcessArray[tmpi],true);
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

            for (tmpi=0; tmpi<salles_count; tmpi++)
            {
                FMapSalleBitDT_A[tmpi][tmpk]=0;
                FMapSalleBitDT_B[tmpi][tmpk]=0;
            }

        }


        for (tmpi=0; tmpi<seances_count; tmpi++)
        {

            solution.SeanceTable()[tmpi].day=-1;
            solution.SeanceTable()[tmpi].hour=-1;
            solution.SeanceTable()[tmpi].doquinzwith=(int)foNoWhere;
            solution.SeanceTable()[tmpi].dogroupwith=-1;
            solution.SeanceTable()[tmpi].groupedwith=-1;
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

bool emEngine::StartSearching()
{

    SYSTEMTIME st0,st1;
    int valp,valc,osp,osc;
    double diff_time;
    process_running=true;//! result;
    process_tries_count=0;
    process_last_progession=0;
    // NextSeance
    iprocess=0;;
    PrHour=0;
    initStartDay();
    while (iprocess<FProcessArrayLength)
    {

#if ENGINE_DEBUGMODE_LEVEL >=1
        printf("[%d] Seance[%d] entering process()\n",iprocess,FProcessArray[iprocess]);
#else

#endif


        if (Process()==true)
        {
            // NextSeance
            iprocess++;
            PrHour=0;
        }
        else //! processed
        {
            PrHour++;
            if (PrHour>15) //reach last hour
            {

                PrHour=0;
                moveIndex(iprocess,-1/*max(0,2*i-FSeanceTableCount)*/);
                Initialize(false);
                last_max_processed_seances=max_processed_seances;
                max_processed_seances=(iprocess>=max_processed_seances)?iprocess:max_processed_seances;
                 if(max_processed_seances>last_max_processed_seances)
                    process_last_progession= process_tries_count;
                iprocess=0;

                process_tries_count++;
        #if SLOW_PRINTING ==1
              if (process_tries_count % 101==0){
        #endif
                //solution.getOptimizeValue(&valp,&valc,&osp,&osc);
                diff_time=difftime(time(0),ptime);
                max_prof_hc=(max_prof_hc>prof_hc)?max_prof_hc:prof_hc;
                max_classe_hc=(max_classe_hc>classe_hc)?max_classe_hc:classe_hc;
                //if(diff_time>=2){



                //progression_tx=process_percent;
                process_percent=(double)max_processed_seances/(double)seances_count*100;

               /* progression_tx=(process_percent-progression_tx)/diff_time*100;
                if(progression_tx>0.0001)
                    process_last_progession = process_tries_count;
                */

                time_t2SystemTime(time(0)-ptime,&st0);
                time_t2SystemTime(time(0)-stime,&st1);

                process_speed=(double)(process_tries_count/diff_time);

                Console::SetCursorPosition(3,cursor_y);
                // printf("\n",FProcessArrayChecksum);
                printf("%06d  %c %02.3f%% %04d/%04d %04.0f%/s p:%d %04d crc%012ld",
                       process_tries_count+1,
                       SPEED_SYM[process_tries_count % 4],

                       process_percent,
                       max_processed_seances,
                       seances_count,
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
    all_processed_seances=iprocess;
    if(all_processed_seances==FProcessArrayLength)
    {
        solution.rebuildSolution();
        return true;
    }
    return false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool emEngine::Process()
{
    bresult=false;
    //isForbiddenday=false;
    tmpgw=-1;
    tmpfo=foMixte;
    cursindex=FProcessArray[iprocess];
    curSeance=&solution.SeanceTable()[cursindex];
    curduree=curSeance->duree;

    curmindex=curSeance->mindex;
    curcindex=curSeance->cindex;
    curpindex=curSeance->pindex;
    curfday=solution.ProfTable()[curpindex].fday;

    if (curpindex==-1)
    {
        /* ShowMessage(format('FATAL ERROR: No "PROF" assigned for "Seance":%d "Mat":%d "Classe":%d',
        [cursindex,curmindex,curCindex])); */
        Console::Write("FATAL ERROR: No 'PROF' assigned for 'Seance' n:");
        Console::Write(cursindex);
        return false;
    }


    PrDay=PrStartDay-1;
    NextDay();
    PrSalle=0;

    tmpbitset=(1 << curduree)-1;
    tmpbitset=tmpbitset << (32-curduree-PrHour);

    while (PrSalle<salles_count)
    {
        if (
            (!isForbiddenday)
            &&(curSeance->type_salle==solution.SalleTable()[PrSalle].stype)
            &&(CheckIsEmpty()!=-1)
        )
        {
            //TODO:check next && previous Seance
            bresult=RempliSalle();
            if (bresult)
                break;
            else
                RempliSalle(false) ;


        }

        PrSalle++;
        if (isForbiddenday ||(PrSalle>=salles_count))
        {
            PrSalle=0;
            NextDay();
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
bool emEngine::RempliSalle(bool abool)
{
    bresult=false;
    if (abool)
    {

          if (tmpgw!=-1)
          {
            solution.setLink(tmpgw,cursindex,ltGroup);
          }
          curSeance->saindex=PrSalle;
          curSeance->hour=PrHour;
          curSeance->day=PrDay;
          curSeance->doquinzwith=(int)(tmpfo);

       // solution.RempliSalle(cursindex,PrSalle,PrDay,PrHour,tmpfo,tmpgw,true);

        if ((tmpfo==foMixte)||(tmpfo==foSemaineA))
        {
            if (tmpgw==-1)
                FMapClasseBitDT_A[curcindex][PrDay]|= tmpbitset;
            FMapSalleBitDT_A[PrSalle][PrDay] |= tmpbitset;
            FMapProfBitDT_A[curpindex][PrDay]|= tmpbitset;

        }
        if ((tmpfo==foMixte)||(tmpfo==foSemaineB))
        {
            if (tmpgw==-1)
                FMapClasseBitDT_B[curcindex][PrDay]|= tmpbitset;
            FMapSalleBitDT_B[PrSalle][PrDay] |= tmpbitset;
            FMapProfBitDT_B[curpindex][PrDay]|= tmpbitset;
        }

        //update FMatMapArray
        FMatMapArray[curcindex][curmindex][PrDay]=1;
        /* if(tmpgw!=-1)
             FMatMapArray[curcindex][solution.SeanceTable()[tmpgw].mindex][PrDay]=1;*/
        FMatProcessMap[curmindex].iprocess++;
        FTypeSalleMapArray[curSeance->type_salle][PrDay][PrHour]--;
       /* if (after_hc==true)
            prof_hc++;
        if((prof_hc>0)&&(PrEndhour!=8)&&(PrEndhour!=16)&&(solution.ProfTable()[curpindex].weeka[PrDay][PrEndhour]!=-1))
            prof_hc--;
*/
        //dec delta
        //curSeance->delta--;
        bresult=true;
    }
    else
    {
        /* curSeance->saindex=-1;
         curSeance->hour=-1;
         curSeance->day=-1;
         curSeance->doquinzwith=(int)(foNoWhere);
          if (curSeance->groupedwith!=-1)
         {
            solution.setLink(cursindex,curSeance->groupedwith,ltClear);

         }

         */
        //solution.RempliSalle(cursindex,PrSalle,PrDay,PrHour,tmpfo,tmpgw,false);

        //update FMatMapArray
        // FMatMapArray[curcindex][curmindex][PrDay]=0;

        bresult=true;
    }


    if (!bresult)
        Console::WriteEx("RepmliSalle failed!",ColorRed);
#if ENGINE_DEBUGMODE_LEVEL >=2
    else  printf("\t->[%d] RepmliSalle n:%d ....[ok]\n",cursindex,PrSalle);
#else

#endif


    return bresult;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emEngine::CheckIsEmpty()
{
    iresultA=-1;
    iresult=-1;
    tmpgw=-1;

    byquinz= curSeance->byquinz;

    tmpfo=foSemaineA;

    SalleBitDT=&FMapSalleBitDT_A[PrSalle];
    ProfBitDT=&FMapProfBitDT_A[curpindex];
    ClasseBitDT=&FMapClasseBitDT_A[curcindex];
    class_se=solution.ClasseTable()[curcindex].weeka[PrDay][PrHour] ;//class  seance
    iresult=checkEmptyDT();
    if (((iresult==-1)&& byquinz)|| ((iresult!=-1) && !byquinz))
    {
        if (byquinz)
            tmpfo=foSemaineB;
        else
            tmpfo=foMixte;

        SalleBitDT=&FMapSalleBitDT_B[PrSalle];
        ProfBitDT=&FMapProfBitDT_B[curpindex];
        ClasseBitDT=&FMapClasseBitDT_B[curcindex];
        class_se=solution.ClasseTable()[curcindex].weekb[PrDay][PrHour] ;//class  seance
        iresultA=iresult;
        iresult=checkEmptyDT();
        if (!byquinz && (iresult!=iresultA))
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

    bcangroup=solution.CanBeByGroup(cursindex,class_se,PrDay,PrHour);

    if(  (( ( (*ClasseBitDT)[PrDay] & tmpbitset) !=0 ) && ! bcangroup )||
            ( ( (*SalleBitDT)[PrDay] & tmpbitset) !=0 )  ||
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
void emEngine::NextDay()
{
    PrDay++;
    PrDay=PrDay % 6;
    PrEndhour=PrHour+curduree;
   // after_hc=(PrHour>0) && (PrHour!=8) && (solution.ProfTable()[curpindex].weeka[PrDay][PrHour-1]==-1);
    isForbiddenday=
        (PrDay==curfday)
        || (FMatMapArray[curcindex][curmindex][PrDay]==1)//no 2 mat in same day
        ||(FTypeSalleMapArray[curSeance->type_salle][PrDay][PrHour]<=0)
        ||( PrEndhour>16) //soir
        || ((PrHour<8)&&( PrEndhour>8))//matinee

       #if  OPTIMIZE_BRANCHING ==0

        ||( ((curduree==4)||(curduree==3)) &&(PrHour % 4!=0)    )           //pour les seances de 2h ou 1.5h
        ||( (curduree==8) &&(PrHour % 8!=0) )                            //.... de 4h
        ||( (PrHour % 2!=0) && (curduree!=3)    )                          // begin with half hour
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
        ||(FmhtDuree_b[curduree][PrDay][PrHour]==false)
        ||(FmhtDuree_e[curduree][PrDay][PrEndhour-1]==false)
        ||(FMatProcessMap[curmindex].iprocess<FmdtMat_b[curmindex][PrDay][PrHour])
        ||(FMatProcessMap[curmindex].iprocess<FmdtMat_e[curmindex][PrDay][PrEndhour-1])

        #endif
         // heure de  pointe
        || ( (iprocess<lowlevel_process_count)&& (curduree==2) &&(PrEndhour>14)  )
        //heure creuse
      //  || ( (iprocess<lowlevel_process_count)&& ((PrHour>0) && (solution.ClasseTable()[curcindex].weeka[PrDay][PrHour-1]==-1))  )
      //  || ( (prof_hc>10)&& after_hc  )
        ;

}
