#include <cstring>
#include <ctime>
#include "console.h"
#include "engine.h"


void time_t2FileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}
void time_t2SystemTime( time_t t, LPSYSTEMTIME sft )
{
    FILETIME ft;
    time_t2FileTime(t, &ft );
    FileTimeToSystemTime(&ft,sft);

}
emEngine::emEngine(char* cf)
{

    emploi=emSolution();
    try
    {
        emploi.LoadFromFile(cf);
        seances_count=emploi.SeanceTableCount();
        salles_count=emploi.SalleTableCount();
        classes_count=emploi.ClasseTableCount();
        prof_count=emploi.ProfTableCount();
        mat_count=emploi.MatTableCount();


    }
    catch(...)
    {
        delete &emploi;
        Console::WriteEx("Error loading engine!",ColorRed);
    }
}

emEngine::~emEngine()
{
    //dtor
}
void emEngine::execute(bool _first ,bool _new,double hltx,double lltx)
{
    max_processed_seances=0;

    time(&ptime);
    if (_first)
        stime=ptime;
    emploi.SetTablesCount();
    highlevel_process_count=(hltx*seances_count / 100);
    lowlevel_process_count=(lltx*seances_count / 100);
    printf("\hltx=%2.2f%% %d/%d\nlltx=%2.2f%% %d/%d\n\n",
           hltx,highlevel_process_count,seances_count,
           lltx,lowlevel_process_count,seances_count

          );

    Initialize(_first || _new);
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

        /*  if  ((goNotProcessLockedSeances in emploi.GlobalOptions) &&
                    (doLocked in emploi.seanceTable[j].Drawopt))
                 || (emploi.ClasseTable[emploi.seanceTable[j].CIndex].NotIncluded &&(goNotIncludedClasse in emploi.GlobalOptions))
                 || (emploi.ProfTable[emploi.seanceTable[j].PIndex].NotIncluded &&(goNotIncludedProf in emploi.GlobalOptions))
                 || (emploi.MatTable[emploi.seanceTable[j].MIndex].NotIncluded &&(goNotIncludedMat in emploi.GlobalOptions))

               continue;

        */
        FProcessArray[k]=j ;
        //fix rang
        emploi.SeanceTable()[j].rang=k;
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
void emEngine::swapValues(const int v,const int w)
{
    int j;
    j=FProcessArray[w] ;
    FProcessArray[w]=FProcessArray[v];;
    FProcessArray[v]=j;

}
bool emEngine::moveIndex(int i1,int i2)
{
    int  i,j, tmp,tmpI2;

    if ((i2>=i1)||(i1<1)||(i2<-1)||(i1>=seances_count)||(i2>=seances_count))
        return false;
    tmp=FProcessArray[i1];//save i1
    (emploi.SeanceTable()[tmp].delta)++;
    tmpI2=i2;

    if (tmpI2==-1)
        for (j=0 ; j< i1; j++)
            if (emploi.SeanceTable()[tmp].delta>emploi.SeanceTable()[FProcessArray[j]].delta)
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
void emEngine::doRandomize()
{
    int j;
    for (j=0 ; j<FProcessArrayLength; j++)
    {
        srand(time(0));
        swapValues(j,rand() % FProcessArrayLength);
    }
}
void emEngine::doSort(const bool ascendant)
{
    int j;
    for (j=0 ; j<FProcessArrayLength; j++)
    {

    }
}
void emEngine::Initialize(const bool AReset)
{
    int i,j,k,PrHour,ADay;

    if (AReset)
    {

        process_percent=0;
        doFill();
        doCheck();

        //  if (goRandomizeSeances in emploi.GlobalOptions)
        doRandomize();
        //  if (goSortByDureeSeances in emploi.GlobalOptions)
        doSort(true);
    }
    /* if (!AReset && (goAllwaysRandomizeSeances in emploi.GlobalOptions ))
         doRandomize;*/
    for (i=0 ; i<classes_count; i++)
        for (j=0 ; j<mat_count; j++)
            for (k=0 ; k<6; k++)
                FMatMapArray[i][j][k]=0;
    if (FProcessArrayLength<seances_count)
    {
        for (i=0 ; i<FProcessArrayLength; i++)
            emploi.viderSeance2(FProcessArray[i],true);
    }
    else
    {
        //doit for all!

        for (PrDay=0 ; PrDay<=10; PrDay++)
            for (PrHour=0; PrHour<=15; PrHour++)
            {

                for (i=0; i<classes_count; i++)
                {
                    emploi.ClasseTable()[i].weeka[PrDay][PrHour]=-1;
                    emploi.ClasseTable()[i].weekb[PrDay][PrHour]=-1;
                }

                for (i=0; i<prof_count; i++)
                {
                    emploi.ProfTable()[i].weeka[PrDay][PrHour]=-1;
                    emploi.ProfTable()[i].weekb[PrDay][PrHour]=-1;
                }

                for (i=0; i<salles_count; i++)
                {
                    emploi.SalleTable()[i].weeka[PrDay][PrHour]=-1;
                    emploi.SalleTable()[i].weekb[PrDay][PrHour]=-1;
                }
            }

        for (i=0; i<seances_count; i++)
        {
            emploi.SeanceTable()[i].groupedwith=-1;
        }

    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool emEngine::StartSearching()
{

    SYSTEMTIME st0,st1;


    char* sym="";
    double diff_time;
    process_running=true;//! result;
    process_tries_count=0;

    // NextSeance
    iprocess=0;;
    PrHour=0;
    initStartDay();
    while (iprocess<FProcessArrayLength)
    {
        //if (process_percent>80.00) PrStartDay=rand() % 5;
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

                max_processed_seances=(iprocess>=max_processed_seances)?iprocess:max_processed_seances;
                iprocess=0;
                process_tries_count++;


                switch (process_tries_count % 4)
                {
                case 0:
                    sym="-";
                    break;
                case 1:
                    sym="\\";
                    break;
                case 2:
                    sym="|";
                    break;
                case 3:
                    sym="/";
                    break;
                }

                //if (process_tries_count % 10=0){
                //write empty line
                Console::SetCursorPosition(3,Console::GetCursorY());
                Console::Write("                                                                ");
                Console::SetCursorPosition(3,Console::GetCursorY());
                diff_time=difftime(time(0),ptime);
                progression_tx=process_percent;
                process_percent=(double)max_processed_seances/(double)seances_count*100;
                progression_tx=(process_percent-progression_tx)/diff_time*100;
                time_t2SystemTime(time(0)-ptime,&st0);
                time_t2SystemTime(time(0)-stime,&st1);

                process_speed=(double)(process_tries_count/diff_time);
                printf("Passe:%06d    %s   %02.4f%% (%03.0f%/s %03.0f%/s)",
                       process_tries_count+1,
                       sym,
                       process_percent,
                       process_speed,
                       progression_tx

                      );
                //Console::Write(s);
                //printf("",difftime(time(0),ptime),difftime(time(0),stime));
                printf("    %02d:%02d:%02d/%02d:%02d:%02d",
                       st0.wHour, st0.wMinute,st0.wSecond,
                       st1.wHour, st1.wMinute,st1.wSecond);


            }//else
        } //if else  end ! processed


    }  //while
    all_processed_seances=iprocess;
    return (all_processed_seances==FProcessArrayLength);
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
    curduree=emploi.SeanceTable()[cursindex].duree;

    curmindex=emploi.SeanceTable()[cursindex].mindex;
    curcindex=emploi.SeanceTable()[cursindex].cindex;
    curpindex=emploi.SeanceTable()[cursindex].pindex;
    curfday=emploi.ProfTable()[curpindex].fday;
    if (curpindex==-1)
    {
        /* ShowMessage(format('FATAL ERROR: No "PROF" assigned for "Seance":%d "Mat":%d "Classe":%d',
        [cursindex,curmindex,curCindex])); */
        Console::Write("FATAL ERROR: No 'PROF' assigned for 'Seance' n°:");
        Console::Write(cursindex);
        return false;
    }

    //initStartDay();
    PrDay=PrStartDay;
    PrSalle=0;
    while (PrSalle<salles_count)
    {
        isForbiddenday=(PrDay==curfday)||
                       (FMatMapArray[curcindex][curmindex][PrDay]==1);//no 2 mat in same day
        /*if (emploi.ProfTable()[curpindex].fday==PrDay)
              NextDay();
        */
        if (
            (!isForbiddenday)
            &&(emploi.SeanceTable()[cursindex].type_salle==emploi.SalleTable()[PrSalle].stype)
            &&(CheckIsEmpty()!=-1))
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

    int i;
    bool result=false;



    if (abool)
    {

        if (tmpgw!=-1)
        {
            emploi.setLink(cursindex,tmpgw,ltGroup);
            //update FMatMapArray
            FMatMapArray[curcindex][emploi.SeanceTable()[tmpgw].mindex][PrDay]=1;
        }
        emploi.SeanceTable()[cursindex].saindex=PrSalle;   //fill with salle ind
        emploi.SeanceTable()[cursindex].hour=PrHour;
        emploi.SeanceTable()[cursindex].day=PrDay;

        for (i= 0; i<curduree; i++)
        {
            if ((tmpfo==foMixte)||(tmpfo==foSemaineA))
            {
                if (tmpgw==-1)
                    emploi.ClasseTable()[curcindex].weeka[PrDay][PrHour+i]=cursindex;
                emploi.ProfTable()[curpindex].weeka[PrDay][PrHour+i]=cursindex;
                emploi.SalleTable()[PrSalle].weeka[PrDay][PrHour+i]=cursindex;
            }
            if ((tmpfo==foMixte)||(tmpfo==foSemaineB))
            {
                if (tmpgw==-1)
                    emploi.ClasseTable()[curcindex].weekb[PrDay][PrHour+i]=cursindex;
                emploi.ProfTable()[curpindex].weekb[PrDay][PrHour+i]=cursindex;
                emploi.SalleTable()[PrSalle].weekb[PrDay][PrHour+i]=cursindex;
            }
            //update FMatMapArray
            FMatMapArray[curcindex][curmindex][PrDay]=1;

        }
    }
    else
    {

        for (i= 0; i<curduree; i++)
        {

            if ((tmpfo==foMixte)||(tmpfo==foSemaineA))
            {
                emploi.SalleTable()[PrSalle].weeka[PrDay][PrHour+i]=-1;
                emploi.ClasseTable()[curcindex].weeka[PrDay][PrHour+i]=-1;
                emploi.ProfTable()[curpindex].weeka[PrDay][PrHour+i]=-1;
            }
            if ((tmpfo==foMixte)||(tmpfo==foSemaineB))
            {
                emploi.SalleTable()[PrSalle].weekb[PrDay][PrHour+i]=-1;
                emploi.ClasseTable()[curcindex].weekb[PrDay][PrHour+i]=-1;
                emploi.ProfTable()[curpindex].weekb[PrDay][PrHour+i]=-1;
            }

        }
        if (tmpgw!=-1)
        {
            emploi.setLink(cursindex,emploi.SeanceTable()[cursindex].groupedwith,ltClear);
            //update FMatMapArray
            FMatMapArray[curcindex][emploi.SeanceTable()[tmpgw].mindex][PrDay]=0;
        }
        emploi.SeanceTable()[cursindex].saindex=-1;
        emploi.SeanceTable()[cursindex].hour=-1;
        emploi.SeanceTable()[cursindex].day=-1;
        //update FMatMapArray
        FMatMapArray[curcindex][curmindex][PrDay]=0;
    }
    result=true;

    if (!result)
        Console::WriteEx("RepmliSalle failed!",ColorRed);
   #if ENGINE_DEBUGMODE_LEVEL >=2
     else  printf("    ->[%d] RepmliSalle n°:%d ....[ok]\n",cursindex,PrSalle);
     #else

     #endif


    return result;

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

    byquinz= emploi.SeanceTable()[cursindex].byquinz;
    PrEndhour=PrHour+curduree;

    tmpfo=foSemaineA;
    SalleDT=&(emploi.SalleTable()[PrSalle].weeka) ;
    ProfDT=&(emploi.ProfTable()[curpindex].weeka) ;
    ClasseDT=&(emploi.ClasseTable()[curcindex].weeka) ;
    iresult=checkEmptyDT();
    if (((iresult==-1)&& byquinz)|| ((iresult!=-1) && !byquinz))
    {
        if (byquinz)
            tmpfo=foSemaineB;
        else
            tmpfo=foMixte;
        SalleDT=&(emploi.SalleTable()[PrSalle].weekb) ;
        ProfDT=&(emploi.ProfTable()[curpindex].weekb)  ;
        ClasseDT=&(emploi.ClasseTable()[curcindex].weekb) ;
        iresultA=iresult;
        iresult=checkEmptyDT();
        if (!byquinz && (iresult!=iresultA))
            iresult=-1 ;
    }


    if ((iresult!=-1) && (class_se!=-1) && bcangroup)
        tmpgw=class_se;
    #if ENGINE_DEBUGMODE_LEVEL >=3
       printf("    ->[%d] checkIsEmpty  result=[%d] PrHour=[%d]\n",cursindex,iresult,PrHour);
       #else

       #endif


    return iresult;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
FORCEINLINE int emEngine::checkEmptyDT()
{
    //int i,t1,t2;

    iresult=-1;
    //Sa=&(emploi.SalleTable()[PrSalle]);
    if(
        ((PrHour<8)&&( PrEndhour>8))||      //matinee
        ( PrEndhour>16) ||     //soir
        (((curduree==4)||(curduree==3)) &&(PrHour % 4!=0))||         //pour les seances de 2h ou 1.5h
        ((curduree==8) &&(PrHour % 8!=0))||  //.... de 4h
        ((PrHour % 2!=0) && (curduree!=3)) ||                  // begin with half hour
        ((PrDay>3)&& (PrHour>=8))     //no course for ven & sam soir
    )
        return -1;

    sse=(*SalleDT)[PrDay][PrHour] ;// salle seance index
    pse=(*ProfDT)[PrDay][PrHour] ; //prof  seance
    class_se=(*ClasseDT)[PrDay][PrHour] ;//class  seance
    bcangroup=emploi.CanBeByGroup(cursindex,class_se,PrHour);

    bool bbool=(
                   (sse!=-1)||
                   (pse!=-1)||
                   //! Class is busy for cur index
                   ((class_se!=-1) && ! bcangroup )
                   //no sport after 17h

                   ||
                   (
                       //(ooNoSportAfter17h in emploi.ClassOptions) &&
                       //(processed_seances<highlevel_process_count) &&
                       (curmindex==6) && (PrEndhour>14)

                   )
                   ||

                   //no class after sport
                   (
                       //(ooNoSportBeforeClass in emploi.ClassOptions)&&
                       (iprocess< lowlevel_process_count)  &&
                       (
                           ((PrHour>0)&&(emploi.SeanceTable()[(*ClasseDT)[PrDay][PrHour-1]].mindex==6))
                           ||
                           ((curmindex==6)&&((*ClasseDT)[PrDay][PrEndhour]!=-1))
                       )
                   )

                   //math-physique before 16h
                   ||
                   ((iprocess<highlevel_process_count) &&(curmindex==0) && (PrEndhour>8))

                   ||
                   ((iprocess<highlevel_process_count) &&(curmindex==13) && (PrEndhour>12))
                   ||
                   (((curmindex==0)||(curmindex==13)) && (PrEndhour>14))

                   ||
                   //arabe-fr-ang after 10h
                   (

                       (iprocess< lowlevel_process_count)  &&
                       (((curmindex ==3) ||(curmindex==4) ||(curmindex == 5)) && (PrHour<4) &&(curduree==2))
                   )
                   ||
                   //education nat/religion ap 10h
                   (

                       (iprocess< lowlevel_process_count)  &&
                       (((curmindex >=8) && (curmindex<=10)) && (PrHour<4))
                   )
                   //histoire-geo apres 10h
                   ||
                   ((iprocess<lowlevel_process_count) &&(curmindex==7) && (PrEndhour<6))

                   // heure de  pointe
                   ||
                   (
                       // ((ooNoHeurePointe in emploi.ProfOptions)||(ooNoHeurePointe in emploi.ClassOptions)) &&
                       (iprocess<lowlevel_process_count) &&  (curduree==2)&&(PrEndhour>14)
                   )

                   /*
                   //heure creuse
                   ||
                   (
                    ((goClassOptimize in emploi.GlobalOptions)||(ooNoHeureCreuse in emploi.ClassOptions))&&
                    (Processed_Seances<HighLevel_Process_count) &&
                    (getCompactIdx(curCindex,emClasse)<2)
                   )
                   ||
                   (
                    (ooNoHeureCreuse in emploi.ProfOptions)    &&
                    (Processed_Seances<HighLevel_Process_count) &&
                    (getCompactIdx(curPindex,emProf)<2)

                    ) */
               );
    if(bbool==false)
    {

        //check no 2 mat in same day
        /* for (i=0;i<15;i++)
         {
          t1=(*ClasseDT)[PrDay][i];
          if (t1!=-1)
             {
              t2=emploi.SeanceTable()[t1].groupedwith ;
             if (  (emploi.SeanceTable()[t1].mindex==curmindex)
                ||((t2!=-1)&&(emploi.SeanceTable()[t2].mindex==curmindex))
               )
                  return -1;
            }
        }
             */
        //check can fill seance in salle

        for (tmpi=0 ; tmpi<curduree; tmpi++)
            if (
                (((*SalleDT)[PrDay][PrHour+tmpi]!=-1))||
                (((*ProfDT)[PrDay][PrHour+tmpi]!=-1))||
                (((*ClasseDT)[PrDay][PrHour+tmpi]!=-1) && ! bcangroup)
            )
                break;
            else if (tmpi==curduree-1)
                iresult=PrHour;



    }
    return iresult;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
FORCEINLINE void emEngine::initStartDay()
{
    srand(time(0))  ;
    PrStartDay=rand() % 5;
    //PrDay=PrStartDay;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
FORCEINLINE void emEngine::NextDay()
{
    PrDay++;
    PrDay=PrDay % 6;
}
