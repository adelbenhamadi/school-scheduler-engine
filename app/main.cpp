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

#define VERSION "0.2"
#define VERSION_SUFFIX "rev225"
#define USE_LIB 0

void getTimeStr(char* buf) {
	time_t t = time(0);
	struct tm* now = localtime(&t);
	strftime(buf, 128, "time%d%m%y-%H%M%S", now);
}
const char* get_version(void)
{
	return VERSION "-" VERSION_SUFFIX;
}
void printversion(void) {
	printf("\n");
	printf("Scheduler Engine %s\n", get_version());
	printf("(C) 2012-2021 Adel BEN HAMADI <benhamadi.adel@gmail.com>\n\n");

}
void printhelp(void)
{

	printf("\n");
	printf("Scheduler Engine %s\n", get_version());

	printf("\t--help                -h             Print this help screen\n");
	printf("\t--version             -V             Print version number\n");
	printf("\t--debug               -D             Enable engine debug messages\n");
	printf("\t--quiet               -Q             Only output error messages\n");
	printf("\t--stdout                             Write to stdout instead of stderr\n");
	printf("\t--no-summary                         Disable summary at end of searching\n");
	printf("\t--verbose            -v              verbose some info!\n");

	printf("\n");
	//  printf("(*) Default settings\n");

}
#if USE_LIB
bool loadLibrary(HINSTANCE libDLL, I_Engine_Intf** pInterface) {
	GETINTERFACE    pfnGetIntf = 0; //pointer to GetMyInterface function
	//I_Engine_Intf * pInterface  =0;

	if (libDLL != NULL)
	{
		//Get the functions address
		pfnGetIntf = (GETINTERFACE)::GetProcAddress(libDLL, "GetEngineIntf");

		//Release Dll if we werent able to get the function
		if (pfnGetIntf == 0)
		{
			::FreeLibrary(libDLL);
			return false;
		}

		//Call the Function
		HRESULT hr = pfnGetIntf(pInterface);

		//Release if it didnt work
		if (FAILED(hr))
		{
			::FreeLibrary(libDLL);
			return false;
		}

		return true;
	}
	return false;
}
void freeLibrary(HINSTANCE libDLL, I_Engine_Intf** pInterface) {
	//How to release the interface

	FREEINTERFACE pfnFreeIntf = (FREEINTERFACE)::GetProcAddress(libDLL, "FreeEngineIntf");
	if (pfnFreeIntf != 0)
		pfnFreeIntf(pInterface);

	//Release the DLL if we dont have any use for it now
	::FreeLibrary(libDLL);

}
#endif

int main(int argc, char* argv[], char* envp[])
{

	char pm_datadir[255] = "./out";
	char str1[255], str2[128];

	CEngineConfig engine_cfg;


	if (argc == 2) {
		if ((_stricmp(argv[1], "-h") == 0) || (_stricmp(argv[1], "--help") == 0)) {
			printhelp();
			return 0;
		}
		if ((_stricmp(argv[1], "-V") == 0) || (_stricmp(argv[1], "--version") == 0)) {
			printversion();
			return 0;
		}
	}
	engine_cfg.slowprinting = true;
	engine_cfg.optimizebranching = false;

	int iarg;
	for (iarg = 1; iarg < argc - 1; iarg++) {
		if (_stricmp(argv[iarg], "-f") == 0)
			engine_cfg.configfile = argv[iarg + 1];
		else if (_stricmp(argv[iarg], "-hl") == 0)
			engine_cfg.hltx = atof(argv[iarg + 1]);
		else if (_stricmp(argv[iarg], "-ll") == 0)
			engine_cfg.lltx = atof(argv[iarg + 1]);
		else if (_stricmp(argv[iarg], "-ml") == 0)
			engine_cfg.mltx = atof(argv[iarg + 1]);
		else if (_stricmp(argv[iarg], "--slowprinting") == 0)
			engine_cfg.slowprinting = true;
		else if (_stricmp(argv[iarg], "--optimizebranching") == 0)
			engine_cfg.optimizebranching = true;

	}
	if (!engine_cfg.configfile) {
		printf("No Config file provided!\n");
		exit(0);
	}
	Console::initConsoleScreen();
	Console::HighVideo();
	Console::Clear();
	Console::SetTitle("Testing console!");
	Console::SetFGColor(ColorWhite);


	int ecount = 0;

	Console::Clear();
	Console::WriteEx("Scheduler Engine \n", ColorBlack, ColorCyan);
	printf("(c) 2012-2021 Adel BEN HAMADI <benhamadi.adel@gmail.com>\n");
	printf("---------------------------------------------------------\n");
	printf("VERSION:         %s\n", get_version());
	printf("DEBUG MODE:      ");
#if ENGINE_DEBUGMODE_LEVEL > 0
	Console::WriteEx("enabled\n", ColorBlack, ColorRed);
#else
	Console::WriteEx("disabled\n", ColorSilver);
#endif

	//load engine
	printf("ENGINE:          %s\n", ENGINENAME);
	Console::SetCursorPosition(3, Console::GetCursorY() + 1);
#if USE_LIB
	I_Engine_Intf engine;
	HINSTANCE hEngineLib = ::LoadLibrary("libEngine.dll");
	try {
		if (loadLibrary(hEngineLib, engine) != true) {
			perror("\nUnable to load libraray interface from 'libEngine.dll'");
			exit(-1);
		}

		if (engine.load(engine_cfg))
			printf("LOAD CONFIG FILE:     %s..[ok]\n", engine.config().configfile);
		else
		{
			printf("LOAD CONFIG FILE:     %s..[failed]\n", engine.config().configfile);
			perror("\nUnable to load config file");
			exit(-1);
		}
	}
	catch (...) {
		exit(-1);
	}
#else
	ScheduleEngine engine;

	try
	{

		if (engine.load(engine_cfg)) {
			printf("CONFIG FILE:           %s\n", engine.config().configfile);
			printf("ENGINE:                v%d\n", engine.pluginInfo().version_maj);
			printf("OPTIMIZE BRANCHING:    ");
#if  OPTIMIZE_BRANCHING == 1
			Console::WriteEx("enabled\n", ColorRed);
#else  
			Console::WriteEx("disabled\n", ColorSilver);
#endif
			ScheduleSolution::Stats& stats = engine.solution().stats();
			printf("STATS:\n");
			printf("\t%d\tClassrooms\n", stats.crooms);
			printf("\t%d\tClasses\n", stats.classes);
			printf("\t%d\tShifts\n", stats.shifts);
			printf("\t%d\tProfessors\n", stats.profs);
			printf("\t%d\tMats\n", stats.mats);
		}
		else
		{
			printf("LOAD CONFIG FILE:      %s..[failed]\n", engine.config().configfile);
			perror("\nUnable to load config file");
			exit(-1);
		}
	}
	catch (...)
	{
		exit(ENGINE_ERROR_LOADING);
	}
#endif


	printf("-----------------------------------------------------------");

	//start searching
	Console::SetCursorPosition(3, Console::GetCursorY());


	const int multi = 15;
	double val = 0.0;
	double oldval = (double)10.0 * multi + 10.0 * multi;

	double a_percent = 0.0;
	double max_percent = 0.0;

	ecount = 1;
	int sol = 1;
	int notaccepted = 0;
	int sol_retenu = 0;
	Console::SetCursorPosition(4, Console::GetCursorY() + 1);
	while (sol < 1000000) {
		
		Console::WriteLine("");
		Console::WriteLine("---------------------");
		printf("Solution:%d  P:%.2f%%\n", sol, max_percent);
		Console::WriteLine("---------------------");
		engine.execute(sol == 1,/*(sol_retenu=0)or(sol-sol_retenu > 5)*/sol % 3 == 1);

		Console::WriteLine("");
		//engine.rebuildSolution();


		auto classeOI = engine.solution().getOptimizeInfo(emClasse);
		auto profOI = engine.solution().getOptimizeInfo(emProf);

		
		Console::Write("\t->Solution found");

		if (engine.checkSolution(false) == false) {
			notaccepted++;
			Console::WriteEx("\t->Checking:   [failure]\n", ColorRed);
			Console::WriteEx("\t->Rejected!\n", ColorRed);
			Console::WriteLine("");

		}
		else
		{
			Console::WriteLine("\t->Checking:   [ok]\n");
			sprintf_s(str1, "\t->Accepted[%d/%d]\n", sol - notaccepted, sol);
			Console::WriteEx(str1, ColorGreen);


			val = (double)profOI.compactIdx + (double)profOI.orphanedIdx;
			a_percent = (double)(100 * (1 - (val / engine.solution().stats().shifts)));
			printf("\t->Optimized at %.2f%% P:%d/%d C:%d/%d\n", a_percent, profOI.compactIdx, profOI.orphanedIdx, classeOI.compactIdx, classeOI.orphanedIdx);

			if (a_percent > max_percent) {
				sol_retenu = sol;
				max_percent = a_percent;
				oldval = val;

				getTimeStr(str2);
				sprintf_s(str1, "sol-v%d-%s.gedt", engine.pluginInfo().version_maj, str2);
				engine.save(str1);
				printf("\t->Autosaved [%s]\n", str1);
				MessageBeep(0xFFFFFFFF);// MB_ICONASTERISK MB_ICONEXCLAMATION MB_ICONOK
				if (val == 0)
					break;

			}
		}
		sol++;
	}//while

	engine.release();

	//end
#if USE_LIB
	freeLibrary(hEngineLib, engine);
#endif
}

