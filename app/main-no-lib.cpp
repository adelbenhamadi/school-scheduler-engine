#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <time.h>

#include "../3rdparty/Console/console.h"

#include "../engine/solution.h"
#include "../engine/engine.h"

using namespace std;



void getTimeStr(char *buf)
{
    time_t t=time(0);
    struct tm  *now=localtime(&t);
    strftime( buf, 128,"time%d%m%y-%H%M%S", now );
}

int main(int argc, char *argv[], char *envp[])
{
    char *pm_configfile;//[254]="lycee-sec-siliana-final-mixte1_uncompressed.edt";
    char pm_datadir[255] ="c:\\emploi\\out\\";//ExtractFilePath();
    char str1[255],str2[128];
    double hltx=95.0;
    double lltx=85.0;
    double mltx=90.0;
    bool justCheck=false;

    if (argc >= 3)
    {

        if(_stricmp( argv[1], "-f" ) == 0 )
            pm_configfile=argv[2];
        if((argc>=4)&&(_stricmp( argv[3], "-c" ) == 0 )){
            justCheck=true;
        }
         else{
            if((argc>=5)&&(_stricmp( argv[3], "-hl")==0))
                hltx=atof(argv[4]);
            if((argc>=7)&&(_stricmp( argv[5], "-ll")==0))
                lltx=atof(argv[6]);
            if((argc>=9)&&(_stricmp( argv[7], "-ml")==0))
                mltx=atof(argv[8]);
       }
    } ;




    Console::initConsoleScreen();
    Console::HighVideo();
    Console::Clear();
    Console::SetTitle("Testing console!");
    Console::SetFGColor(ColorWhite);

    int ecount=0;
    printf("\nPress any key to cancel ");
    while(!Console::KeyDown() && ecount<5)
    {
        Console::Write(".");
        Sleep(100);

        ecount++;
    } ;

    if (ecount<5)  exit(0);

    Console::Clear();
    Console::WriteEx("Gemp Generetor v1.0\n",ColorYellow,ColorBlue);
    printf("Copyright 2012 Adel BEN HAMADI. All rights reserved\n\n");

    printf("-------------------------------------------------------\n");
    printf("VERSION:         1.0.0.0 rev 000\n");
    printf("DEBUG MODE:      ");
#if ENGINE_DEBUGMODE_LEVEL > 0
    Console::WriteEx("enabled\n",ColorWhite,ColorRed);
#else
    Console::WriteEx("disabled\n",ColorSilver);
#endif
    //load engine


    printf("ENGINE:          %s\n",ENGINENAME);
    printf("CONFIG FILE:     %s\n",pm_configfile);
    printf("-------------------------------------------------------\n");

    Console::SetCursorPosition(3,Console::GetCursorY()+1);
    emEngine* engine;
    // engine=new emEngine(pm_configfile);
    try
    {

        engine=new emEngine(pm_configfile);

    }
    catch(...)
    {
        exit(ENGINE_ERROR_LOADING);

    }
    if(engine==NULL)
        exit(ENGINE_ERROR_LOADING);
    int valp,valc,osp,osc;
    int multi=15;
    int oldvalc=10*multi;
    int oldvalp=10*multi;
    int oldosp=10*multi;
    int oldosc=10*multi;
    double val;
    double oldval=oldvalp+oldosp+oldvalc+oldosc;

    double a_percent=0;
    double max_percent=0;

    ecount=1;
    int sol=1;
    int notaccepted=0;
    int sol_retenu=0;

    if(justCheck==true){
        Console::SetCursorPosition(4,Console::GetCursorY()+1);
        Console::Write("\n");
        Console::Write("Entering checking mode...\n");
        Console::Write("Checking solution:  ");
        Console::WriteLine(pm_configfile);
        engine->solution.rebuildSolution();
        printf("rebuild solution..[ok]\n\n");
        engine->solution.getOptimizeValue(&valp,&valc,&osp,&osc);
        wsprintf(str1,"->solution optimize info:%d %d/%d/%d/%d\n",sol,valp,valc,osp,osc);
        Console::Write(str1);


        if (engine->solution.verifyProcessedSeances(false)==false)
            {
                Console::WriteEx("->invalid solution!\n",ColorRed);
            }
            else
            {

                Console::WriteEx("->this is a valid solution!\n",ColorGreen) ;

                val=valp+osp/*+valc+osc*/;
                a_percent=(double) (100*(1-(val/engine->solution.SeanceTableCount()))) ;
                printf("->optimized at %.2f%% P:%d/%d C:%d/%d\n",a_percent,valp,osp,valc,osc);
            }
        printf("\nPress any key to exit ");
        while(true){
           if (Console::KeyDown())
            exit(0);
        }

    }

        while (sol<1000000)
        {
            Console::SetCursorPosition(4,Console::GetCursorY()+1);
            Console::Write("\n");
            Console::Write("---------------------\n");
            printf(" Solution:%d  P:%.2f%%\n",sol,max_percent);

            Console::Write("---------------------\n");
            engine->execute(sol==1,/*(sol_retenu=0)or(sol-sol_retenu > 5)*/sol % 3==1,hltx,lltx,mltx);
            // Sleep(250);

            Console::Write("\n");
            //engine->solution.rebuildSolution();
            engine->solution.getOptimizeValue(&valp,&valc,&osp,&osc);
            wsprintf(str1,"  ->solution found:%d %d/%d/%d/%d\n",sol,valp,valc,osp,osc);
            Console::Write(str1);

            if (engine->solution.verifyProcessedSeances(false)==false)
            {
                notaccepted++;
                Console::WriteEx("  ->solution not accepted!\n",ColorRed);
            }
            else
            {

                wsprintf(str1,"  ->solution accepted[%d/%d]\n",sol-notaccepted,sol) ;
                Console::WriteEx(str1,ColorGreen);


                val=valp+osp/*+valc+osc*/;
                a_percent=(double) (100*(1-(val/engine->solution.SeanceTableCount()))) ;
                printf("  ->optimized at %.2f%% P:%d/%d C:%d/%d\n",a_percent,valp,osp,valc,osc);

                if (val<oldval )
                {
                    sol_retenu=sol;
                    max_percent=a_percent;
                    oldval= val;
                    oldvalp=valp;
                    oldvalc=valc;
                    oldosp=osp;
                    oldosc=osc;
                    printf("  ->solution autosaved P:%d/%d C:%d/%d\n",valp,osp,valc,osc);


                    getTimeStr(str2);
                    wsprintf(str1,"sol-e%s-%d-%d-%s.edt",ENGINENAME,valp,valc,str2);
                    engine->solution.SaveToFile(str1);
                    MessageBeep(MB_ICONASTERISK);// MB_ICONASTERISK MB_ICONEXCLAMATION MB_ICONOK
                    if (val==0)
                        break;

                }
            }
            sol++;
        }//while


    delete engine;
    exit(0);

    /* CemSolution *sol=new CemSolution();
     try{

     sol->LoadFromFile("lycee-sec-siliana-final-mixte1_uncompressed_saved.edt");
     //sol->SaveToFile("lycee-sec-siliana-final-mixte1_uncompressed_saved.edt");
     }
     catch( char * err ) {
       printf("Exception raised: %s error n�:%d\n",err,(int)GetLastError());
     }
     delete sol;// sol.~CemSolution();
     */


}
