#include <ctime>
#include "Console/console.h"
#include "classes.h"
#include "I_engine.h"

using namespace std;

#define VERSION "1.0"
#define VERSION_SUFFIX "rev224"

void getTimeStr(char *buf){
    time_t t=time(0);
    struct tm  *now=localtime(&t);
    strftime( buf, 128,"time%d%m%y-%H%M%S", now );
}
const char *get_version(void)
{
	return VERSION"-"VERSION_SUFFIX;
}
void printversion(void){
    printf("\n");
    printf("Gemp generator %s\n", get_version());
    printf("By Adel BEN HAMADI bhamadi_adel@yahoo.fr\n");
    printf("(C) 2012 Adel BEN HAMADI\n\n");

}
void printhelp(void)
{

    printf("\n");
    printf("Gemp generator %s\n", get_version());

    printf("\t--help                -h             Print this help screen\n");
    printf("\t--version             -V             Print version number\n");
    printf("\t--debug               -D             Enable Gemp engine debug messages\n");
    printf("\t--quiet               -Q             Only output error messages\n");
    printf("\t--stdout                             Write to stdout instead of stderr\n");
    printf("\t--no-summary                         Disable summary at end of searching\n");


    printf("\n");
  //  printf("(*) Default settings\n");

}

bool loadEmploiLib(HINSTANCE libDLL,I_Engine_Intf ** pInterface){
GETINTERFACE    pfnGetIntf=0; //pointer to GetMyInterface function
//I_Engine_Intf * pInterface  =0;

if(libDLL != NULL)
{
   //Get the functions address
   pfnGetIntf= (GETINTERFACE)::GetProcAddress(libDLL,"GetEngineIntf");

   //Release Dll if we werent able to get the function
   if(pfnGetIntf == 0)
   {
        ::FreeLibrary(libDLL);
        return true;
   }

   //Call the Function
   HRESULT hr = pfnGetIntf(pInterface);

   //Release if it didnt work
   if(FAILED(hr))
   {
        ::FreeLibrary(libDLL);
        return false;
   }

  return true;
 }

}
void freeEmploiLib(HINSTANCE libDLL,I_Engine_Intf ** pInterface){
 //How to release the interface

   FREEINTERFACE pfnFreeIntf = (FREEINTERFACE )::GetProcAddress(libDLL,"FreeEngineIntf");
   if(pfnFreeIntf != 0)
        pfnFreeIntf(pInterface);

   //Release the DLL if we dont have any use for it now
   ::FreeLibrary(libDLL);

}


int main(int argc, char *argv[], char *envp[])
{

   char pm_datadir[255] ="c:\\emploi\\out\\";//ExtractFilePath();
   char str1[255],str2[128];

   CEngineConfig *engine_cfg;
   engine_cfg = ( CEngineConfig *) malloc(sizeof( CEngineConfig));

  if(argc==2){
     if((_stricmp( argv[1], "-h" ) == 0 )||(_stricmp( argv[1], "--help" ) == 0 )){
     printhelp();
     return 0;
     }
     if((_stricmp( argv[1], "-V" ) == 0 )||(_stricmp( argv[1], "--version" ) == 0 )){
     printversion();
     return 0;
     }
  }
     engine_cfg->slowprinting=true;
     engine_cfg->optimizebranching=false;

     int iarg;
     for(iarg=1;iarg<argc-1;iarg++){
        if(_stricmp( argv[iarg], "-f" ) == 0 )
            engine_cfg->configfile=argv[iarg+1];
       else if(_stricmp( argv[iarg], "-hl")==0)
            engine_cfg->hltx=atof(argv[iarg+1]);
       else if(_stricmp( argv[iarg], "-ll")==0)
            engine_cfg->lltx=atof(argv[iarg+1]);
       else if(_stricmp( argv[iarg], "-ml")==0)
            engine_cfg->mltx=atof(argv[iarg+1]);
       else if(_stricmp( argv[iarg], "--slowprinting")==0)
            engine_cfg->slowprinting=true;
       else if(_stricmp( argv[iarg], "--optimizebranching")==0)
            engine_cfg->optimizebranching=true;

     }

   Console::initConsoleScreen();
   Console::HighVideo();
   Console::Clear();
   Console::SetTitle("Testing console!");
   Console::SetFGColor(ColorWhite);

   int ecount=0;
      printf("\nPress any key to cancel ");
      while(!Console::KeyDown() && ecount<5){
        Console::Write(".");
        Sleep(100);

        ecount++;
      } ;

     if (ecount<5)  exit(0);
   // MessageBeep(-1);
    Beep(750, 175 );

       Console::Clear();
       Console::WriteEx("Gemp Generetor v1.0\n",ColorYellow,ColorBlue);
       printf("Copyright 2012 Adel BEN HAMADI. All rights reserved\n\n");
       printf("-------------------------------------------------------\n");
       printf("VERSION:         %s\n",get_version());
       printf("DEBUG MODE:      ");
       #if ENGINE_DEBUGMODE_LEVEL > 0
       Console::WriteEx("enabled\n",ColorWhite,ColorRed);
       #else
       Console::WriteEx("disabled\n",ColorSilver);
       #endif

   //load engine
     I_Engine_Intf * pEngineIntf  =0;
     CEnginePlugin * pPluginInfo;
     HINSTANCE hEmploiDll = ::LoadLibrary("emploi-lib-e13.dll");
      try{
      if(loadEmploiLib(hEmploiDll,&pEngineIntf)!=true)
        exit(-1);

      pPluginInfo=pEngineIntf->getPluginInfo() ;


      if(pEngineIntf->Load(engine_cfg) )
            printf("CONFIG FILE:     %s..[ok]\n",engine_cfg->configfile);
          else
          {
            printf("CONFIG FILE:     %s..[failed]\n",engine_cfg->configfile);
            return -1;
          }
      }
      catch(...){
      exit(-1);

      }
      printf("ENGINE:          v%d\n",pPluginInfo->version_maj);

      printf("-------------------------------------------------------");

      Console::SetCursorPosition(3,Console::GetCursorY());


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
     while (sol<1000000){
     Console::SetCursorPosition(4,Console::GetCursorY()+1);
     Console::Write("\n");
     Console::Write("---------------------\n");
     printf(" Solution:%d  P:%.2f%%\n",sol,max_percent);

     Console::Write("---------------------\n");
     pEngineIntf->execute(sol==1,/*(sol_retenu=0)or(sol-sol_retenu > 5)*/sol % 3==1);
     // Sleep(250);

     //Console::SetCursorPosition(6,Console::GetCursorY()+1);
     Console::Write("\n");
     //pEngineIntf->rebuildSolution();
     pEngineIntf->getOptimizeValue(&valp,&valc,&osp,&osc);

     wsprintf(str1,"\t->solution found:%d %d/%d/%d/%d\n",sol,valp,valc,osp,osc);
     Console::Write(str1);

     if (pEngineIntf->verifySolution(false)==false) {
      notaccepted++;
      Console::WriteEx("\t->solution not accepted!\n",ColorRed);
     }
       else
      {

       wsprintf(str1,"\t->solution accepted[%d/%d]\n",sol-notaccepted,sol) ;
       Console::WriteEx(str1,ColorGreen);


        val=valp+osp/*+valc+osc*/;
        a_percent=(double) (100*(1-(val/pEngineIntf->SeancesCount()))) ;
        printf("\t->optimized at %.2f%% P:%d/%d C:%d/%d\n",a_percent,valp,osp,valc,osc);

       if (val<oldval ){
             sol_retenu=sol;
             max_percent=a_percent;
             oldval= val;
             oldvalp=valp;
             oldvalc=valc;
             oldosp=osp;
             oldosc=osc;
             printf("\t->solution autosaved P:%d/%d C:%d/%d\n",valp,osp,valc,osc);


            getTimeStr(str2);
            wsprintf(str1,"sol-v%d-%d-%d-%s.gedt",pPluginInfo->version_maj,valp,valc,str2);
            pEngineIntf->Save(str1);
            MessageBeep(0xFFFFFFFF);// MB_ICONASTERISK MB_ICONEXCLAMATION MB_ICONOK
          if (val==0)
                break;

         }
      }
     sol++;
    }//while
    pEngineIntf->Release();

     //end
    freeEmploiLib(hEmploiDll,&pEngineIntf);
}
