
#include <stdio.h>
#include "../3rdparty/Console/console.h"
#include "solution.h"




CemSolution::CemSolution()
{
    //printf("emSolution created!\n");
}

CemSolution::~CemSolution()
{
    //printf("emSolution destroyed!\n");
}
bool CemSolution::saveHeader(CFileStream* stream)
{
    int sz;
    CemConfig* pc = emConfig;

    try
    {
        sz=sizeof(CemConfig);
        stream->write(&sz, sizeof(sz));
        stream->write(pc, sz);

    }
    catch(...)
    {
        return false;
    }
 
    return true;

}

CemConfig* CemSolution::loadHeader(CFileStream* stream)
{
    int sz;
    CemConfig pc;
    try
    {
        stream->read(&sz, sizeof(sz));
      
        stream->read(&pc, sz);
    }
    catch(...)
    {
        return NULL;
    }
    return &pc;

}

bool CemSolution::writeData(CFileStream* stream,WORD phase)
{
    int sz,Le,i;
    CCroom* sa;
    CClasse* cl;
    CShift* se;
    CProf* pr;
    CMat* ma;
    char s[2]="" ;
    //read string to compare to magic
    if(phase==0)
        stream->writeString(emConfig->magic);


    switch (phase)
    {
    case 0:
        //write count
        Le=FCroomTableCount;
        stream->write(&Le, sizeof(Le));
        //write struct
        sz=sizeof(CCroom);
        stream->write(&sz ,sizeof(sz));
       
        for(i=0; i<Le; i++)
        {

            stream->write(&FCroomTable[i],sz);

            //write magic separateur
            stream->write(s,2);
        }
       
        break;
    case 1:
        //write count
        Le=FClasseTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CClasse);
        stream->write(&sz ,sizeof(sz));
        for(i=0; i<Le; i++)
        {

            stream->write(&FClasseTable[i],sz);

            //write magic separateur
            stream->write(s,2);
        }
    
        break;
    case 2:
        //write count
        Le=FShiftTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CShift);
        stream->write(&sz ,sizeof(sz));
      
        for(i=0; i<Le; i++)
        {

            stream->write(&FShiftTable[i],sz);
            //write magic separateur
            stream->write(s,2);
        }

        break;
    case 3:
        //write count
        Le=FProfTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CProf);
        stream->write(&sz ,sizeof(sz));
        for(i=0; i<Le; i++)
        {

            stream->write(&FProfTable[i],sz);

            //write magic separateur
            stream->write(s,2);
        }

        break;
    case 4:
        //write count
        Le=FMatTableCount;
        stream->write(&Le, sizeof(Le));
        //write struct
        sz=sizeof(CMat);
        stream->write(&sz ,sizeof(sz));
        for(i=0; i<Le; i++)
        {

            stream->write(&FMatTable[i],sz);

            //write magic separateur
            stream->write(s,2);
        }

        break;

    }
    return true;
}

int CemSolution::readData(CFileStream* stream,WORD phase)
{
    int sz,Le,i;
    CCroom* sa;
    CClasse* cl;
    CShift* se;
    CProf* pr;
    CMat* ma;
    //read string then compare to magic
    if(phase==0)
        printf("magic:%s\n",stream->readString());
    char s[2]="" ;
    //read count
    stream->read(&Le, sizeof(Le));

    switch (phase)
    {
    case 0:
        FCroomTable=new CCroom[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        sa=(CCroom*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(sa,sz);
            FCroomTable[i]=(CCroom)(*sa);

            //read magic separateur
            stream->read(s,2);
        }
        delete sa;

        break;
    case 1:
        FClasseTable=new CClasse[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        cl=(CClasse*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(cl,sz);
            FClasseTable[i]=(CClasse)(*cl);
            //printf("Classe n� %d [%s]\n",FClasseTable[i].rank,FClasseTable[i].name);

            //read magic separateur
            stream->read(s,2);
        }
        delete cl;

        break;

    case 2:
        FShiftTable=new CShift[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        se=(CShift*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(se,sz);
            FShiftTable[i]=(CShift)(*se);


            //read magic separateur
            stream->read(s,2);
        }
        delete se;

        break;

    case 3:
        FProfTable=new CProf[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        pr=(CProf*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(pr,sz);
            FProfTable[i]=(CProf)(*pr);
            //printf("prof n� %d [%s]\n",FProfTable[i].rank,FProfTable[i].name);

            //read magic separateur
            stream->read(s,2);
        }
        delete pr;

        break;

    case 4:
        FMatTable=new CMat[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        ma=(CMat*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(ma,sz);
            FMatTable[i]=(CMat)(*ma);
            //printf("croom n� %s [%s]\n",FMatTable[i].name,FMatTable[i].symbol);

            //read magic separateur
            stream->read(s,2);
        }
        delete ma;

        break;
    }

    return Le;

}

bool   CemSolution::LoadFromFile(const char* filename)
{
    CFileStream fs(filename,cmOpen);
    emConfig=loadHeader(&fs);
    if(emConfig==NULL)
    {
        printf("error reading header!");
        return false;
    }

#if DEBUG_MODE
     printf("magic:[%s]\n",emConfig->magic);
     printf("version_lbl:[%s]\n",emConfig->version_lbl);
     printf("lbl_1:[%s]\n",emConfig->lbl_1);
     printf("lbl_2:[%s]\n",emConfig->lbl_2);
     printf("lbl_3:[%s]\n",emConfig->lbl_3);
     printf("lbl_4:[%s]\n",emConfig->lbl_4);
     printf("lbl_annee:[%s]\n",emConfig->lbl_annee);
#endif

    FCroomTableCount=readData(&fs,0);
    FClasseTableCount=readData(&fs,1);
    FShiftTableCount=readData(&fs,2);
    FProfTableCount=readData(&fs,3);
    FMatTableCount=readData(&fs,4);

    printf("Class-room count:   %d\n",FCroomTableCount);
    printf("Class count:        %d\n",FClasseTableCount);
    printf("Shift count:        %d\n",FShiftTableCount);
    printf("Professor count:    %d\n",FProfTableCount);
    printf("Mat count:          %d\n",FMatTableCount);


    return true;
}

bool   CemSolution::SaveToFile(const char* filename)
{

    CFileStream fs(filename,cmCreate);

    if(saveHeader(&fs)!=true)
    {
        printf("error writing header!");
        return false;
    }

#if DEBUG_MODE
    printf("magic:[%s]\n",emConfig->magic);
     printf("version_lbl:[%s]\n",emConfig->version_lbl);
     printf("lbl_1:[%s]\n",emConfig->lbl_1);
     printf("lbl_2:[%s]\n",emConfig->lbl_2);
     printf("lbl_3:[%s]\n",emConfig->lbl_3);
     printf("lbl_4:[%s]\n",emConfig->lbl_4);
     printf("lbl_annee:[%s]\n",emConfig->lbl_annee);
#endif
    writeData(&fs,0);
    writeData(&fs,1);
    writeData(&fs,2);
    writeData(&fs,3);
    writeData(&fs,4);

#if DEBUG_MODE   
    printf("nombre de croom:%d\n",FCroomTableCount);
      printf("nombre de classe:%d\n",FClasseTableCount);
      printf("nombre de shift:%d\n",FShiftTableCount);
      printf("nombre de prof:%d\n",FProfTableCount);
      printf("nombre de mat:%d\n",FMatTableCount);
    
#endif

    return true;

}

int CemSolution::getOrphinedIdxByDay(const int AIndex,const int ADay,const CEmploiMode emMode)
{
    int h, tmpA,tmpB;
    CDayTable  *wa,*wb;
    int result=0;
    switch (emMode)
    {
    case emClasse:
        wa=&(FClasseTable[AIndex].weeka);
        wb=&(FClasseTable[AIndex].weekb) ;
        break;

    case emProf:
        wa=&(FProfTable[AIndex].weeka);
        wb=&(FProfTable[AIndex].weekb) ;
        break;
    default:
        return -1;

    }

    tmpA=0;
    tmpB=0;
    //check 1
    for (h=0 ; h<=7; h++)
    {
        tmpA= tmpA+((*wa)[ADay][h]!=-1) ;
        tmpB= tmpB+((*wb)[ADay][h]!=-1) ;
    }

    result=result + (((tmpA<4)&&(tmpA>0))||((tmpB<4)&&(tmpB>0)));

    tmpA=0;
    tmpB=0;
    for (h=8 ; h<=15; h++)
    {
        tmpA= tmpA+((*wa)[ADay][h]!=-1) ;
        tmpB= tmpB+((*wb)[ADay][h]!=-1) ;
    }
    result=result + (((tmpA<4)&&(tmpA>0))||((tmpB<4)&&(tmpB>0)));
    return result;
}

int CemSolution::getOrphinedIdx(const int AIndex,const CEmploiMode emMode)
{
    int d;
    int result=0;
    for(d=0; d<=5; d++)
        result=result+getOrphinedIdxByDay(AIndex,d,emMode);
    return result;
}
int CemSolution::getIdxByDaytable(CDayTable *Adt)
{
    int Se=-1;
    int ADay=0;
    int result=0;
    int h,du;
    bool check1,check2;
    while (ADay<=5)
    {
        h=0;
        while (h<15)
        {
            Se=(*Adt)[ADay][h];
            if (Se!=-1)
            {
                du=FShiftTable[Se].length;
                check1=(
                           ((h+du<6)||((h>=8)&&(h+du<14)))       &&
                           ((*Adt)[ADay][h+du]==-1)              &&
                           ((*Adt)[ADay][h+du+1]==-1)            &&
                           ((*Adt)[ADay][h+du+2]!=-1)
                       );
                check2=(
                           ((h+du<4)||((h>=8)&&(h+du<12)))        &&
                           ((*Adt)[ADay][h+du]==-1)              &&
                           ((*Adt)[ADay][h+du+1]==-1)            &&
                           ((*Adt)[ADay][h+du+2]==-1)            &&
                           ((*Adt)[ADay][h+du+3]==-1)            &&
                           ((*Adt)[ADay][h+du+4]!=-1)
                       ) ;
                result=result+check1+check2;
                h=h+du-1;
            }
            h++;
        }
        ADay++;
    }
    return result;
}

int CemSolution::getCompactIdx(const int AIndex,const CEmploiMode emMode)
{

    int result=0;
    switch(emMode)
    {
    case emClasse:
        result=getIdxByDaytable(&FClasseTable[AIndex].weeka);
        break;
    case emProf:
        result=getIdxByDaytable(&FProfTable[AIndex].weeka);
        break;

    }
    return result;
}
int CemSolution::getShiftsCount(const int AIndex,const CEmploiMode emMode)
{

    int result=0;
    int i;
    if (AIndex<0)
        return 0;

    switch(emMode)
    {
    case emClasse:
        for (i=0 ; i<FShiftTableCount; i++)
            if(FShiftTable[i].cindex==AIndex)
                result++;
        break;
    case emProf:
        for (i=0 ; i<FShiftTableCount; i++)
            if(FShiftTable[i].pindex==AIndex)
                result++;
        break;
    case emMat:
        for (i=0 ; i<FShiftTableCount; i++)
            if(FShiftTable[i].mindex==AIndex)
                result++;
        break;
    case emCroom:
        for (i=0 ; i<FShiftTableCount; i++)
            if(FShiftTable[i].saindex==AIndex)
                result++;
        break;
    }
    return result;

}

bool    CemSolution::getOptimizeValue(int *hp,int*hc,int*cp,int*cc)
{

    int e;
    *hp=0;
    *hc=0;
    *cp=0;
    *cc=0;

    for(e=0 ; e< FClasseTableCount; e++)
    {
        *hc=*hc+getOrphinedIdx(e,emClasse);
        *cc=*cc+getCompactIdx(e,emClasse);
    }

    for(e=0 ; e< FProfTableCount; e++)
    {
        *hp=*hp+getOrphinedIdx(e,emProf);
        *cp=*cp+getCompactIdx(e,emProf);
    }

    return true;

}

bool CemSolution::parkShift(const int Sindex,bool ABool)
{

    return true;
}

bool  CemSolution::checkDT(const int Sindex,CDayTable* dt)
{
    int day,h;
    bool result,checked;
    CShift* se;
    result=false;
    checked=false;
    se=&(FShiftTable[Sindex]);
    day=se->day ;
    h=se->hour  ;
    if((day<0)||(day>10)||(h<0)||(h>15))
    {
        parkShift(Sindex,true);
        return false;
    }

    if ((*dt)[day][h]!=Sindex)
        return false;
    for (day=0; day<=5; day++)
        for (h=0; h<=15; h++)
            if ((*dt)[day][h]==Sindex)
            {
                result=((se->day==day)
                        &&(h>=se->hour)
                        &&(h<=se->hour+se->length-1));
                checked=(h>=(se->hour+se->length-1));
                if (!result || checked)
                    return result;
            }
    return true;
}

bool  CemSolution::CheckAllDT(const int Sindex)
{
    CClasse* Cl;
    CCroom *Sa;
    CProf *Pr;
    CShift *Se;
    bool  checkA,checkB,result,result1,result2;
    char s[254]="";

    //just for testing: emulate an erronous Shift:
    //FShiftTable[0].day=1;
    //  FCroomTable[0].weekb[0,0]=111;
    result=false;

    Se=&(FShiftTable[Sindex]);
    Cl=&(FClasseTable[Se->cindex]);
    Sa=&(FCroomTable[Se->saindex]);
    Pr=&(FProfTable[Se->pindex]);

    //check classe table

    checkA=checkDT(Sindex,&(Cl->weeka))  ;
    checkB=checkDT(Sindex,&(Cl->weekb))  ;

    result=((Se->bygroup
             &&

             (
                 ( (    (Se->groupedwith!=-1)
                        && (FShiftTable[Se->groupedwith].groupedwith==Sindex)
                        && (FShiftTable[Se->groupedwith].day==Se->day)
                        && (FShiftTable[Se->groupedwith].hour==Se->hour)
                        && (FShiftTable[Se->groupedwith].length==Se->length)


                   )
                    && (Se->dogroupwith==-1)

                 )
                 ||
                 (checkA && checkB)

             )

            )
            ||
            (
                (Se->every2weeks && checkA)
                ||
                (Se->every2weeks && checkB)
                ||
                (checkA && checkB)
            ) );



    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        sprintf_s(s,"\t->[%d]  .. [ok]",Sindex);
    else
        sprintf_s(s,"\n\t->[%d]  checking stage1 .. [failure]\n",Sindex);
    Console::Write(s);

    result1=result;
    //check croom table
    checkA=checkDT(Sindex,&(Sa->weeka))  ;
    checkB=checkDT(Sindex,&(Sa->weekb))  ;

    result=(Se->every2weeks && checkA)
           ||
           (Se->every2weeks && checkB)
           ||
           (checkA && checkB)  ;


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        sprintf_s(s,"\t->[%d]  .. [ok]",Sindex);
    else
        sprintf_s(s,"\n\t->[%d]  checking stage2 .. [failure]\n",Sindex);
    Console::Write(s);

    result2=result;

    //check prof table
    checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result=(Se->every2weeks && checkA)
           ||
           (Se->every2weeks && checkB)
           ||
           (checkA && checkB);


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        sprintf_s(s,"\t->[%d]  .. [ok]",Sindex);
    else
        sprintf_s(s,"\n\t->[%d]  checking stage3 .. [failure]\n",Sindex);
    Console::Write(s);

    //check grouping table
    /*checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result=(Se->every2weeks && checkA)
           ||
           (Se->every2weeks && checkB)
           ||
           (checkA && checkB);


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        sprintf_s(s,"   ->[%d]  .. [ok]",Sindex);
    else
        sprintf_s(s,"\n   ->[%d]  checking stage3 .. [failure]\n",Sindex);
    Console::Write(s);
    */
    return (result1 && result2 && result);

}

bool   CemSolution::verifyProcessedShifts(bool b)
{
    bool result=true;
//int pass=0;
    int Sindex=0;

    while (Sindex<FShiftTableCount)
    {

        if  (!CheckAllDT(Sindex))
            result=false;
        Sindex++;
    }//while

    char s[254]="";
    Console::Write("\n");
    if (result)
        Console::Write(  "\t->verifying processed Shifts -> [ok]\n");
    else
        Console::WriteEx("\t->verifying processed Shifts -> [failure]\n",ColorRed);

    return result;
}



int    CemSolution::ShiftTableCount(void)
{
    return FShiftTableCount;

}

bool CemSolution::setLink(const int s1,const int s2,ClinkType Alink)
{

    if((s1==-1)||(s2==-1))
        return false;
    //   gc_RaiseError('Error: setLink with invalid parameters!');

    switch(Alink)
    {
    case ltGroup: //set Grouped link Shift index

        FShiftTable[s1].groupedwith=s2;
        FShiftTable[s2].groupedwith=s1;
        FShiftTable[s1].dogroupwith=s2;
        FShiftTable[s2].dogroupwith=-1;
        FModified=true;
        break;

    case ltClear:

        FShiftTable[s1].groupedwith=-1;
        FShiftTable[s2].groupedwith=-1;
        FShiftTable[s1].dogroupwith=-1;
        FShiftTable[s2].dogroupwith=-1;
        FModified=true;
        break;

    }
    return true;
}

bool CemSolution::FillCroom(const int AShift,const int ACroom,const int ADay,const int AHour,
                             const CFillOption fo,const int gw,bool abool)
{

    int i=0;
    bool result=false;
    int du=FShiftTable[AShift].length;
    int classei=FShiftTable[AShift].cindex;
    int profi=FShiftTable[AShift].pindex;

    if (abool)
    {

        if (gw!=-1)
        {
            setLink(gw,AShift,ltGroup);

        }
        FShiftTable[AShift].saindex=ACroom;   //fill with croom ind
        FShiftTable[AShift].hour=AHour;
        FShiftTable[AShift].day=ADay;
        FShiftTable[AShift].doAlternatewith=(int)fo;

        for (i= 0; i<du; i++)
        {
            if ((fo==foMixte)||(fo==foWeekA))
            {
                if (gw==-1)
                    FClasseTable[classei].weeka[ADay][AHour+i]=AShift;
                FProfTable[profi].weeka[ADay][AHour+i]=AShift;
                FCroomTable[ACroom].weeka[ADay][AHour+i]=AShift;
            }
            if ((fo==foMixte)||(fo==foWeekB))
            {
                if (gw==-1)
                    FClasseTable[classei].weekb[ADay][AHour+i]=AShift;
                FProfTable[profi].weekb[ADay][AHour+i]=AShift;
                FCroomTable[ACroom].weekb[ADay][AHour+i]=AShift;
            }


        }
    }
    else
    {

        for (i= 0; i<du; i++)
        {

            if ((fo==foMixte)||(fo==foWeekA))
            {
                FCroomTable[ACroom].weeka[ADay][AHour+i]=-1;
                FClasseTable[classei].weeka[ADay][AHour+i]=-1;
                FProfTable[profi].weeka[ADay][AHour+i]=-1;
            }
            if ((fo==foMixte)||(fo==foWeekB))
            {
                FCroomTable[ACroom].weekb[ADay][AHour+i]=-1;
                FClasseTable[classei].weekb[ADay][AHour+i]=-1;
                FProfTable[profi].weekb[ADay][AHour+i]=-1;
            }

        }
        if (FShiftTable[AShift].groupedwith!=-1)
        {
            setLink(FShiftTable[AShift].groupedwith,AShift,ltClear);

        }
        FShiftTable[AShift].saindex=-1;
        FShiftTable[AShift].hour=-1;
        FShiftTable[AShift].day=-1;
        FShiftTable[AShift].doAlternatewith=(int)foNoWhere;
    }
    result=true;

    /*if (!result)
        Console::WriteEx("RepmliCroom failed!",ColorRed);*/

    return result;

}
void CemSolution::clearAllDT()
{

    int d,h,i;
    for (d= 0; d<11; d++)
        for (h= 0; h<=15; h++)
        {
            for (i= 0; i<ClasseTableCount(); i++)
            {
                FClasseTable[i].weeka[d][h]=-1;
                FClasseTable[i].weekb[d][h]=-1;
            }
            for (i= 0; i<CroomTableCount(); i++)
            {
                FCroomTable[i].weeka[d][h]=-1;
                FCroomTable[i].weekb[d][h]=-1;
            }
            for (i= 0; i<ProfTableCount(); i++)
            {
                FProfTable[i].weeka[d][h]=-1;
                FProfTable[i].weekb[d][h]=-1;
            }

        }
}

void CemSolution::rebuildSolution()
{

    int e,gw;
    clearAllDT();
    for(e=0 ; e< FShiftTableCount; e++)
    {
        if((FShiftTable[e].groupedwith!=-1)&&(FShiftTable[e].dogroupwith==-1))
            gw=FShiftTable[e].groupedwith;
        else gw=-1;
        FillCroom(e,
                    FShiftTable[e].saindex,
                    FShiftTable[e].day,
                    FShiftTable[e].hour,
                    (CFillOption)(FShiftTable[e].doAlternatewith),
                    gw,
                    true
                   );

    }

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CFillOption CemSolution::getShiftFillMode(const int Se)
{
    int aday,ahour,croomi;

    CFillOption result=foMixte;

    if (FShiftTable[Se].every2weeks)
    {
        croomi=FShiftTable[Se].saindex;
        aday=FShiftTable[Se].day ;
        ahour=FShiftTable[Se].hour;
        if (FCroomTable[croomi].weeka[aday][ahour]==Se)
            result=foWeekA;
        else  if (FCroomTable[croomi].weekb[aday][ahour]==Se)
            result=foWeekB;
        else
            result=foNoWhere;
    }
    return result;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool CemSolution::CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour)
{
    return(
              (Se1!=-1)&&(Se2!=-1)&&(Ahour>=0)&&(Ahour<=15)                     &&
              (Se1!=Se2)                                                        &&
              FShiftTable[Se1].bygroup                                         &&
              FShiftTable[Se2].bygroup                                         &&
              (FShiftTable[Se2].dogroupwith==-1)                               &&
              (FShiftTable[Se1].groupedwith==-1)                               &&
              (FShiftTable[Se2].groupedwith==-1)                               &&
              (FShiftTable[Se1].every2weeks==FShiftTable[Se2].every2weeks)            &&
              (FShiftTable[Se1].length==FShiftTable[Se2].length)                &&
              (Aday==FShiftTable[Se2].day)                                     &&
              (Ahour==FShiftTable[Se2].hour)
          );

}
void CemSolution::shellSort()
{
    int n, i, j;
    CShift *pSe;

    n=0;
    int Le=FShiftTableCount;
    while(n<Le)
    {
        n=3*n+1;
    }

    while(n!=0)
    {
        n=n/3;
        for (i=n; i<Le; i++)
        {
            pSe=&FShiftTable[i];
            j=i;

            while((j>(n-1)) && (FShiftTable[j-n].length>pSe->length))
            {
                FShiftTable[j]=FShiftTable[j-n];
                j=j-n;
            }
            FShiftTable[j]=*pSe;
        }
    }
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
void CemSolution::SetTablesCount()
{

}
bool CemSolution::clearShift(const int se,bool abool)
{
    return true;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
CMat* CemSolution::MatTable()
{
    return FMatTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CProf* CemSolution::ProfTable()
{
    return FProfTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CShift* CemSolution::ShiftTable()
{
    return FShiftTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CCroom* CemSolution::CroomTable()
{
    return FCroomTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CClasse* CemSolution::ClasseTable()
{
    return FClasseTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CemSolution::MatTableCount(void)
{
    return FMatTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CemSolution::CroomTableCount(void)
{
    return FCroomTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CemSolution::ProfTableCount(void)
{
    return FProfTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CemSolution::ClasseTableCount(void)
{
    return FClasseTableCount;
}
int CemSolution::CroomCountByType(int stype){
int i=0;
int result=0;
for(i=0;i<FCroomTableCount;i++){
    if(FCroomTable[i].stype==stype)
            result++;

    }

  return result;

}
