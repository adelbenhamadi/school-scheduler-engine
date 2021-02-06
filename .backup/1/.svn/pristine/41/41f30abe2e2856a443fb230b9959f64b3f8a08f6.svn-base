
#include <stdio.h>
#include "console.h"
#include "solution.h"




emSolution::emSolution()
{
    //printf("emSolution created!\n");
}

emSolution::~emSolution()
{
    //printf("emSolution destroyed!\n");
}
bool emSolution::saveHeader(CFileStream* stream)
{
    int sz;
    CemConfig* pc;

    try
    {
        sz=sizeof(CemConfig);
        stream->write(&sz, sizeof(sz));
        pc=(CemConfig*)malloc(sz);
        pc=emConfig;
        stream->write(pc, sz);

    }
    catch(...)
    {
        if(pc!=NULL) delete pc;
        return false;
    }
    delete pc;
    return true;

}

CemConfig* emSolution::loadHeader(CFileStream* stream)
{
    int sz;
    CemConfig* pc;
    try
    {
        stream->read(&sz, sizeof(sz));
        pc=(CemConfig*) malloc(sz);
        stream->read(pc, sz);
    }
    catch(...)
    {
        return NULL;
    }
    return pc;

}

bool emSolution::writeData(CFileStream* stream,WORD phase)
{
    int sz,Le,i;
    CSalle* sa;
    CClasse* cl;
    CSeance* se;
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
        Le=FSalleTableCount;
        stream->write(&Le, sizeof(Le));
        //write struct
        sz=sizeof(CSalle);
        stream->write(&sz ,sizeof(sz));
        sa=(CSalle*)malloc(sz);
        for(i=0; i<Le; i++)
        {

            sa=&FSalleTable[i];
            stream->write(sa,sz);

            //write magic separateur
            stream->write(s,2);
        }
        delete sa;
        break;
    case 1:
        //write count
        Le=FClasseTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CClasse);
        stream->write(&sz ,sizeof(sz));
        cl=(CClasse*)malloc(sz);
        for(i=0; i<Le; i++)
        {

            cl=&FClasseTable[i];
            stream->write(cl,sz);

            //write magic separateur
            stream->write(s,2);
        }
        delete cl;
        break;
    case 2:
        //write count
        Le=FSeanceTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CSeance);
        stream->write(&sz ,sizeof(sz));
        se=(CSeance*)malloc(sz);
        for(i=0; i<Le; i++)
        {

            se=&FSeanceTable[i];
            stream->write(se,sz);

            //write magic separateur
            stream->write(s,2);
        }
        delete se;
        break;
    case 3:
        //write count
        Le=FProfTableCount;
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CProf);
        stream->write(&sz ,sizeof(sz));
        pr=(CProf*)malloc(sz);
        for(i=0; i<Le; i++)
        {

            pr=&FProfTable[i];
            stream->write(pr,sz);

            //write magic separateur
            stream->write(s,2);
        }
        delete pr;
        break;
    case 4:
        //write count
        Le=FMatTableCount;
        stream->write(&Le, sizeof(Le));
        //write struct
        sz=sizeof(CMat);
        stream->write(&sz ,sizeof(sz));
        ma=(CMat*)malloc(sz);
        for(i=0; i<Le; i++)
        {

            ma=&FMatTable[i];
            stream->write(ma,sz);

            //write magic separateur
            stream->write(s,2);
        }
        delete ma;
        break;

    }
    return true;
}

int emSolution::readData(CFileStream* stream,WORD phase)
{
    int sz,Le,i;
    CSalle* sa;
    CClasse* cl;
    CSeance* se;
    CProf* pr;
    CMat* ma;
    //read string to compare to magic
    if(phase==0)
        printf("magic:%s\n",stream->readString());
    char s[2]="" ;
    //read count
    stream->read(&Le, sizeof(Le));

    switch (phase)
    {
    case 0:
        FSalleTable=new CSalle[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        sa=(CSalle*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(sa,sz);
            FSalleTable[i]=(CSalle)(*sa);
            //printf("salle n� %d [%s]\n",FSalleTable[i].rang,FSalleTable[i].name);

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
            //printf("Classe n� %d [%s]\n",FClasseTable[i].rang,FClasseTable[i].name);

            //read magic separateur
            stream->read(s,2);
        }
        delete cl;

        break;

    case 2:
        FSeanceTable=new CSeance[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        se=(CSeance*)malloc(sz);
        for(i=0; i<Le; i++)
        {
            stream->read(se,sz);
            FSeanceTable[i]=(CSeance)(*se);


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
            //printf("prof n� %d [%s]\n",FProfTable[i].rang,FProfTable[i].name);

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
            //printf("salle n� %s [%s]\n",FMatTable[i].name,FMatTable[i].symbol);

            //read magic separateur
            stream->read(s,2);
        }
        delete ma;

        break;
    }




    return Le;

}

bool   emSolution::LoadFromFile(const char* fn)
{
    CFileStream fs=CFileStream(fn,cmOpen);
    emConfig=loadHeader(&fs);
    if(emConfig==NULL)
    {
        printf("error reading header!");
        return false;
    }

    /* printf("magic:[%s]\n",emConfig->magic);
     printf("version_lbl:[%s]\n",emConfig->version_lbl);
     printf("lbl_1:[%s]\n",emConfig->lbl_1);
     printf("lbl_2:[%s]\n",emConfig->lbl_2);
     printf("lbl_3:[%s]\n",emConfig->lbl_3);
     printf("lbl_4:[%s]\n",emConfig->lbl_4);
     printf("lbl_annee:[%s]\n",emConfig->lbl_annee);*/
    //



    FSalleTableCount=readData(&fs,0);
    FClasseTableCount=readData(&fs,1);
    FSeanceTableCount=readData(&fs,2);
    FProfTableCount=readData(&fs,3);
    FMatTableCount=readData(&fs,4);

    printf("nombre de salle:    %d\n",FSalleTableCount);
    printf("nombre de classe:   %d\n",FClasseTableCount);
    printf("nombre de seance:   %d\n",FSeanceTableCount);
    printf("nombre de prof:     %d\n",FProfTableCount);
    printf("nombre de mat:      %d\n",FMatTableCount);



    fs.~CFileStream();
    return true;
}

bool   emSolution::SaveToFile(const char* fn)
{

    CFileStream fs=CFileStream(fn,cmCreate);

    if(saveHeader(&fs)!=true)
    {
        printf("error writing header!");
        return false;
    }

    /* printf("magic:[%s]\n",emConfig->magic);
     printf("version_lbl:[%s]\n",emConfig->version_lbl);
     printf("lbl_1:[%s]\n",emConfig->lbl_1);
     printf("lbl_2:[%s]\n",emConfig->lbl_2);
     printf("lbl_3:[%s]\n",emConfig->lbl_3);
     printf("lbl_4:[%s]\n",emConfig->lbl_4);
     printf("lbl_annee:[%s]\n",emConfig->lbl_annee);*/
    //



    writeData(&fs,0);
    writeData(&fs,1);
    writeData(&fs,2);
    writeData(&fs,3);
    writeData(&fs,4);

    /*  printf("nombre de salle:%d\n",FSalleTableCount);
      printf("nombre de classe:%d\n",FClasseTableCount);
      printf("nombre de seance:%d\n",FSeanceTableCount);
      printf("nombre de prof:%d\n",FProfTableCount);
      printf("nombre de mat:%d\n",FMatTableCount);
         */

    fs.~CFileStream();
    return true;

}

int emSolution::getOrphinedIdxByDay(const int AIndex,const int ADay,const CEmploiMode emMode)
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

int emSolution::getOrphinedIdx(const int AIndex,const CEmploiMode emMode)
{
    int d;
    int result=0;
    for(d=0; d<=5; d++)
        result=result+getOrphinedIdxByDay(AIndex,d,emMode);
    return result;
}
int emSolution::getIdxByDaytable(CDayTable *Adt)
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
                du=FSeanceTable[Se].duree;
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

int emSolution::getCompactIdx(const int AIndex,const CEmploiMode emMode)
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
int emSolution::getSeancesCount(const int AIndex,const CEmploiMode emMode)
{

    int result=0;
    int i;
    if (AIndex<0)
        return 0;

    switch(emMode)
    {
    case emClasse:
        for (i=0 ; i<FSeanceTableCount; i++)
            if(FSeanceTable[i].cindex==AIndex)
                result++;
        break;
    case emProf:
        for (i=0 ; i<FSeanceTableCount; i++)
            if(FSeanceTable[i].pindex==AIndex)
                result++;
        break;
    case emMat:
        for (i=0 ; i<FSeanceTableCount; i++)
            if(FSeanceTable[i].mindex==AIndex)
                result++;
        break;
    case emSalle:
        for (i=0 ; i<FSeanceTableCount; i++)
            if(FSeanceTable[i].saindex==AIndex)
                result++;
        break;
    }
    return result;

}

bool    emSolution::getOptimizeValue(int *hp,int*hc,int*cp,int*cc)
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

bool emSolution::parkSeance(const int Sindex,bool ABool)
{
    CSeance* se;
}

bool  emSolution::checkDT(const int Sindex,CDayTable* dt)
{
    int day,h;
    bool result,checked;
    CSeance* se;
    result=false;
    checked=false;
    se=&(FSeanceTable[Sindex]);
    day=se->day ;
    h=se->hour  ;
    if((day<0)||(day>10)||(h<0)||(h>15))
    {
        parkSeance(Sindex,true);
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
                        &&(h<=se->hour+se->duree-1));
                checked=(h>=(se->hour+se->duree-1));
                if (!result || checked)
                    return result;
            }
    return true;
}

bool  emSolution::CheckAllDT(const int Sindex)
{
    CClasse* Cl;
    CSalle *Sa;
    CProf *Pr;
    CSeance *Se;
    bool  checkA,checkB,result,result1,result2;
    char s[254]="";

    //just for testing: emulate an erronous Seance:
    //FSeanceTable[0].day=1;
    //  FSalleTable[0].weekb[0,0]=111;
    result=false;

    Se=&(FSeanceTable[Sindex]);
    Cl=&(FClasseTable[Se->cindex]);
    Sa=&(FSalleTable[Se->saindex]);
    Pr=&(FProfTable[Se->pindex]);

    //check classe table

    checkA=checkDT(Sindex,&(Cl->weeka))  ;
    checkB=checkDT(Sindex,&(Cl->weekb))  ;

    result=((Se->bygroup
             &&

             (
                 ( (    (Se->groupedwith!=-1)
                        && (FSeanceTable[Se->groupedwith].groupedwith==Sindex)
                        && (FSeanceTable[Se->groupedwith].day==Se->day)
                        && (FSeanceTable[Se->groupedwith].hour==Se->hour)
                        && (FSeanceTable[Se->groupedwith].duree==Se->duree)


                   )
                    && (Se->dogroupwith==-1)

                 )
                 ||
                 (checkA && checkB)

             )

            )
            ||
            (
                (Se->byquinz && checkA)
                ||
                (Se->byquinz && checkB)
                ||
                (checkA && checkB)
            ) );



    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        wsprintf(s,"\t->[%d]  .. [ok]",Sindex);
    else
        wsprintf(s,"\n\t->[%d]  checking stage1 .. [failure]\n",Sindex);
    Console::Write(s);

    result1=result;
    //check salle table
    checkA=checkDT(Sindex,&(Sa->weeka))  ;
    checkB=checkDT(Sindex,&(Sa->weekb))  ;

    result=(Se->byquinz && checkA)
           ||
           (Se->byquinz && checkB)
           ||
           (checkA && checkB)  ;


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        wsprintf(s,"\t->[%d]  .. [ok]",Sindex);
    else
        wsprintf(s,"\n\t->[%d]  checking stage2 .. [failure]\n",Sindex);
    Console::Write(s);

    result2=result;

    //check prof table
    checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result=(Se->byquinz && checkA)
           ||
           (Se->byquinz && checkB)
           ||
           (checkA && checkB);


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        wsprintf(s,"\t->[%d]  .. [ok]",Sindex);
    else
        wsprintf(s,"\n\t->[%d]  checking stage3 .. [failure]\n",Sindex);
    Console::Write(s);

    //check grouping table
    /*checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result=(Se->byquinz && checkA)
           ||
           (Se->byquinz && checkB)
           ||
           (checkA && checkB);


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result)
        wsprintf(s,"   ->[%d]  .. [ok]",Sindex);
    else
        wsprintf(s,"\n   ->[%d]  checking stage3 .. [failure]\n",Sindex);
    Console::Write(s);
    */
    return (result1 && result2 && result);

}

bool   emSolution::verifyProcessedSeances(bool b)
{
    bool result=true;
//int pass=0;
    int Sindex=0;

    while (Sindex<FSeanceTableCount)
    {

        if  (!CheckAllDT(Sindex))
            result=false;
        Sindex++;
    }//while

    char s[254]="";
    Console::Write("\n");
    if (result)
        Console::Write(  "\t->verifying processed Seances -> [ok]\n");
    else
        Console::WriteEx("\t->verifying processed Seances -> [failure]\n",ColorRed);

    return result;
}



int    emSolution::SeanceTableCount(void)
{
    return FSeanceTableCount;

}

bool emSolution::setLink(const int s1,const int s2,ClinkType Alink)
{

    if((s1==-1)||(s2==-1))
        return false;
    //   gc_RaiseError('Error: setLink with invalid parameters!');

    switch(Alink)
    {
    case ltGroup: //set Grouped link Seance index

        FSeanceTable[s1].groupedwith=s2;
        FSeanceTable[s2].groupedwith=s1;
        FSeanceTable[s1].dogroupwith=s2;
        FSeanceTable[s2].dogroupwith=-1;
        FModified=true;
        break;

    case ltClear:

        FSeanceTable[s1].groupedwith=-1;
        FSeanceTable[s2].groupedwith=-1;
        FSeanceTable[s1].dogroupwith=-1;
        FSeanceTable[s2].dogroupwith=-1;
        FModified=true;
        break;

    }
    return true;
}

bool emSolution::RempliSalle(const int ASeance,const int ASalle,const int ADay,const int AHour,
                             const CFillOption fo,const int gw,bool abool)
{

    int i=0;
    bool result=false;
    int du=FSeanceTable[ASeance].duree;
    int classei=FSeanceTable[ASeance].cindex;
    int profi=FSeanceTable[ASeance].pindex;

    if (abool)
    {

        if (gw!=-1)
        {
            setLink(gw,ASeance,ltGroup);

        }
        FSeanceTable[ASeance].saindex=ASalle;   //fill with salle ind
        FSeanceTable[ASeance].hour=AHour;
        FSeanceTable[ASeance].day=ADay;
        FSeanceTable[ASeance].doquinzwith=(int)fo;

        for (i= 0; i<du; i++)
        {
            if ((fo==foMixte)||(fo==foSemaineA))
            {
                if (gw==-1)
                    FClasseTable[classei].weeka[ADay][AHour+i]=ASeance;
                FProfTable[profi].weeka[ADay][AHour+i]=ASeance;
                FSalleTable[ASalle].weeka[ADay][AHour+i]=ASeance;
            }
            if ((fo==foMixte)||(fo==foSemaineB))
            {
                if (gw==-1)
                    FClasseTable[classei].weekb[ADay][AHour+i]=ASeance;
                FProfTable[profi].weekb[ADay][AHour+i]=ASeance;
                FSalleTable[ASalle].weekb[ADay][AHour+i]=ASeance;
            }


        }
    }
    else
    {

        for (i= 0; i<du; i++)
        {

            if ((fo==foMixte)||(fo==foSemaineA))
            {
                FSalleTable[ASalle].weeka[ADay][AHour+i]=-1;
                FClasseTable[classei].weeka[ADay][AHour+i]=-1;
                FProfTable[profi].weeka[ADay][AHour+i]=-1;
            }
            if ((fo==foMixte)||(fo==foSemaineB))
            {
                FSalleTable[ASalle].weekb[ADay][AHour+i]=-1;
                FClasseTable[classei].weekb[ADay][AHour+i]=-1;
                FProfTable[profi].weekb[ADay][AHour+i]=-1;
            }

        }
        if (FSeanceTable[ASeance].groupedwith!=-1)
        {
            setLink(FSeanceTable[ASeance].groupedwith,ASeance,ltClear);

        }
        FSeanceTable[ASeance].saindex=-1;
        FSeanceTable[ASeance].hour=-1;
        FSeanceTable[ASeance].day=-1;
        FSeanceTable[ASeance].doquinzwith=(int)foNoWhere;
    }
    result=true;

    /*if (!result)
        Console::WriteEx("RepmliSalle failed!",ColorRed);*/

    return result;

}
void emSolution::clearAllDT()
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
            for (i= 0; i<SalleTableCount(); i++)
            {
                FSalleTable[i].weeka[d][h]=-1;
                FSalleTable[i].weekb[d][h]=-1;
            }
            for (i= 0; i<ProfTableCount(); i++)
            {
                FProfTable[i].weeka[d][h]=-1;
                FProfTable[i].weekb[d][h]=-1;
            }

        }
}

void emSolution::rebuildSolution()
{

    int e,gw;
    clearAllDT();
    for(e=0 ; e< FSeanceTableCount; e++)
    {
        if((FSeanceTable[e].groupedwith!=-1)&&(FSeanceTable[e].dogroupwith==-1))
            gw=FSeanceTable[e].groupedwith;
        else gw=-1;
        RempliSalle(e,
                    FSeanceTable[e].saindex,
                    FSeanceTable[e].day,
                    FSeanceTable[e].hour,
                    (CFillOption)(FSeanceTable[e].doquinzwith),
                    gw,
                    true
                   );

    }

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CFillOption emSolution::getSeanceFillMode(const int Se)
{
    int aday,ahour,sallei;

    CFillOption result=foMixte;

    if (FSeanceTable[Se].byquinz)
    {
        sallei=FSeanceTable[Se].saindex;
        aday=FSeanceTable[Se].day ;
        ahour=FSeanceTable[Se].hour;
        if (FSalleTable[sallei].weeka[aday][ahour]==Se)
            result=foSemaineA;
        else  if (FSalleTable[sallei].weekb[aday][ahour]==Se)
            result=foSemaineB;
        else
            result=foNoWhere;
    }
    return result;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool emSolution::CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour)
{
    return(
              (Se1!=-1)&&(Se2!=-1)&&(Ahour>=0)&&(Ahour<=15)                     &&
              (Se1!=Se2)                                                        &&
              FSeanceTable[Se1].bygroup                                         &&
              FSeanceTable[Se2].bygroup                                         &&
              (FSeanceTable[Se2].dogroupwith==-1)                               &&
              (FSeanceTable[Se1].groupedwith==-1)                               &&
              (FSeanceTable[Se2].groupedwith==-1)                               &&
              (FSeanceTable[Se1].byquinz==FSeanceTable[Se2].byquinz)            &&
              (FSeanceTable[Se1].duree==FSeanceTable[Se2].duree)                &&
              (Aday==FSeanceTable[Se2].day)                                     &&
              (Ahour==FSeanceTable[Se2].hour)
          );

}
void emSolution::shellSort()
{
    int n, i, j;
    CSeance *pSe;

    n=0;
    int Le=FSeanceTableCount;
    while(n<Le)
    {
        n=3*n+1;
    }

    while(n!=0)
    {
        n=n/3;
        for (i=n; i<Le; i++)
        {
            pSe=&FSeanceTable[i];
            j=i;

            while((j>(n-1)) && (FSeanceTable[j-n].duree>pSe->duree))
            {
                FSeanceTable[j]=FSeanceTable[j-n];
                j=j-n;
            }
            FSeanceTable[j]=*pSe;
        }
    }
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
void emSolution::SetTablesCount()
{

}
bool emSolution::viderSeance2(const int se,bool abool)
{

}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
CMat* emSolution::MatTable()
{
    return FMatTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CProf* emSolution::ProfTable()
{
    return FProfTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CSeance* emSolution::SeanceTable()
{
    return FSeanceTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CSalle* emSolution::SalleTable()
{
    return FSalleTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CClasse* emSolution::ClasseTable()
{
    return FClasseTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emSolution::MatTableCount(void)
{
    return FMatTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emSolution::SalleTableCount(void)
{
    return FSalleTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emSolution::ProfTableCount(void)
{
    return FProfTableCount;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int emSolution::ClasseTableCount(void)
{
    return FClasseTableCount;
}
int emSolution::SalleCountByType(int stype){
int i=0;
int result=0;
for(i=0;i<FSalleTableCount;i++){
    if(FSalleTable[i].stype==stype)
            result++;

    }

  return result;

}
