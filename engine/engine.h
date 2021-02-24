#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "solution.h"
#include "I_engine.h"

#ifndef EMENGINE_H
#define EMENGINE_H


#define ENGINENAME "ENGINE1-alpha31"
#define ENGINE_DEBUGMODE_LEVEL 0

#define ENGINE_ERROR_LOADING 500L
#define EXIT_FAILURE_BAD_DATA 1001L
#define EXIT_FAILURE_BAD_FILL_OP 1061L
#define EXIT_FAILURE_BAD_CHECK 1071L


#define OPTIMIZE_BRANCHING  0
#define USE_BACKTRACKING    0
#define SLOW_PRINTING       1


const char SPEED_SYM[4] = { '|','/','-','\\' };

class ScheduleEngine :public I_Engine_Intf
{
public:

	ScheduleEngine();
	virtual ~ScheduleEngine();
	void execute(bool bfirst, bool bnew);

	bool release();
	bool checkSolution(const bool b);

	bool load(CEngineConfig cfg);
	bool save(const char* fn);

	CEnginePlugin& pluginInfo() { return _pluginInfo; }
	ScheduleSolution& solution() { return _solution; }
	CEngineConfig& config() { return _engineConfig; }

protected:

	void nextDay();
	
	void initProcesses();
	void doRandomize();
	void doSort(const bool ascendant);
	void doQuickSort(const bool ascendant, const int p, const int r);
	int doPartition(const int p, const int r);
	unsigned long processesCheckSum();
	bool checkProcesses() const;
	void swapValues(const int v, const int w);
	bool moveIndex(int i1, int i2);
	int checkEmptyDT() const;
	int checkIsEmpty();

	bool fillShift();
	bool clearShift(const int sh);
	void initialize(const bool AReset);
	bool process(const int iprocess);
	void retry();
	bool startSearching();
	DWORD getBitset(const int len, const int start) {
		DWORD bitset = (1 << len) - 1;
		bitset = bitset << (32 - len - start);
		//printf("\nbitset=%d", bitset);
		return bitset;

	}
#if  OPTIMIZE_BRANCHING 
	void doFillConstraintMap();
#endif   
#if  USE_BACKTRACKING
	void doBackTrack(const int iprocess);
#endif  
private:
	CEngineConfig  _engineConfig;
	ScheduleSolution _solution;
	CEnginePlugin _pluginInfo;
	CMapDayTable* _matMapDT;

	CBitDayTable* _mapClassesBitDT_A, * _mapCroomBitDT_A, * _mapProfBitDT_A,
		* _mapClassesBitDT_B, * _mapCroomBitDT_B, * _mapProfBitDT_B;
	CMapDayHoursTable* FmdtMat_b, * FmdtProf_b, * FmdtClasse_b, * FmdtCroom_b;
	CMapDayHoursTable* FmdtMat_e, * FmdtProf_e, * FmdtClasse_e, * FmdtCroom_e;
	CMapHourTable* FmhtLength_b, * FmhtLength_e;
	CDayTable* FTypeCroomMapArray;
	bool _solutionBuilt;
	struct ProcessLevel {
		int shiftCount;
		int level;
		double tx;
		int iprocess;
		ProcessLevel(int sh, int lv, int tx, int i) :shiftCount(sh), level(lv), tx(tx), iprocess(i) {}
		ProcessLevel() :shiftCount(0), level(-1), tx(0), iprocess(0) {}
	};
	std::vector<ProcessLevel> _matProcessLevel;

	unsigned long _processChecksum;


	std::vector <int> _dProcesses;


	CBitDayTable* ProfBitDT, * CroomBitDT, * ClasseBitDT;


	struct ProcessingInfo {
		int sindex,croom, fday;
		bool canBeGrouped, every2w;
		DWORD bitset;
		EFillMode fillMode;
		bool isForbidden;
		CShift* pShift;
		int lastBacktracked;
		struct TimeSlot {
			int index /*index*/, startDay, day, start /*hour*/, end /*hour*/;
			int groupWith;
			TimeSlot() :index(0),  startDay(-1), day(-1), start(0), end(0), groupWith(-1) {};
		};
		TimeSlot timeSlot;
		ProcessingInfo() : pShift(NULL), sindex(-1), fday(-1),
			canBeGrouped(false), every2w(false),
			bitset(0), fillMode(EFillMode::foNoWhere), isForbidden(true), lastBacktracked(-1)
		{};
		void nextDay() {
			timeSlot.day = ++timeSlot.day % WORKABLE_DAY_COUNT;
			timeSlot.end = timeSlot.start + pShift->length;
		}
		void nextCroom() {
			croom++;
		}
		void resetCroom() {
			croom = 0;
		}
		void prevSlotIndex() {
			timeSlot.index--;
			timeSlot.start = 0;
		}
		void nextSlotIndex() {
			timeSlot.index++;
			timeSlot.start = 0;
		}
		void nextSlotStart() {
			timeSlot.start++;
		}
		void resetSlot() {
			timeSlot.index = 0;
			timeSlot.start = 0;
			srand(time(0));
			timeSlot.startDay = rand() % 5;
			//timeSlot.day = timeSlot.startDay;
		}
	};
	ProcessingInfo _processInfo;


	time_t ptime, stime;

	int cursor_y;
	struct BreakHour
	{
		int count, high;
		BreakHour() : count(0), high(0) {}
	};
	BreakHour _classes_bh, _profs_bh;

	bool _after_break_hour;

	struct ProcessStats {
		bool running;
		double percent;
		int speed;
		int  max_shifts, last_max_shifts, highlevel_count, lowlevel_count, tries_count, last_progession;

		ProcessStats() :max_shifts(0), last_max_shifts(0), highlevel_count(0), lowlevel_count(0),
			tries_count(0), last_progession(0), percent(0.0), speed(0), running(false) {}
	};
	ProcessStats _processStats;

};



#endif // EMENGINE_H
