
#include <stdio.h>
#include "../3rdparty/Console/console.h"
#include "solution.h"




ScheduleSolution::ScheduleSolution()
{
    //printf("emSolution created!\n");
}

ScheduleSolution::~ScheduleSolution()
{
    //printf("emSolution destroyed!\n");
}
bool ScheduleSolution::saveHeader(CFileStream* stream)
{
    int sz;
    ScheduleConfig pc = emConfig;

    try
    {
        sz=sizeof(ScheduleConfig);
        stream->write(&sz, sizeof(sz));
        stream->write(&pc, sz);

    }
    catch(...)
    {
        return false;
    }
 
    return true;

}

ScheduleConfig ScheduleSolution::loadHeader(CFileStream* stream)
{
    int sz;
    ScheduleConfig pc;
    try
    {
        stream->read(&sz, sizeof(sz));
      
        stream->read(&pc, sz);
    }
    catch(...)
    {
        //return NULL;
        perror("\nloadHeader error!");
    }
    return pc;

}

bool ScheduleSolution::writeData(CFileStream* stream,WORD phase)
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
        stream->writeString(emConfig.magic);


    switch (phase)
    {
    case 0:
        //write count
        Le=_stats.crooms;
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
        Le=_dClasses.size();
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CClasse);
        stream->write(&sz ,sizeof(sz));

        for(i=0; i<Le; i++)
        {

            stream->write(&(_dClasses)[i],sz);         
            //write magic separateur
            stream->write(s,2);
        }
    
        break;
    case 2:
        //write count
        Le=_dShifts.size();
        stream->write(&Le, sizeof(Le));

        //write struct
        sz=sizeof(CShift);
        stream->write(&sz ,sizeof(sz));
      
        for(i=0; i<Le; i++)
        {

            stream->write(&_dShifts[i],sz);
            //write magic separateur
            stream->write(s,2);
        }

        break;
    case 3:
        //write count
        Le=_stats.profs;
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
        Le=_stats.mats;
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

int ScheduleSolution::readData(CFileStream* stream,WORD phase)
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
        CCroom sa;
        for(i=0; i<Le; i++)
        {
            stream->read(&sa,sz);
            FCroomTable[i]=sa;

            //read magic separateur
            stream->read(s,2);
        }

        break;
    case 1:
        //_dClasses.reserve(Le);
        //read struct
        stream->read(&sz ,sizeof(sz));
        CClasse cl;
        for(i=0; i<Le; i++)
        {
            stream->read(&cl,sz);
            _dClasses.emplace_back(cl);

            //read magic separateur
            stream->read(s,2);
        }     

        break;

    case 2:
        //_dShifts.reserve(Le);
        //read struct
        stream->read(&sz ,sizeof(sz));
        CShift se;
        for(i=0; i<Le; i++)
        {
            stream->read(&se,sz);
            _dShifts.emplace_back(se);

            //read magic separateur
            stream->read(s,2);
        }

        break;

    case 3:
        FProfTable=new CProf[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        CProf pr;
        for(i=0; i<Le; i++)
        {
            stream->read(&pr,sz);
            FProfTable[i]=pr;       

            //read magic separateur
            stream->read(s,2);
        }

        break;

    case 4:
        FMatTable=new CMat[Le];
        //read struct
        stream->read(&sz ,sizeof(sz));
        CMat ma;
        for(i=0; i<Le; i++)
        {
            stream->read(&ma,sz);
            FMatTable[i]=ma;

            //read magic separateur
            stream->read(s,2);
        }

        break;
    }

    return Le;

}

bool   ScheduleSolution::load(const char* filename)
{
    CFileStream fs(filename,cmOpen);
    emConfig = loadHeader(&fs);
    if(emConfig.magic_len != 255)
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

     _stats.crooms = readData(&fs, 0);
     _stats.classes = readData(&fs, 1);
     _stats.shifts = readData(&fs, 2);
     _stats.profs = readData(&fs, 3);
     _stats.mats = readData(&fs, 4);

    printf("STATS\n-----------------------\n");
    printf("%d\tClassrooms\n", _stats.crooms);
    printf("%d\tClasses\n", _stats.classes);
    printf("%d\tShifts\n", _stats.shifts);
    printf("%d\tProfessors\n", _stats.profs);
    printf("%d\tMats\n", _stats.mats);


    return true;
}

bool   ScheduleSolution::save(const char* filename)
{

    CFileStream fs(filename,cmCreate);

    if(saveHeader(&fs)!=true)
    {
        printf("error writing header!");
        return false;
    }

#if ENGINE_DEBUGMODE_LEVEL >2
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
    printf("nombre de croom:%d\n",_stats.crooms);
      printf("nombre de classe:%d\n",_dClasses.size());
      printf("nombre de shift:%d\n",_dShifts.size());
      printf("nombre de prof:%d\n",_stats.profs);
      printf("nombre de mat:%d\n",_stats.mats);
    
#endif

    return true;

}

int ScheduleSolution::getOrphinedShiftByDay(const int AIndex,const int ADay,const EScheduleMode emMode)
{
    int h, tmpA,tmpB;
    CDayTable  *wa,*wb;
    int result=0;
    switch (emMode)
    {
    case emClasse:
        wa=&(_dClasses[AIndex].weeka);
        wb=&(_dClasses[AIndex].weekb) ;
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

int ScheduleSolution::getOrphinedShift(const int AIndex,const EScheduleMode emMode)
{
    int d;
    int result=0;
    for(d=0; d<=5; d++)
        result=result+getOrphinedShiftByDay(AIndex,d,emMode);
    return result;
}
int ScheduleSolution::getIdxByDaytable(CDayTable *Adt)
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
                du=_dShifts[Se].length;
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

int ScheduleSolution::getCompactIdx(const int AIndex,const EScheduleMode emMode)
{

    int result=0;
    switch(emMode)
    {
    case emClasse:
        result=getIdxByDaytable(&_dClasses[AIndex].weeka);
        break;
    case emProf:
        result=getIdxByDaytable(&FProfTable[AIndex].weeka);
        break;

    }
    return result;
}
int ScheduleSolution::getShiftsCount(const int AIndex,const EScheduleMode emMode)
{

    int result=0;
    int i;
    if (AIndex<0)
        return 0;

    switch(emMode)
    {
    case emClasse:
        for (i=0 ; i<_dShifts.size(); i++)
            if(_dShifts[i].cindex==AIndex)
                result++;
        break;
    case emProf:
        for (i=0 ; i<_dShifts.size(); i++)
            if(_dShifts[i].pindex==AIndex)
                result++;
        break;
    case emMat:
        for (i=0 ; i<_dShifts.size(); i++)
            if(_dShifts[i].mindex==AIndex)
                result++;
        break;
    case emCroom:
        for (i=0 ; i<_dShifts.size(); i++)
            if(_dShifts[i].crindex==AIndex)
                result++;
        break;
    }
    return result;

}

bool    ScheduleSolution::getOptimizeValue(int *hp,int*hc,int*cp,int*cc)
{

    int e;
    *hp=0;
    *hc=0;
    *cp=0;
    *cc=0;

    for(e=0 ; e< _dClasses.size(); e++)
    {
        *hc=*hc+getOrphinedShift(e,emClasse);
        *cc=*cc+getCompactIdx(e,emClasse);
    }

    for(e=0 ; e< _stats.profs; e++)
    {
        *hp=*hp+getOrphinedShift(e,emProf);
        *cp=*cp+getCompactIdx(e,emProf);
    }

    return true;

}

bool ScheduleSolution::parkShift(const int Sindex,bool ABool)
{

    return true;
}

bool  ScheduleSolution::checkDT(const int Sindex,CDayTable* dt)
{
    int day,h;
    bool result,checked;
    CShift* se;
    result=false;
    checked=false;
    se=&(_dShifts[Sindex]);
    day=se->day ;
    h=se->hour  ;
    if((day<0)||(day>10)||(h<0)||(h>15))
    {
        parkShift(Sindex,true);
        return false;
    }

    if ((*dt)[day][h] != Sindex) {
        return false;
    }
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

bool  ScheduleSolution::CheckAllDT(const int Sindex)
{
    CClasse* Cl;
    CCroom *Sa;
    CProf *Pr;
    CShift *Se;
    bool  checkA,checkB,result0,result1,result2;
    char s[254] = "";

    //just for testing: emulate an erronous Shift:
    //_dShifts[0].day=1;
    //  FCroomTable[0].weekb[0,0]=111;
    result0=false;

    Se=&(_dShifts[Sindex]);
    Cl=&(_dClasses[Se->cindex]);
    Sa=&(FCroomTable[Se->crindex]);
    Pr=&(FProfTable[Se->pindex]);

    //check classe table

    checkA=checkDT(Sindex,&(Cl->weeka))  ;
    checkB=checkDT(Sindex,&(Cl->weekb))  ;

    result0=((Se->bygroup
             &&

             (
                 ( (    (Se->groupedwith!=-1)
                        && (_dShifts[Se->groupedwith].groupedwith==Sindex)
                        && (_dShifts[Se->groupedwith].day==Se->day)
                        && (_dShifts[Se->groupedwith].hour==Se->hour)
                        && (_dShifts[Se->groupedwith].length==Se->length)


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
    if (result0) {
        sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
    }
    else {
        sprintf_s(s, "\n\t->[%d]  checking stage1 .. [failure] \n checkA:%d,checkB:%d\n", Sindex,checkA,checkB);
      
    }
    Console::Write(s);

    result1=result0;
    //check croom table
    checkA=checkDT(Sindex,&(Sa->weeka))  ;
    checkB=checkDT(Sindex,&(Sa->weekb))  ;

    result0=(Se->every2weeks && checkA)
           ||
           (Se->every2weeks && checkB)
           ||
           (checkA && checkB)  ;


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result0) {
        sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
    }
    else {
        sprintf_s(s, "\n\t->[%d]  checking stage2 .. [failure] \n checkA:%d,checkB:%d\n", Sindex, checkA, checkB);
    }
    Console::Write(s);

    result2=result0;

    //check prof table
    checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result0=(Se->every2weeks && checkA)
           ||
           (Se->every2weeks && checkB)
           ||
           (checkA && checkB);


    // sleep(10);
    //initialize cursor pos
    Console::SetCursorPosition(3,Console::GetCursorY());
    if (result0) {
        sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
    }
    else {
        sprintf_s(s, "\n\t->[%d]  checking stage3 .. [failure] \n checkA:%d,checkB:%d\n", Sindex, checkA, checkB);
    }
    Console::Write(s);

    //check grouping table
    /*checkA=checkDT(Sindex,&(Pr->weeka))  ;
    checkB=checkDT(Sindex,&(Pr->weekb))  ;

    result=(Se->_current.every2weeks && checkA)
           ||
           (Se->_current.every2weeks && checkB)
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
    return (result1 && result2 && result0);

}

bool   ScheduleSolution::verifyProcessedShifts(bool b)
{
    bool result=true;
//int pass=0;
    int Sindex=0;

    while (Sindex<_dShifts.size())
    {

        if  (!CheckAllDT(Sindex))
            result=false;
        Sindex++;
    }//while

    char s[254]="";
    Console::WriteLine("");
    if (result)
        Console::WriteLine(  "\t->verifying processed Shifts -> [ok]");
    else
        Console::WriteEx("\t->verifying processed Shifts -> [failure]\n",ColorRed);

    return result;
}


bool ScheduleSolution::setLink(const int s1,const int s2,ELinkType Alink)
{

    if((s1==-1)||(s2==-1))
        return false;
    //   gc_RaiseError('Error: setLink with invalid parameters!');

    switch(Alink)
    {
    case ltGroup: //set Grouped link Shift index

        _dShifts[s1].groupedwith=s2;
        _dShifts[s2].groupedwith=s1;
        _dShifts[s1].dogroupwith=s2;
        _dShifts[s2].dogroupwith=-1;
        FModified=true;
        break;

    case ltClear:

        _dShifts[s1].groupedwith=-1;
        _dShifts[s2].groupedwith=-1;
        _dShifts[s1].dogroupwith=-1;
        _dShifts[s2].dogroupwith=-1;
        FModified=true;
        break;

    }
    return true;
}

bool ScheduleSolution::fillCroom(const int AShift,const int ACroom,const int ADay,const int AHour,
                             const EFillMode fo,const int gw,bool abool)
{

    int i=0;
    bool result=false;
    int du=_dShifts[AShift].length;
    int classei=_dShifts[AShift].cindex;
    int profi=_dShifts[AShift].pindex;

    if (abool)
    {

        if (gw!=-1)
        {
            setLink(gw,AShift,ltGroup);

        }
        _dShifts[AShift].crindex=ACroom;   //fill with croom ind
        _dShifts[AShift].hour=AHour;
        _dShifts[AShift].day=ADay;
        _dShifts[AShift].doAlternatewith=(int)fo;

        for (i= 0; i<du; i++)
        {
            if ((fo==foMixte)||(fo==foWeekA))
            {
                if (gw==-1)
                    _dClasses[classei].weeka[ADay][AHour+i]=AShift;
                FProfTable[profi].weeka[ADay][AHour+i]=AShift;
                FCroomTable[ACroom].weeka[ADay][AHour+i]=AShift;
            }
            if ((fo==foMixte)||(fo==foWeekB))
            {
                if (gw==-1)
                    _dClasses[classei].weekb[ADay][AHour+i]=AShift;
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
                _dClasses[classei].weeka[ADay][AHour+i]=-1;
                FProfTable[profi].weeka[ADay][AHour+i]=-1;
            }
            if ((fo==foMixte)||(fo==foWeekB))
            {
                FCroomTable[ACroom].weekb[ADay][AHour+i]=-1;
                _dClasses[classei].weekb[ADay][AHour+i]=-1;
                FProfTable[profi].weekb[ADay][AHour+i]=-1;
            }

        }
        if (_dShifts[AShift].groupedwith!=-1)
        {
            setLink(_dShifts[AShift].groupedwith,AShift,ltClear);

        }
        _dShifts[AShift].crindex=-1;
        _dShifts[AShift].hour=-1;
        _dShifts[AShift].day=-1;
        _dShifts[AShift].doAlternatewith=(int)foNoWhere;
    }
    result=true;

    /*if (!result)
        Console::WriteEx("RepmliCroom failed!",ColorRed);*/

    return result;

}
void ScheduleSolution::clearAllDT()
{

    int d,h,i;
    for (d= 0; d<11; d++)
        for (h= 0; h<=15; h++)
        {
            for (i= 0; i< _dClasses.size(); i++)
            {
                _dClasses[i].weeka[d][h]=-1;
                _dClasses[i].weekb[d][h]=-1;
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

void ScheduleSolution::rebuildSolution()
{

    int e,gw;
    clearAllDT();
    for(e=0 ; e< _dShifts.size(); e++)
    {
        if((_dShifts[e].groupedwith!=-1)&&(_dShifts[e].dogroupwith==-1))
            gw=_dShifts[e].groupedwith;
        else gw=-1;
        fillCroom(e,
                    _dShifts[e].crindex,
                    _dShifts[e].day,
                    _dShifts[e].hour,
                    (EFillMode)(_dShifts[e].doAlternatewith),
                    gw,
                    true
                   );

    }

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
EFillMode ScheduleSolution::getShiftFillMode(const int Se)
{
    int aday,ahour,croomi;

    EFillMode result=foMixte;

    if (_dShifts[Se].every2weeks)
    {
        croomi=_dShifts[Se].crindex;
        aday=_dShifts[Se].day ;
        ahour=_dShifts[Se].hour;
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
bool ScheduleSolution::CanBeByGroup(const int Se1,const int Se2,const int Aday,const int Ahour)
{
    return(
              (Se1!=-1)&&(Se2!=-1)&&(Ahour>=0)&&(Ahour<=15)                     &&
              (Se1!=Se2)                                                        &&
              _dShifts[Se1].bygroup                                         &&
              _dShifts[Se2].bygroup                                         &&
              (_dShifts[Se2].dogroupwith==-1)                               &&
              (_dShifts[Se1].groupedwith==-1)                               &&
              (_dShifts[Se2].groupedwith==-1)                               &&
              (_dShifts[Se1].every2weeks==_dShifts[Se2].every2weeks)            &&
              (_dShifts[Se1].length==_dShifts[Se2].length)                &&
              (Aday==_dShifts[Se2].day)                                     &&
              (Ahour==_dShifts[Se2].hour)
          );

}
void ScheduleSolution::shellSort()
{
    int n, i, j;
    CShift *pSe;

    n=0;
    int Le=_dShifts.size();
    while(n<Le)
    {
        n=3*n+1;
    }

    while(n!=0)
    {
        n=n/3;
        for (i=n; i<Le; i++)
        {
            pSe=&_dShifts[i];
            j=i;

            while((j>(n-1)) && (_dShifts[j-n].length>pSe->length))
            {
                _dShifts[j]=_dShifts[j-n];
                j=j-n;
            }
            _dShifts[j]=*pSe;
        }
    }
}

bool ScheduleSolution::clearShift(const int se,bool abool)
{
    return true;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
CMat* ScheduleSolution::MatTable()
{
    return FMatTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CProf* ScheduleSolution::ProfTable()
{
    return FProfTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
std::vector<CShift>& ScheduleSolution::ShiftTable()
{
    return _dShifts;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
CCroom* ScheduleSolution::CroomTable()
{
    return FCroomTable;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
std::vector<CClasse>& ScheduleSolution::ClasseTable()
{
    return _dClasses;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int ScheduleSolution::MatTableCount(void)
{
    return _stats.mats;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int ScheduleSolution::CroomTableCount(void)
{
    return _stats.crooms;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int ScheduleSolution::ProfTableCount(void)
{
    return _stats.profs;
}


int ScheduleSolution::CroomCountByType(int cr_type){
int i=0;
int result=0;
for(i=0;i<_stats.crooms;i++){
    if(FCroomTable[i].stype==cr_type)
            result++;

    }

  return result;

}
