
#include "../3rdparty/Console/console.h"
#include "classes.h"
#include "engine.h"
#include "utils.h"

#include <assert.h>

ScheduleEngine::~ScheduleEngine()
{
	//dtor
}
bool ScheduleEngine::release()
{
	delete this;
	return true;
}

ScheduleEngine::ScheduleEngine() : _solution(), _pluginInfo(), _engineConfig(),_solutionBuilt(false) {
	//  pluginInfo.description="\0";
	//  pluginInfo.name=&ENGINENAME;
	_pluginInfo.version_maj = 1;
	_pluginInfo.version_min = 0;
	_engineConfig.slowprinting = true;
	_engineConfig.optimizebranching = false;
}

bool ScheduleEngine::load(CEngineConfig cfg) {
	try
	{
		_engineConfig = cfg;
		_solution.load(_engineConfig.configfile);

		const auto& st = _solution.stats();
		_matMapDT = new CMapDayTable[st.classes];

		_matProcessLevel.reserve(st.mats);

		_mapClassesBitDT_A = new CBitDayTable[st.classes];
		_mapCroomBitDT_A = new CBitDayTable[st.crooms];
		_mapProfBitDT_A = new CBitDayTable[st.profs];

		_mapClassesBitDT_B = new CBitDayTable[st.classes];
		_mapCroomBitDT_B = new CBitDayTable[st.crooms];
		_mapProfBitDT_B = new CBitDayTable[st.profs];
		FTypeCroomMapArray = new  CDayTable[MAX_CROOMTYPE_COUNT];

#if  OPTIMIZE_BRANCHING ==1
		FmhtLength_b = new CMapHourTable[9];
		FmhtLength_e = new CMapHourTable[9];

		FmdtMat_b = new CMapDayHoursTable[st.mats];
		FmdtMat_e = new CMapDayHoursTable[st.mats];
#endif
	}
	catch (...)
	{

		Console::WriteEx("\nError loading engine!", ColorRed);
		return false;
	}
	return true;
}

bool ScheduleEngine::save(const char* fn) {

	return _solution.save(fn);

}


bool  ScheduleEngine::checkSolution(const bool b) {
	return _solution.checkProcessedShifts();
}

void ScheduleEngine::execute(bool bfirst, bool bnew)
{
	_processStats.max_shifts = 0;
	_profs_bh.high = 0;
	_classes_bh.high = 0;
	time(&ptime);
	if (bfirst) {

		stime = ptime;
	}


	_processStats.highlevel_count = ((int)_engineConfig.hltx * _solution.stats().shifts / 100);
	_processStats.lowlevel_count = ((int)_engineConfig.lltx * _solution.stats().shifts / 100);
	printf("\nhltx:%2.2f%% highLevel:%d/%d\nlltx:%2.2f%%, LowLevel %d/%d\nmltx:%2.2f%%\n\n",
		_engineConfig.hltx, _processStats.highlevel_count, _solution.stats().shifts,
		_engineConfig.lltx, _processStats.lowlevel_count, _solution.stats().shifts,
		_engineConfig.mltx

	);

	for (int i = 0; i < _solution.stats().mats; i++)
	{
		int sc = _solution.getShiftsCount(i, emMat);
		ScheduleEngine::ProcessLevel pv(
			sc,
			(int)(_engineConfig.mltx * sc / 100),
			_engineConfig.mltx,
			0
		);
		_matProcessLevel.emplace_back(pv);

#if ENGINE_DEBUGMODE_LEVEL >1
		printf("mindex=%d   count=%d    level=%d\n",
			i,
			_matProcessLevel.back().shiftCount,
			_matProcessLevel.back().level);
#endif
	}

	initialize(bfirst || bnew);
	cursor_y = Console::GetCursorY();
	startSearching();
	//TODO Optimize;

}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ScheduleEngine::initProcesses()
{

	_dProcesses.clear();
	//int k=0;
	for (int j = 0; j < _solution.stats().shifts; j++)
	{


		//_dProcesses[k]=j ;
		_dProcesses.push_back(j);
		//fix rank
		_solution.Shift(j)->rank = j;
		//reset delta
		_solution.Shift(j)->delta = 8 - _solution.Shift(j)->length;

	}

	if (checkProcesses() == false) {
		perror("\nEngine initialize error: bad check!");
		exit(EXIT_FAILURE_BAD_CHECK);
	}
	//  if (goRandomizeShifts in _solution.GlobalOptions)
	doRandomize();
	_processChecksum = processesCheckSum();
}

bool ScheduleEngine::checkProcesses() const
{

	for (int p = 0; p < _dProcesses.size(); p++)
		for (int j = 0; j < _dProcesses.size(); j++)
			if ((j != p) && (_dProcesses[p] == _dProcesses[j]))
			{
				printf("   ->checking _dProcesses: duplication error _dProcesses[%d]=_dProcesses[%d]=%d", p, j, _dProcesses[p]);
				return false;
			}
	return true;
}

unsigned long ScheduleEngine::processesCheckSum()
{

	unsigned long sum = _adler32((unsigned char*)&(_dProcesses[0]), _dProcesses.size());
	//sum =sum^0xFFFFFFFF;
	// printf("\n checksum=%ld\n\n",sum);
	return unsigned(sum);

}

bool ScheduleEngine::moveIndex(int i1, int i2)
{

	int& sc = _solution.stats().shifts;
	if ((i2 >= i1) || (i1 < 1) || (i2 < -1) || (i1 >= sc) || (i2 >= sc))
		return false;
	int pi1 = _dProcesses[i1];//save i1
	_solution.Shift(pi1)->delta++;
	int tmpI2 = i2;

	if (tmpI2 == -1)
		for (int j = 0; j < i1; j++)
			if (_solution.Shift(pi1)->delta > _solution.Shift(_dProcesses[j])->delta)
			{
				tmpI2 = j;
				break;
			}

	//translation
	if (tmpI2 >= 0) {
		for (int i = i1; i > tmpI2; i--)
			_dProcesses[i] = _dProcesses[i - 1];
		_dProcesses[tmpI2] = pi1; // fill i2 with i1
		return true;
	}
	return false;
}

void ScheduleEngine::swapValues(const int v, const int w)
{
	int tmp = _dProcesses[w];
	_dProcesses[w] = _dProcesses[v];
	_dProcesses[v] = tmp;

}

int ScheduleEngine::doPartition(const int p, const int r)
{

	//random pivot or not!!
	//int x=_dProcesses[rand() % _dProcesses.size()];
	int x = _dProcesses[r];

	int i = p - 1;

	for (int j = p; j < r; j++)
	{
		if (_solution.Shift(_dProcesses[j])->delta <= _solution.Shift(x)->delta)
		{
			i++;
			swapValues(i, j);
		}
	}
	swapValues(i + 1, r);
	return i + 1;

}

void ScheduleEngine::doQuickSort(const bool ascendant, const int p, const int r)
{

	if (p < r)
	{
		int part = doPartition(p, r);//pivot
		doQuickSort(ascendant, p, part - 1);
		doQuickSort(ascendant, part + 1, r);
	}

}

void ScheduleEngine::doSort(const bool ascendant)
{

	doQuickSort(ascendant, 0, _dProcesses.size() - 1);

#if ENGINE_DEBUGMODE_LEVEL >1
	int i;
	for (i = 0; i < _dProcesses.size(); i++)
	{
		printf("%d   [%d]     rank=%d     length=%d   _processInfo.every2weeks=%d  delta=%d\n",
			i,
			_dProcesses[i],
			_solution.Shift(_dProcesses[i])->rank,
			_solution.Shift(_dProcesses[i])->length,
			_solution.Shift(_dProcesses[i])->every2weeks,
			_solution.Shift(_dProcesses[i])->delta
		);
	}
#else

#endif


}

void ScheduleEngine::doRandomize()
{

	for (int j = 0; j < _dProcesses.size(); j++)
	{
		srand(time(0));
		swapValues(j, rand() % _dProcesses.size());
		// moveIndex(j,rand()% _dProcesses.size());

	}
	//printf("\nnew checksum: %ul", processesCheckSum());
}

#if  OPTIMIZE_BRANCHING
void ScheduleEngine::doFillConstraintMap() {
	int j, d, h;

	for (d = 0; d < WORKABLE_DAY_COUNT; d++)
		for (h = 0; h < HOUR_TICK_COUNT; h++)
		{
			FmhtLength_b[0][d][h] = 0;
			FmhtLength_b[0][d][h] = 0;
			FmhtLength_b[1][d][h] = 0;
			FmhtLength_b[1][d][h] = 0;

			for (j = 2; j < 9; j++) {
				FmhtLength_b[j][d][h] = 1;
				FmhtLength_e[j][d][h] = 1;

				if ((j != 3) && (h % 2 != 0))
					FmhtLength_b[j][d][h] = 0;

				if ((d > 3) && (h >= 8)) {
					FmhtLength_b[j][d][h] = 0;
					FmhtLength_e[j][d][h] = 0;
				}

			}


			/*
				   FmhtLength_b[2][d][h]=((h % 2==0)&&( (h<=6)||((h>=8)&&(h<=13)) ));
				   FmhtLength_e[2][d][h]=((h % 2==0)&&( ((h>=2)&&(h<=8))||((h>=10)&&(h<HOUR_TICK_COUNT)) ));
			*/
			FmhtLength_b[3][d][h] = (h % 4 == 0);
			FmhtLength_b[4][d][h] = (h % 4 == 0);
			FmhtLength_b[6][d][h] = (h == 0) || (h == 2) || (h == 8) || (h == 10);
			FmhtLength_b[8][d][h] = (h % 8 == 0);

			FmhtLength_b[5][d][h] = 0;
			FmhtLength_e[5][d][h] = 0;

		}

	for (d = 0; d < WORKABLE_DAY_COUNT; d++)
		for (h = 0; h < HOUR_TICK_COUNT; h++)
		{

			for (j = 0; j < _solution.stats().mats; j++)
			{
				FmdtMat_b[j][d][h] = 0;
				FmdtMat_e[j][d][h] = 0;
			}

			if (h < 6) {
				FmdtMat_b[7][d][h] = _matProcessLevel[7].level;//HG >11h


			}
			if (h < 4) {

				FmdtMat_b[8][d][h] = _matProcessLevel[8].level;//
				FmdtMat_b[9][d][h] = _matProcessLevel[9].level;//
				FmdtMat_b[10][d][h] = _matProcessLevel[10].level;//

			}
			if (h > 8) {
				FmdtMat_e[0][d][h] = _matProcessLevel[0].level;//math <12h
				if (d == 0)
					FmdtMat_e[3][d][h] = _matProcessLevel[3].level;//arabe lundi <12h
				if (d <= 1)
					FmdtMat_e[4][d][h] = _matProcessLevel[4].level;//francais lundi,mardi <12h
				if (d == 3)
					FmdtMat_e[5][d][h] = _matProcessLevel[5].level;//
			}

			if (h > 12) {
				FmdtMat_e[0][d][h] = _solution.stats().shifts;
				FmdtMat_e[13][d][h] = _solution.stats().shifts; //ph <14h 100%
			}

			if (h > 14) {
				FmdtMat_e[6][d][h] = MAX_SHIFTS_COUNT;//sport <17h
			}

		}

}
#endif

#if  USE_BACKTRACKING
void ScheduleEngine::doBackTrack(const int iprocess) {
	assert(iprocess > 0 && iprocess < _dProcesses.size() - 1);

	//clear last processed shift
	int lastProcessed = _dProcesses[iprocess - 1];
	clearShift(lastProcessed);
	swapValues(iprocess, iprocess - 1);
	_processInfo.lastBacktracked = iprocess;

	printf("\rBT[%d]", iprocess);
	//move to previous timeslot index
	_processInfo.prevSlotIndex();
}
#endif

void ScheduleEngine::initialize(const bool AReset)
{
	if (AReset)
	{
		
		initProcesses();
		_processStats.percent = 0;
		//fill croom type map
		for (int i = 0; i < MAX_CROOMTYPE_COUNT; i++) {
			int c = _solution.CroomCountByType(i);
			for (int j = 0; j < MAX_DAY_COUNT; j++)
				for (int k = 0; k < HOUR_TICK_COUNT; k++)
					FTypeCroomMapArray[i][j][k] = c;
		}
#if  OPTIMIZE_BRANCHING ==1
		doFillConstraintMap();
#endif


	}
	//try to check if we are in bad processing path..
	if (
		((_processStats.speed >= 1000) && (_processStats.tries_count > (50000 - (int)_processStats.speed * 10)))
		|| ((_processStats.tries_count - _processStats.last_progession) > (1000 + (int)_processStats.percent * 500))
		)
	{

		Console::WriteEx("\n\tbad search path, reprocessing..", ColorRed);
		cursor_y++;
		Sleep(1000);
		/*if(_processStats.percent<99.0 ){
		doRandomize();
		}
		else {
		 doSort(true);
		}*/
		doRandomize();
		_processStats.tries_count = 0;
		_processStats.last_progession = _processStats.tries_count;
		_processStats.max_shifts = 0;
		_profs_bh.high = 0;
		_classes_bh.high = 0;
		time(&ptime);
		_processChecksum = processesCheckSum();
	}



	//reset mat DT bitset maps
	for (int i = 0; i < _solution.stats().classes; i++)
		for (int j = 0; j < _solution.stats().mats; j++)
			for (int k = 0; k < WORKABLE_DAY_COUNT; k++) {
				_matMapDT[i][j][k] = 0;
			}
	
	//clear all shifts, skip DT tables
	for (int i = 0; i < _solution.stats().shifts; i++)
			_solution.clearShift(_dProcesses[i],  false);
	
	//reset all DT bitset maps
	for (int k = 0; k < MAX_DAY_COUNT; k++)
	{
		for (int i = 0; i < _solution.stats().classes; i++)
		{
			_mapClassesBitDT_A[i][k] = 0;
			_mapClassesBitDT_B[i][k] = 0;
		}

		for (int i = 0; i < _solution.stats().profs; i++)
		{
			_mapProfBitDT_A[i][k] = 0;
			_mapProfBitDT_B[i][k] = 0;

		}

		for (int i = 0; i < _solution.stats().crooms; i++)
		{
			_mapCroomBitDT_A[i][k] = 0;
			_mapCroomBitDT_B[i][k] = 0;
		}

	}


	//reset mat process levels
	for (int i = 0; i < _solution.stats().mats; i++)
	{
		_matProcessLevel[i].iprocess = 0;

	}

	_profs_bh.count = 0;
	_classes_bh.count = 0;
	_processInfo.timeSlot.day = -1;
	_processInfo.timeSlot.start = -1;
	//always begin from index 0!!
	_processInfo.timeSlot.index = 0;
	_solutionBuilt = false;
	_processInfo.lastBacktracked = -1;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */

bool ScheduleEngine::startSearching()
{

	_processStats.running = true;
	_processStats.tries_count = 0;
	_processStats.last_progession = 0;

	_processInfo.resetSlot();
	while (_processInfo.timeSlot.index < _dProcesses.size())
	{

#if ENGINE_DEBUGMODE_LEVEL >0
		printf("\rslot(%d,%d) -> shift[%d]", _processInfo.timeSlot.index, _processInfo.timeSlot.start, _dProcesses[_processInfo.timeSlot.index]);
#endif
		if (process(_processInfo.timeSlot.index))
		{
			//if processed move to next shift
#if ENGINE_DEBUGMODE_LEVEL >0
			printf("\t%d processed", _processInfo.timeSlot.index);
#endif
			_processInfo.nextSlotIndex();

		}
		else //not processed
		{
			//process next hour
			_processInfo.nextSlotStart();
			//if reach last hour
			if (_processInfo.timeSlot.start >= HOUR_TICK_COUNT)
			{
#if USE_BACKTRACKING
				if (_processInfo.timeSlot.index < _dProcesses.size() - 1 && _processInfo.timeSlot.index > _processInfo.lastBacktracked) {
					doBackTrack(_processInfo.timeSlot.index);
				}
				else {
					retry();
				}
#else
				//no backtracking alaways retry!
				retry();
#endif
			}
		} //end not processed


	}  //while
   //if all processed rebuild solution to fill weeka and weekb maps for every classe,prof and croom
	if (_processInfo.timeSlot.index == _dProcesses.size())
	{
		//printf("\nrebuildSolution()..");
		_solution.rebuildSolution();
		_solutionBuilt = true;
		return true;
	}
	return false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool ScheduleEngine::process(const int iprocess)
{
#if ENGINE_DEBUGMODE_LEVEL>4
	printf("\nentering process()..");
#endif
	bool result = false;

	_processInfo.timeSlot.groupWith = -1;
	_processInfo.fillMode = EFillMode::foMixte;
	_processInfo.sindex = _dProcesses[iprocess];

	_processInfo.pShift = _solution.Shift(_processInfo.sindex);
	_processInfo.fday = _solution.Prof(_processInfo.pShift->pindex)->fday;

	if (_processInfo.pShift->pindex == -1)
	{
		printf("FATAL ERROR: No 'PROFESSOR' assigned for 'SHIFT' n: %d", _processInfo.sindex);
		exit(EXIT_FAILURE_BAD_DATA);
	}


	_processInfo.timeSlot.day = _processInfo.timeSlot.startDay - 1;
	nextDay();
	_processInfo.bitset = getBitset(_processInfo.pShift->length, _processInfo.timeSlot.start);

	//loop over all crooms
	_processInfo.resetCroom();
	while (_processInfo.croom < _solution.stats().crooms)
	{
		if (
			(!_processInfo.isForbidden)
			&& (_processInfo.pShift->croom_type == _solution.Croom(_processInfo.croom)->stype)
			&& (checkIsEmpty() != -1)
			)
		{

			if (result = fillShift())
				break;
			else {
				exit(EXIT_FAILURE_BAD_FILL_OP);
			}



		}

		_processInfo.nextCroom();
		if (_processInfo.isForbidden || (_processInfo.croom >= _solution.stats().crooms))
		{
			_processInfo.resetCroom();
			nextDay();
			//if reach startDay again then break and return false 
			if (_processInfo.timeSlot.day == _processInfo.timeSlot.startDay)
				break;

		}


	}  //end  while
#if ENGINE_DEBUGMODE_LEVEL >2
	printf("    ->[%d] process()  result=[%d]\n", _processInfo.sindex, result);
#else

#endif


	return result;

}

void ScheduleEngine::retry() {
	SYSTEMTIME st0, st1;
	double diff_time;

	if (!moveIndex(_processInfo.timeSlot.index, -1/*max(0,2*i-_dShifts.size())*/)) {
#if ENGINE_DEBUGMODE_LEVEL>0
		printf("\n moveIndex failure: %d", _processInfo.timeSlot.index);
#endif
	}
	_processStats.last_max_shifts = _processStats.max_shifts;
	_processStats.max_shifts = (_processInfo.timeSlot.index >= _processStats.max_shifts) ? _processInfo.timeSlot.index : _processStats.max_shifts;
	if (_processStats.max_shifts > _processStats.last_max_shifts)_processStats.last_progession = _processStats.tries_count;



	_processStats.tries_count++;
#if SLOW_PRINTING
	if (_processStats.tries_count % 201 == 0) {
#endif

		diff_time = difftime(time(0), ptime);
		_profs_bh.high = (_profs_bh.high > _profs_bh.count) ? _profs_bh.high : _profs_bh.count;
		_classes_bh.high = (_classes_bh.high > _classes_bh.count) ? _classes_bh.high : _classes_bh.count;

		//progression_tx=_processStats.percent;
		_processStats.percent = (double)_processStats.max_shifts / (double)_solution.stats().shifts * 100;

		time_t2SystemTime(time(0) - ptime, &st0);
		time_t2SystemTime(time(0) - stime, &st1);

		_processStats.speed = (double)(_processStats.tries_count / diff_time);

		Console::SetCursorPosition(7, cursor_y);
		printf("#%06d #%03d %c %02.3f%% %d/%d %d/s  BH (p:%d , c:%d)",
			_processStats.tries_count + 1,
			_processStats.tries_count - _processStats.last_progession,
			SPEED_SYM[_processStats.tries_count % 4],

			_processStats.percent,
			_processStats.max_shifts,
			solution().stats().shifts,
			_processStats.speed,
			_profs_bh.count,
			_classes_bh.count


		);
		//printf(" crc%012ld", _processChecksum);
		printf(" %02d:%02d:%02d/%02d:%02d:%02d",
			st0.wHour, st0.wMinute, st0.wSecond,
			st1.wHour, st1.wMinute, st1.wSecond);

#if SLOW_PRINTING ==1
	}
#endif

	initialize(false);

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool ScheduleEngine::fillShift()
{
	bool result = false;

	//if we have to setLink 
	if (_processInfo.timeSlot.groupWith != -1)
	{
		_solution.setLink(_processInfo.timeSlot.groupWith, _processInfo.sindex, ltGroup);
	}
	_processInfo.pShift->crindex = _processInfo.croom;
	_processInfo.pShift->hour = _processInfo.timeSlot.start;
	_processInfo.pShift->day = _processInfo.timeSlot.day;
	_processInfo.pShift->fillMode = (int)(_processInfo.fillMode);

	if ((_processInfo.fillMode == foMixte) || (_processInfo.fillMode == foWeekA))
	{
		if (_processInfo.timeSlot.groupWith == -1)
			_mapClassesBitDT_A[_processInfo.pShift->cindex][_processInfo.timeSlot.day] |= _processInfo.bitset;
		_mapCroomBitDT_A[_processInfo.croom][_processInfo.timeSlot.day] |= _processInfo.bitset;
		_mapProfBitDT_A[_processInfo.pShift->pindex][_processInfo.timeSlot.day] |= _processInfo.bitset;

	}
	if ((_processInfo.fillMode == foMixte) || (_processInfo.fillMode == foWeekB))
	{
		if (_processInfo.timeSlot.groupWith == -1)
			_mapClassesBitDT_B[_processInfo.pShift->cindex][_processInfo.timeSlot.day] |= _processInfo.bitset;
		_mapCroomBitDT_B[_processInfo.croom][_processInfo.timeSlot.day] |= _processInfo.bitset;
		_mapProfBitDT_B[_processInfo.pShift->pindex][_processInfo.timeSlot.day] |= _processInfo.bitset;
	}

	//update _matMapDT
	_matMapDT[_processInfo.pShift->cindex][_processInfo.pShift->mindex][_processInfo.timeSlot.day] = 1;
	_matProcessLevel[_processInfo.pShift->mindex].iprocess++;
	FTypeCroomMapArray[_processInfo.pShift->croom_type][_processInfo.timeSlot.day][_processInfo.timeSlot.start]--;
	/* if (_after_break_hour==true)
		 _profs_bh.count++;
	 if((_profs_bh.count>0)&&(_processInfo.timeSlot.end!=8)&&(_processInfo.timeSlot.end!=HOUR_TICK_COUNT)&&(_solution.Prof(_processInfo.pShift->pindex).weeka[_processInfo.timeSlot.day][_processInfo.timeSlot.end]!=-1))
		 _profs_bh.count--;

	//dec delta
	_processInfo.pShift->delta--;
	*/
	result = true;

#if ENGINE_DEBUGMODE_LEVEL >2
	printf("\t->[%d] fillShift() n:%d ....[ok]\n", _processInfo.sindex, _processInfo.croom);
#else

#endif
	return result;

}

bool ScheduleEngine::clearShift(const int sh) {

	CShift* psh = _solution.Shift(sh);
	
	//get shift bitset
	DWORD bitset = getBitset(psh->length, psh->hour);
	//we have to reverse bits in order to undo : if b = b|a , then to undo: b = b & ~a
	bitset = ~bitset;
	if ((psh->fillMode == foMixte) || (psh->fillMode == foWeekA))
	{
		if (psh->groupedwith == -1)
			_mapClassesBitDT_A[psh->cindex][psh->day] &= bitset;
		_mapCroomBitDT_A[psh->crindex][psh->day] &= bitset;
		_mapProfBitDT_A[psh->pindex][psh->day] &= bitset;

	}
	if ((psh->fillMode == foMixte) || (psh->fillMode == foWeekB))
	{
		if (psh->groupedwith == -1)
			_mapClassesBitDT_B[psh->cindex][psh->day] &= bitset;
		_mapCroomBitDT_B[psh->crindex][psh->day] &= bitset;
		_mapProfBitDT_B[psh->pindex][psh->day] &= bitset;
	}

	//update _matMapDT
	_matMapDT[psh->cindex][psh->mindex][psh->day] = 0;
	_matProcessLevel[psh->mindex].iprocess--;
	FTypeCroomMapArray[psh->croom_type][psh->day][psh->hour]++;

	psh->crindex = -1;
	psh->hour = -1;
	psh->day = -1;
	psh->fillMode = (int)(foNoWhere);

	if (psh->groupedwith != -1)
	{
		_solution.setLink(sh, psh->groupedwith, ltClear);

	}
	return true;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
int ScheduleEngine::checkIsEmpty()
{
#if ENGINE_DEBUGMODE_LEVEL>4
	printf("\nentering checkIsEmpty()... _dProcesses[%d] =%d", _processInfo.sindex, _dProcesses[_processInfo.sindex]);
#endif
	int iresultA = -1;
	int iresult = -1;
	int class_shift = -1;
	_processInfo.timeSlot.groupWith = -1;

	_processInfo.every2w = _processInfo.pShift->every2weeks;

	_processInfo.fillMode = foWeekA;

	CroomBitDT = &_mapCroomBitDT_A[_processInfo.croom];
	ProfBitDT = &_mapProfBitDT_A[_processInfo.pShift->pindex];
	ClasseBitDT = &_mapClassesBitDT_A[_processInfo.pShift->cindex];
	class_shift = _solution.Classe(_processInfo.pShift->cindex)->weeka[_processInfo.timeSlot.day][_processInfo.timeSlot.start];//weeka class  shift
	_processInfo.canBeGrouped = class_shift == -1 ? false : _solution.canAlternate(_processInfo.sindex, class_shift, _processInfo.timeSlot.day, _processInfo.timeSlot.start);
	iresult = checkEmptyDT();
	if (((iresult == -1) && _processInfo.every2w) || ((iresult != -1) && !_processInfo.every2w))
	{
		if (_processInfo.every2w)
			_processInfo.fillMode = foWeekB;
		else
			_processInfo.fillMode = foMixte;

		CroomBitDT = &_mapCroomBitDT_B[_processInfo.croom];
		ProfBitDT = &_mapProfBitDT_B[_processInfo.pShift->pindex];
		ClasseBitDT = &_mapClassesBitDT_B[_processInfo.pShift->cindex];
		class_shift = _solution.Classe(_processInfo.pShift->cindex)->weekb[_processInfo.timeSlot.day][_processInfo.timeSlot.start];//weekb class  shift
		_processInfo.canBeGrouped = class_shift == -1 ? false : _solution.canAlternate(_processInfo.sindex, class_shift, _processInfo.timeSlot.day, _processInfo.timeSlot.start);
		iresultA = iresult;
		iresult = checkEmptyDT();
		if (!_processInfo.every2w && (iresult != iresultA))
			iresult = -1;
	}


	if ((iresult != -1) && (class_shift != -1) && _processInfo.canBeGrouped)
		_processInfo.timeSlot.groupWith = class_shift;
#if ENGINE_DEBUGMODE_LEVEL >3
	printf("\t->[%d] checkIsEmpty  result=[%d] _processInfo.timeSlot.start=[%d]\n", _processInfo.sindex, iresult, _processInfo.timeSlot.start);
#else

#endif

#if ENGINE_DEBUGMODE_LEVEL>4
	printf("\nleaving checkIsEmpty()..");
#endif
	return iresult;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
inline int ScheduleEngine::checkEmptyDT() const
{
#if ENGINE_DEBUGMODE_LEVEL>4
	printf("\nentering checkEmptyDT()... _dProcesses[%d] =%d", _processInfo.sindex, _dProcesses[_processInfo.sindex]);
#endif
	if (
		((((*ClasseBitDT)[_processInfo.timeSlot.day] & _processInfo.bitset) != 0) && !_processInfo.canBeGrouped)
		|| (((*CroomBitDT)[_processInfo.timeSlot.day] & _processInfo.bitset) != 0)
		|| (((*ProfBitDT)[_processInfo.timeSlot.day] & _processInfo.bitset) != 0)
		)
		return -1;


	return _processInfo.timeSlot.start;

}



/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ScheduleEngine::nextDay()
{
	_processInfo.nextDay();
	// _after_break_hour=(_processInfo.timeSlot.start>0) && (_processInfo.timeSlot.start!=8) && (_solution.ProfTable()[curpindex].weeka[_processInfo.timeSlot.day][_processInfo.timeSlot.start-1]==-1);
	_processInfo.isForbidden =
		(_processInfo.timeSlot.day == _processInfo.fday)
		|| (_matMapDT[_processInfo.pShift->cindex][_processInfo.pShift->mindex][_processInfo.timeSlot.day] == 1)//every classe must have one mat once a day
		|| (_processInfo.timeSlot.end > HOUR_TICK_COUNT) //evening
		|| (_processInfo.timeSlot.start < 8) && (_processInfo.timeSlot.end > 8)//no course across morning and evening shift

#if  OPTIMIZE_BRANCHING
		|| (FmhtLength_b[_processInfo.length][_processInfo.timeSlot.day][_processInfo.timeSlot.start] == false)
		|| (FmhtLength_b[_processInfo.length][_processInfo.timeSlot.day][_processInfo.timeSlot.end - 1] == false)
		|| (_matProcessLevel[_processInfo.mindex]._processInfo.timeSlot.index < FmdtMat_b[_processInfo.mindex][_processInfo.timeSlot.day][_processInfo.timeSlot.start])
		|| (_matProcessLevel[_processInfo.mindex]._processInfo.timeSlot.index < FmdtMat_e[_processInfo.mindex][_processInfo.timeSlot.day][_processInfo.timeSlot.end - 1])

#else
		|| (((_processInfo.pShift->length == 4) || (_processInfo.pShift->length == 3)) && (_processInfo.timeSlot.start % 4 != 0))   //for 2h/1.5h length shifts
		|| ((_processInfo.pShift->length == 8) && (_processInfo.timeSlot.start % 8 != 0))                                     //.... 4h 4h
		|| ((_processInfo.timeSlot.start % 2 != 0) && (_processInfo.pShift->length != 3))                                 // begin with half hour
		|| ((_processInfo.timeSlot.day == 4) && (_processInfo.timeSlot.start >= 8))                                                 //no course for friday & saturday evening
		|| ((_processInfo.timeSlot.day == 5) && (_processInfo.timeSlot.start >= 8))

#endif      
		//no maths after 14h
		|| ((_processInfo.pShift->mindex == 0) && (_matProcessLevel[0].iprocess < _matProcessLevel[0].level) && (_processInfo.timeSlot.end > 8))

		//no physics after 14h
		|| ((_processInfo.pShift->mindex == 13) && (_matProcessLevel[13].iprocess < _matProcessLevel[13].level) && (_processInfo.timeSlot.end > 12))

		//no sport after 17h
		|| ((_processInfo.pShift->mindex == 6) && (_processInfo.timeSlot.end > 14))

		// busy hours
		|| ((_processInfo.timeSlot.index <= _processStats.lowlevel_count) && (_processInfo.pShift->length == 2) && (_processInfo.timeSlot.end > 14))
		// break hour
		|| ((_processInfo.timeSlot.index <= _processStats.lowlevel_count) && ((_processInfo.timeSlot.start > 0) && (_solution.ClasseTable()[_processInfo.pShift->cindex].weeka[_processInfo.timeSlot.day][_processInfo.timeSlot.start - 1] == -1)))
		|| ((_profs_bh.count > 10) && _after_break_hour)



		;

}
