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

void test_load_Config() {

}
void test_1() {
	ScheduleEngine engine;
	CClasse cl1;
	CShift sh1;
	engine.solution().ClasseTable().push_back(cl1);
	engine.solution().ShiftTable().push_back(sh1);
	CCroom sa;
	//engine.solution().CroomTable()[0] = &sa;
}
int main(int argc, char* argv[], char* envp[])
{
	Console::initConsoleScreen();
	Console::HighVideo();
	Console::Clear();
	Console::SetTitle("Tests");
	Console::SetFGColor(ColorWhite);

	CEngineConfig engine_cfg;
	engine_cfg.slowprinting = true;
	engine_cfg.optimizebranching = false;

	


}