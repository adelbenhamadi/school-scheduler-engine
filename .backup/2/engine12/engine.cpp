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
emEngine::emEngine(){

    pluginInfo=new CEnginePlugin;
    pluginInfo=getPluginInfo();

}
emEngine::~emEngine()
{
    //dtor
}
bool emEngine::Release()
{
    delete this;
}
bool emEngine::Load(char* cf)
{


    try
    {
        emploi=emSolution();
        emploi.LoadFromFile(cf);
        seances_count=emploi.SeanceTableCount();
        salles_count=emploi.SalleTableCount();
        classes_count=emploi.ClasseTableCount();
        prof_count=emploi.ProfTableCount();
        mat_count=emploi.MatTableCount();

        FMatMapArray=new CMapDayTable[classes_count];

        FMatProcessMap=new CProcessLevelRecord[mat_count];

    }
    catch(...)
    {
        delete &emploi;
        Console::WriteEx("Error loading engine!",ColorRed);
    }
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
bool emEngine::Save(const char* fn){

   return emploi.SaveToFile(fn);

}

bool  emEngine::verifySolution(bool b){
   return emploi.verifyProcessedSeances(b);
}
bool  emEngine::getOptimizeValue(int *hp,int*hc,int*cp,int*cc){
   return emploi.getOptimizeValue(hp,hc,cp,cc);
}

void emEngine::execute(bool _first ,bool _new,double hltx,double lltx,double mltx)
{
    max_processed_seances=0;

    time(&ptime);
    if (_first)
        stime=ptime;
    emploi.SetTablesCount();
    highlevel_process_count=(hltx*seances_count / 100);
    lowlevel_process_count=(lltx*seances_count / 100);
    printf("hltx=%2.2f%% %d/%d\nlltx=%2.2f%% %d/%d\nmltx=%2.2f%%\n",
           hltx,highlevel_process_count,seances_count,
           lltx,lowlevel_process_count,seances_count,
           mltx

          );
     for(tmpi=0; tmpi<mat_count; tmpi++)
    {
        FMatProcessMap[tmpi].iprocess=0;
        FMatProcessMap[tmpi].tx=mltx;
        FMatProcessMap[tmpi].seanceCount=emploi.getSeancesCount(tmpi,emMat);
        FMatProcessMap[tmpi].level=(int)(FMatProcessMap[tmpi].tx*FMatProcessMap[tmpi].seanceCount / 100);
        #if ENGINE_DEBUGMODE_LEVEL >=0
        printf("mindex=%d   count=%d    level=%d\n",
               tmpi,
               FMatProcessMap[tmpi].seanceCount,
               FMatProcessMap[tmpi].level);
        #endif
    }
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
        //reset delat
        emploi.SeanceTable()[j].delta=8-emploi.SeanceTable()[j].duree;//(1+emploi.SeanceTable()[j].byquinz);
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

uint16_t _fletcher16( uint8_t* data, int count ){

   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   int index;
   for( index = 0; index < count; ++index )
   {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
   }

   return (sum2 << 8) | sum1;
 }
const int MOD_ADLER = 65521;
/* data is the location of the data in physical memory and
len =length of data in bytes */
uint64_t _adler32(unsigned char *data, int32_t len)
{
    uint64_t a = 1, b = 0;
    int32_t i;

    /* Process each byte of the data in order */
    for (i = 0; i < len; ++i)
    {
        a = (a + data[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}

unsigned long emEngine::getProcessArrayChecksum(){

        unsigned long sum = _adler32((unsigned char*)FProcessArray,FProcessArrayLength);
        //sum =sum^0xFFFFFFFF;
       // printf("\n checksum=%ld\n\n",sum);
        return sum;

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

void emEngine::swapValues(const int v,const int w)
{
    int tmp=FProcessArray[w] ;
    FProcessArray[w]=FProcessArray[v];
    FProcessArray[v]=tmp;

}
int emEngine::doPartition(const int p,const int r){

    //random pivot or not!!
    //int x=FProcessArray[rand() % FProcessArrayLength];
    int x=FProcessArray[r];

    tmpi=p-1;

    for(tmpj=p;tmpj<r;tmpj++){
        if(emploi.SeanceTable()[FProcessArray[tmpj]].delta<=emploi.SeanceTable()[x].delta){
        tmpi++;
        swapValues(tmpi,tmpj);
        }
    }
    swapValues(tmpi+1,r);
    return tmpi+1;

}

void emEngine::doQuickSort(const bool ascendant,const int p,const int r){

 if(p<r){
    tmpk=doPartition(p,r);//pivot
    doQuickSort(ascendant,p,tmpk-1);
    doQuickSort(ascendant,tmpk+1,r);
 }

}
void emEngine::doShellSort(const bool ascendant)
{
 /*
 int n, i, j;
 n=0;
 int Le=FProcessArrayLength;
 while(n<Le)
     {
     n=3*n+1;
     }

 while(n!=0)
     {
     n=n/3;
     for (i=n;i<Le;i++)
         {

         j=i;

         while((j>(n-1)) &&
               (emploi.SeanceTable()[FProcessArray[j-n]].duree<emploi.SeanceTable()[FProcessArray[i]].duree))
             {
             FProcessArray[j]=FProcessArray[j-n];
             j=j-n;
             }
         FProcessArray[j]=FProcessArray[i];
         }
     }
*/
}

void emEngine::doSort(const bool ascendant){

  doQuickSort(true,0,FProcessArrayLength-1);

  #if ENGINE_DEBUGMODE_LEVEL >=0
   int i;
   for (i=0;i<FProcessArrayLength;i++)
     {
       printf("%d   [%d]     rang=%d     duree=%d   byquinz=%d  delta=%d\n",
              i,
              FProcessArray[i],
              emploi.SeanceTable()[FProcessArray[i]].rang,
              emploi.SeanceTable()[FProcessArray[i]].duree,
              emploi.SeanceTable()[FProcessArray[i]].byquinz,
              emploi.SeanceTable()[FProcessArray[i]].delta
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
void emEngine::Initialize(const bool AReset)
{
   if (AReset)
    {
        process_percent=0;
        doFill();
        doCheck();
         //  if (goRandomizeSeances in emploi.GlobalOptions)
        doRandomize();
        doSort(true);
        //FProcessArrayChecksum=0;
        FProcessArrayChecksum=getProcessArrayChecksum();

    }
       if (
          (   (process_speed>=1000) && (process_tries_count > (40000-(int)process_speed*10) )  )
          || ((process_tries_count-process_last_progession)>(1000+(int)process_percent*500))
          ){

        Console::WriteEx("\n        bad search path reprocessing..",ColorRed);
        Sleep(1000);
       /* if(true || (process_percent<99.0)){
        doRandomize();

        }*/
        //doRandomize();
        doSort(true);

        process_tries_count=0;
        process_last_progession=process_tries_count;
        max_processed_seances=0;
        time(&ptime);
        FProcessArrayChecksum=getProcessArrayChecksum();
        }



    for (tmpi=0 ; tmpi<classes_count; tmpi++)
        for (tmpj=0 ; tmpj<mat_count; tmpj++)
            for (tmpk=0 ; tmpk<6; tmpk++)
                FMatMapArray[tmpi][tmpj][tmpk]=0;

    if (FProcessArrayLength<seances_count)
    {
        for (tmpi=0 ; tmpi<FProcessArrayLength; tmpi++)
            emploi.viderSeance2(FProcessArray[tmpi],true);
    }
    else
    {
        //doit for all!

      /*  for (tmpk=0 ; tmpk<=10; tmpk++)
            for (tmpj=0; tmpj<=15; tmpj++)
            {

                for (tmpi=0; tmpi<classes_count; tmpi++)
                {
                    emploi.ClasseTable()[tmpi].weeka[tmpk][tmpj]=-1;
                    emploi.ClasseTable()[tmpi].weekb[tmpk][tmpj]=-1;
                }

                for (tmpi=0; tmpi<prof_count; tmpi++)
                {
                    emploi.ProfTable()[tmpi].weeka[tmpk][tmpj]=-1;
                    emploi.ProfTable()[tmpi].weekb[tmpk][tmpj]=-1;
                }

                for (tmpi=0; tmpi<salles_count; tmpi++)
                {
                    emploi.SalleTable()[tmpi].weeka[tmpk][tmpj]=-1;
                    emploi.SalleTable()[tmpi].weekb[tmpk][tmpj]=-1;
                }
            }
*/

      emploi.clearAllDT();
      for (tmpi=0; tmpi<seances_count; tmpi++)
        {
            emploi.SeanceTable()[tmpi].groupedwith=-1;

            emploi.SeanceTable()[tmpi].day=-1;
            emploi.SeanceTable()[tmpi].hour=-1;
        }

 }
    for(tmpi=0; tmpi<mat_count; tmpi++)
    {
        FMatProcessMap[tmpi].iprocess=0;

    }
    curcindex=-1;
    curmindex=-1;
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

                //if (process_tries_count % 10=0){
                //write empty line
                Console::SetCursorPosition(3,Console::GetCursorY());
                Console::Write("                                                                ");
                Console::SetCursorPosition(3,Console::GetCursorY());
                diff_time=difftime(time(0),ptime);
                progression_tx=process_percent;
                process_percent=(double)max_processed_seances/(double)seances_count*100;
                progression_tx=(process_percent-progression_tx)/diff_time*100;
                if(progression_tx>0)
                   process_last_progession = process_tries_count;

                time_t2SystemTime(time(0)-ptime,&st0);
                time_t2SystemTime(time(0)-stime,&st1);

                process_speed=(double)(process_tries_count/diff_time);
               // printf("\n",FProcessArrayChecksum);
                printf("%06d  %c %02.3f%% %04d/%04d %03.0f%/s %04d crc%08ld",
                       process_tries_count+1,
                       SPEED_SYM[process_tries_count % 4],

                       process_percent,
                       max_processed_seances,
                       seances_count,
                       process_speed,
                       //progression_tx,
                       process_tries_count-process_last_progession,
                       FProcessArrayChecksum

                      );
                //Console::Write(s);
                //printf("",difftime(time(0),ptime),difftime(time(0),stime));
                printf(" %02d:%02d:%02d/%02d:%02d:%02d",
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
    curSeance=&emploi.SeanceTable()[cursindex];
    curduree=curSeance->duree;

    curmindex=curSeance->mindex;
    curcindex=curSeance->cindex;
    curpindex=curSeance->pindex;
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
    PrDay=PrStartDay-1;
    NextDay();
    PrSalle=0;
    while (PrSalle<salles_count)
    {
       if (
            (!isForbiddenday)
            &&(curSeance->type_salle==emploi.SalleTable()[PrSalle].stype)
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
            emploi.setLink(cursindex,tmpgw,ltGroup);
            //update FMatMapArray
            FMatMapArray[curcindex][emploi.SeanceTable()[tmpgw].mindex][PrDay]=1;
        }
        curSeance->saindex=PrSalle;   //fill with salle ind
        curSeance->hour=PrHour;
        curSeance->day=PrDay;

        for (tmpi= PrHour; tmpi<PrEndhour; tmpi++)
        {
            if ((tmpfo==foMixte)||(tmpfo==foSemaineA))
            {
                if (tmpgw==-1)
                    emploi.ClasseTable()[curcindex].weeka[PrDay][tmpi]=cursindex;
                emploi.ProfTable()[curpindex].weeka[PrDay][tmpi]=cursindex;
                emploi.SalleTable()[PrSalle].weeka[PrDay][tmpi]=cursindex;
            }
            if ((tmpfo==foMixte)||(tmpfo==foSemaineB))
            {
                if (tmpgw==-1)
                    emploi.ClasseTable()[curcindex].weekb[PrDay][tmpi]=cursindex;
                emploi.ProfTable()[curpindex].weekb[PrDay][tmpi]=cursindex;
                emploi.SalleTable()[PrSalle].weekb[PrDay][tmpi]=cursindex;
            }
            //update FMatMapArray
            FMatMapArray[curcindex][curmindex][PrDay]=1;

        }
        FMatProcessMap[curmindex].iprocess++;
        //dec delta
        //curSeance->delta--;
        bresult=true;
    }
    else
    {

        for (tmpi= PrHour; tmpi<PrEndhour; tmpi++)
        {

            if ((tmpfo==foMixte)||(tmpfo==foSemaineA))
            {
                emploi.SalleTable()[PrSalle].weeka[PrDay][tmpi]=-1;
                emploi.ClasseTable()[curcindex].weeka[PrDay][tmpi]=-1;
                emploi.ProfTable()[curpindex].weeka[PrDay][tmpi]=-1;
            }
            if ((tmpfo==foMixte)||(tmpfo==foSemaineB))
            {
                emploi.SalleTable()[PrSalle].weekb[PrDay][tmpi]=-1;
                emploi.ClasseTable()[curcindex].weekb[PrDay][tmpi]=-1;
                emploi.ProfTable()[curpindex].weekb[PrDay][tmpi]=-1;
            }

        }
        if (tmpgw!=-1)
        {
            emploi.setLink(cursindex,curSeance->groupedwith,ltClear);
            //update FMatMapArray
            FMatMapArray[curcindex][emploi.SeanceTable()[tmpgw].mindex][PrDay]=0;
        }
        curSeance->saindex=-1;
        curSeance->hour=-1;
        curSeance->day=-1;
        //update FMatMapArray
        FMatMapArray[curcindex][curmindex][PrDay]=0;
    bresult=true;
    }


    if (!bresult)
        Console::WriteEx("RepmliSalle failed!",ColorRed);
   #if ENGINE_DEBUGMODE_LEVEL >=2
     else  printf("    ->[%d] RepmliSalle n°:%d ....[ok]\n",cursindex,PrSalle);
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
inline int emEngine::checkEmptyDT()
{

   // sse=(*SalleDT)[PrDay][PrHour] ;// salle seance index
   // pse=(*ProfDT)[PrDay][PrHour] ; //prof  seance
    class_se=(*ClasseDT)[PrDay][PrHour] ;//class  seance
    bcangroup=(class_se!=-1)&&(emploi.CanBeByGroup(cursindex,class_se,PrDay,PrHour));

  if (
       ((*SalleDT)[PrDay][PrHour]!=-1)||
       ((*ProfDT)[PrDay][PrHour]!=-1)||
       //! Class is busy for cur index
       ((class_se!=-1) && ! bcangroup )
       //no sport after 17h
      /* ||
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
      */
   )
    return -1;
       //check can fill seance in salle
        for (tmpi=PrHour ; tmpi<PrEndhour; tmpi++)
            if (
                (((*SalleDT)[PrDay][tmpi]!=-1))||
                (((*ProfDT)[PrDay][tmpi]!=-1))||
                (((*ClasseDT)[PrDay][tmpi]!=-1) && ! bcangroup)
            )
            return -1;
            return PrHour;
      /*          break;
            else if (tmpi==curduree-1)
                iresult=PrHour;
        return iresult;*/
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
    PrEndhour=PrHour+curduree;
    isForbiddenday=
            (PrDay==curfday)
            || (FMatMapArray[curcindex][curmindex][PrDay]==1)//no 2 mat in same day
            || ((PrHour<8)&&( PrEndhour>8))//matinee
            ||( PrEndhour>16) //soir
            ||( ((curduree==4)||(curduree==3)) &&(PrHour % 4!=0)    )           //pour les seances de 2h ou 1.5h
            ||( (curduree==8) &&(PrHour % 8!=0) )                            //.... de 4h
            ||( (PrHour % 2!=0) && (curduree!=3)    )                          // begin with half hour
            ||( (PrDay>3)&& (PrHour>=8) )                                   //no course for ven & sam soir
            //math-physique
           ||(  (curmindex==0) && (FMatProcessMap[0].iprocess<FMatProcessMap[0].level) && (PrEndhour>8)     )
           || ( ((curmindex==0)||(curmindex==13)) && (PrEndhour>12)     )
            //arabe-francais-anglais
           ||(  (curmindex==3) && (FMatProcessMap[3].iprocess<FMatProcessMap[3].level) && (PrEndhour>8) && (PrDay==0)    )
           ||(  (curmindex==4) && (FMatProcessMap[4].iprocess<FMatProcessMap[4].level) && (PrEndhour>4) && (PrDay>=4)    )
           ||(  (curmindex==5) && (FMatProcessMap[5].iprocess<FMatProcessMap[5].level) && (PrEndhour>8) && (PrDay==3)    )

           //education nat/religion ap 10h
          // || ( (iprocess< lowlevel_process_count)  && (((curmindex >=8) && (curmindex<=10)) && (PrHour<4))   )

           //histoire-geo apres 10h
           || ( (curmindex==7) && (FMatProcessMap[7].iprocess<FMatProcessMap[7].level) && (PrEndhour<6)     )

           // heure de  pointe
           || ( (iprocess<lowlevel_process_count)&& (curduree==2) &&(PrEndhour>14)  )

           //sport
           || ( (curmindex==6) && (PrEndhour>14)    )
           //test
          // || ( (iprocess<highlevel_process_count) &&  (curpindex==33) && (PrDay<=2)&& (PrEndhour>8)        )
            ;

}
