
#include <stdio.h>
#include <assert.h>

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
	ScheduleConfig pc = _scheduleConfig;

	try
	{
		sz = sizeof(ScheduleConfig);
		stream->write(&sz, sizeof(sz));
		stream->write(&pc, sz);

	}
	catch (...)
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
	catch (...)
	{
		//return NULL;
		perror("\nloadHeader error!");
	}
	return pc;

}

bool ScheduleSolution::writeData(CFileStream* stream, WORD phase)
{
	int sz, Le, i;
	CCroom* sa;
	CClasse* cl;
	CShift* se;
	CProf* pr;
	CMat* ma;
	char s[2] = "";
	//read string to compare to magic
	if (phase == 0)
		stream->writeString(_scheduleConfig.magic);


	switch (phase)
	{
	case 0:
		//write count
		Le = _stats.crooms;
		stream->write(&Le, sizeof(Le));
		//write struct
		sz = sizeof(CCroom);
		stream->write(&sz, sizeof(sz));

		for (i = 0; i < Le; i++)
		{

			stream->write(&_dCRooms[i], sz);

			//write magic separateur
			stream->write(s, 2);
		}

		break;
	case 1:
		//write count
		Le = _dClasses.size();
		stream->write(&Le, sizeof(Le));

		//write struct
		sz = sizeof(CClasse);
		stream->write(&sz, sizeof(sz));

		for (i = 0; i < Le; i++)
		{

			stream->write(&(_dClasses)[i], sz);
			//write magic separateur
			stream->write(s, 2);
		}

		break;
	case 2:
		//write count
		Le = _dShifts.size();
		stream->write(&Le, sizeof(Le));

		//write struct
		sz = sizeof(CShift);
		stream->write(&sz, sizeof(sz));

		for (i = 0; i < Le; i++)
		{

			stream->write(&_dShifts[i], sz);
			//write magic separateur
			stream->write(s, 2);
		}

		break;
	case 3:
		//write count
		Le = _stats.profs;
		stream->write(&Le, sizeof(Le));

		//write struct
		sz = sizeof(CProf);
		stream->write(&sz, sizeof(sz));
		for (i = 0; i < Le; i++)
		{

			stream->write(&_dProfs[i], sz);

			//write magic separateur
			stream->write(s, 2);
		}

		break;
	case 4:
		//write count
		Le = _stats.mats;
		stream->write(&Le, sizeof(Le));
		//write struct
		sz = sizeof(CMat);
		stream->write(&sz, sizeof(sz));
		for (i = 0; i < Le; i++)
		{

			stream->write(&_dMats[i], sz);

			//write magic separateur
			stream->write(s, 2);
		}

		break;

	}
	return true;
}

int ScheduleSolution::readData(CFileStream* stream, WORD phase)
{
	int sz, Le, i;

	CShift t;
	//read string then compare to magic
	if (phase == 0)
		printf("magic:%s\n", stream->readString());
	char s[2] = "";
	//read count
	stream->read(&Le, sizeof(Le));

	switch (phase)
	{
	case 0:
		//_dCRooms.reserve(Le);
		//read struct
		stream->read(&sz, sizeof(sz));
		CCroom sa;
		for (i = 0; i < Le; i++)
		{
			stream->read(&sa, sz);
			_dCRooms.emplace_back(sa);

			//read magic separateur
			stream->read(s, 2);
		}

		break;
	case 1:
		//_dClasses.reserve(Le);
		//read struct
		stream->read(&sz, sizeof(sz));
		CClasse cl;
		for (i = 0; i < Le; i++)
		{
			stream->read(&cl, sz);
			_dClasses.emplace_back(cl);

			//read magic separateur
			stream->read(s, 2);
		}

		break;

	case 2:
		//_dShifts.reserve(Le);
		//read struct
		stream->read(&sz, sizeof(sz));

		for (i = 0; i < Le; i++)
		{
			stream->read(&t, sz);
			CShift se = CShift(t);
			_dShifts.emplace_back(se);

			//read magic separateur
			stream->read(s, 2);
		}

		break;

	case 3:
		//_dProfs.reserve(Le);
		//read struct
		stream->read(&sz, sizeof(sz));
		CProf pr;
		for (i = 0; i < Le; i++)
		{
			stream->read(&pr, sz);
			_dProfs.emplace_back(pr);

			//read magic separateur
			stream->read(s, 2);
		}

		break;

	case 4:
		//_dMats.reserve(Le);
		//read struct
		stream->read(&sz, sizeof(sz));
		CMat ma;
		for (i = 0; i < Le; i++)
		{
			stream->read(&ma, sz);
			_dMats.emplace_back(ma);

			//read magic separateur
			stream->read(s, 2);
		}

		break;
	}

	return Le;

}

bool   ScheduleSolution::load(const char* filename)
{
	CFileStream fs(filename, cmOpen);
	_scheduleConfig = loadHeader(&fs);
	if (_scheduleConfig.magic_len != 255)
	{
		printf("error reading header!");
		return false;
	}

#if DEBUG_MODE
	printf("magic:[%s]\n", _scheduleConfig->magic);
	printf("version_lbl:[%s]\n", _scheduleConfig->version_lbl);
	printf("lbl_1:[%s]\n", _scheduleConfig->lbl_1);
	printf("lbl_2:[%s]\n", _scheduleConfig->lbl_2);
	printf("lbl_3:[%s]\n", _scheduleConfig->lbl_3);
	printf("lbl_4:[%s]\n", _scheduleConfig->lbl_4);
	printf("lbl_year:[%s]\n", _scheduleConfig->lbl_annee);
#endif

	_stats.crooms = readData(&fs, 0);
	_stats.classes = readData(&fs, 1);
	_stats.shifts = readData(&fs, 2);
	_stats.profs = readData(&fs, 3);
	_stats.mats = readData(&fs, 4);

	return true;
}

bool ScheduleSolution::save(const char* filename)
{

	CFileStream fs(filename, cmCreate);

	if (saveHeader(&fs) != true)
	{
		printf("error writing header!");
		return false;
	}

#if ENGINE_DEBUGMODE_LEVEL >2
	printf("magic:[%s]\n", _scheduleConfig->magic);
	printf("version_lbl:[%s]\n", _scheduleConfig->version_lbl);
	printf("lbl_1:[%s]\n", _scheduleConfig->lbl_1);
	printf("lbl_2:[%s]\n", _scheduleConfig->lbl_2);
	printf("lbl_3:[%s]\n", _scheduleConfig->lbl_3);
	printf("lbl_4:[%s]\n", _scheduleConfig->lbl_4);
	printf("lbl_annee:[%s]\n", _scheduleConfig->lbl_annee);
#endif
	writeData(&fs, 0);
	writeData(&fs, 1);
	writeData(&fs, 2);
	writeData(&fs, 3);
	writeData(&fs, 4);

#if DEBUG_MODE   
	printf("nombre de croom:%d\n", _stats.crooms);
	printf("nombre de classe:%d\n", _dClasses.size());
	printf("nombre de shift:%d\n", _dShifts.size());
	printf("nombre de prof:%d\n", _stats.profs);
	printf("nombre de mat:%d\n", _stats.mats);

#endif

	return true;

}

int ScheduleSolution::getOrphinedShiftByDay(const int AIndex, const int ADay, const EScheduleMode emMode)
{
	int h, tmpA, tmpB;
	CDayTable* wa, * wb;
	int result = 0;
	switch (emMode)
	{
	case emClasse:
		wa = &(_dClasses[AIndex].weeka);
		wb = &(_dClasses[AIndex].weekb);
		break;

	case emProf:
		wa = &(_dProfs[AIndex].weeka);
		wb = &(_dProfs[AIndex].weekb);
		break;
	default:
		return -1;

	}

	tmpA = 0;
	tmpB = 0;
	//check 1
	for (h = 0; h < 8; h++)
	{
		tmpA = tmpA + ((*wa)[ADay][h] != -1);
		tmpB = tmpB + ((*wb)[ADay][h] != -1);
	}

	result = result + (((tmpA < 4) && (tmpA > 0)) || ((tmpB < 4) && (tmpB > 0)));

	tmpA = 0;
	tmpB = 0;
	for (h = 8; h < HOUR_TICK_COUNT; h++)
	{
		tmpA = tmpA + ((*wa)[ADay][h] != -1);
		tmpB = tmpB + ((*wb)[ADay][h] != -1);
	}
	result = result + (((tmpA < 4) && (tmpA > 0)) || ((tmpB < 4) && (tmpB > 0)));
	return result;
}

int ScheduleSolution::getOrphinedShift(const int AIndex, const EScheduleMode emMode)
{
	int d;
	int result = 0;
	for (d = 0; d < WORKABLE_DAY_COUNT; d++)
		result = result + getOrphinedShiftByDay(AIndex, d, emMode);
	return result;
}
int ScheduleSolution::getIdxByDaytable(CDayTable* Adt)
{
	int sh = -1;

	int result = 0;
	int du;
	bool check1, check2;
	for (int d = 0; d < WORKABLE_DAY_COUNT; d++)
	{
		for (int h = 0; h < HOUR_TICK_COUNT; h++)
		{
			sh = (*Adt)[d][h];
			if (sh != -1)
			{
				du = _dShifts[sh].length;
				check1 = (
					((h + du < 6) || ((h >= 8) && (h + du < 14))) &&
					((*Adt)[d][h + du] == -1) &&
					((*Adt)[d][h + du + 1] == -1) &&
					((*Adt)[d][h + du + 2] != -1)
					);
				check2 = (
					((h + du < 4) || ((h >= 8) && (h + du < 12))) &&
					((*Adt)[d][h + du] == -1) &&
					((*Adt)[d][h + du + 1] == -1) &&
					((*Adt)[d][h + du + 2] == -1) &&
					((*Adt)[d][h + du + 3] == -1) &&
					((*Adt)[d][h + du + 4] != -1)
					);
				result = result + check1 + check2;
				h = h + du - 1;
			}
			h++;
		}

	}
	return result;
}

int ScheduleSolution::getCompactIdx(const int AIndex, const EScheduleMode emMode)
{

	int result = 0;
	switch (emMode)
	{
	case emClasse:
		result = getIdxByDaytable(&_dClasses[AIndex].weeka);
		break;
	case emProf:
		result = getIdxByDaytable(&_dProfs[AIndex].weeka);
		break;

	}
	return result;
}

int ScheduleSolution::getShiftsCount(const int AIndex, const EScheduleMode emMode)
{

	int result = 0;
	int i;
	if (AIndex < 0)
		return 0;

	switch (emMode)
	{
	case emClasse:
		for (i = 0; i < _dShifts.size(); i++)
			if (_dShifts[i].cindex == AIndex)
				result++;
		break;
	case emProf:
		for (i = 0; i < _dShifts.size(); i++)
			if (_dShifts[i].pindex == AIndex)
				result++;
		break;
	case emMat:
		for (i = 0; i < _dShifts.size(); i++)
			if (_dShifts[i].mindex == AIndex)
				result++;
		break;
	case emCroom:
		for (i = 0; i < _dShifts.size(); i++)
			if (_dShifts[i].crindex == AIndex)
				result++;
		break;
	}
	return result;

}

OptimizeInfo ScheduleSolution::getOptimizeInfo(const EScheduleMode emMode)
{
	int oi = 0, ci = 0;

	switch (emMode)
	{
	case emClasse:
		for (int e = 0; e < _stats.classes; e++)
		{
			oi += getOrphinedShift(e, emClasse);
			ci += getCompactIdx(e, emClasse);
		}
		break;
	case emProf:
		for (int e = 0; e < _stats.profs; e++)
		{
			oi += getOrphinedShift(e, emProf);
			ci += getCompactIdx(e, emProf);
		}
		break;
	default:
		oi = 0;
		ci = 0;
		break;
	}

	return OptimizeInfo(oi, ci);
}

bool ScheduleSolution::parkShift(const int Sindex, bool ABool)
{

	return true;
}

bool  ScheduleSolution::checkDT(const int Sindex, CDayTable* dt)
{

	bool result, checked;
	result = false;
	checked = false;
	auto se = &(_dShifts[Sindex]);
	int day = se->day;
	int hour = se->hour;
	if ((day < 0) || (day >= MAX_DAY_COUNT) || (hour < 0) || (hour >= HOUR_TICK_COUNT))
	{
		parkShift(Sindex, true);
		return false;
	}

	if ((*dt)[day][hour] != Sindex) {
		return false;
	}
	for (int d = 0; d < WORKABLE_DAY_COUNT; d++)
		for (int h = 0; h < HOUR_TICK_COUNT; h++)
			if ((*dt)[d][h] == Sindex)
			{
				result = ((se->day == d)
					&& (h >= se->hour)
					&& (h <= se->hour + se->length - 1));
				checked = (h >= (se->hour + se->length - 1));
				if (!result || checked)
					return result;
			}
	return true;
}

bool  ScheduleSolution::checkAllDT(const int Sindex)
{
	CShift& Se = _dShifts[Sindex];
	CClasse& Cl = _dClasses[Se.cindex];
	CCroom& Sa = _dCRooms[Se.crindex];
	CProf& Pr = _dProfs[Se.pindex];
	bool  checkA, checkB;


	//check classe table

	checkA = checkDT(Sindex, &(Cl.weeka));
	checkB = checkDT(Sindex, &(Cl.weekb));

	bool result0 =
		Se.bygroup
		&&
		(
			(
				Se.groupedwith != -1
				&& Se.dogroupwith == -1
				&& _dShifts[Se.groupedwith].groupedwith == Sindex
				&& _dShifts[Se.groupedwith].day == Se.day
				&& _dShifts[Se.groupedwith].hour == Se.hour
				&& _dShifts[Se.groupedwith].length == Se.length
				)
			||
			(checkA && checkB)

			)
		||
		(
			(Se.every2weeks && checkA)
			||
			(Se.every2weeks && checkB)
			||
			(checkA && checkB)

			);



#if DEBUG_VERBOSE   
	char s[254] = "";
	Console::SetCursorPosition(3, Console::GetCursorY());
	if (result0) {
		sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
	}
	else {
		sprintf_s(s, "\n\t->[%d]  checking stage1 .. [failure] checkA:%d,checkB:%d\n", Sindex, checkA, checkB);

	}
	Console::Write(s);
#endif

	//check croom table
	checkA = checkDT(Sindex, &(Sa.weeka));
	checkB = checkDT(Sindex, &(Sa.weekb));

	bool result1 = (Se.every2weeks && checkA) || (Se.every2weeks && checkB) || (checkA && checkB);


#if DEBUG_VERBOSE
	Console::SetCursorPosition(3, Console::GetCursorY());
	if (result1) {
		sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
	}
	else {
		sprintf_s(s, "\n\t->[%d]  checking stage2 .. [failure] checkA:%d,checkB:%d\n", Sindex, checkA, checkB);
	}
	Console::Write(s);
#endif


	//check prof table
	checkA = checkDT(Sindex, &(Pr.weeka));
	checkB = checkDT(Sindex, &(Pr.weekb));

	bool result2 = (Se.every2weeks && checkA)
		||
		(Se.every2weeks && checkB)
		||
		(checkA && checkB);


#if DEBUG_VERBOSE
	Console::SetCursorPosition(3, Console::GetCursorY());
	if (result2) {
		sprintf_s(s, "\t->[%d]  .. [ok]", Sindex);
	}
	else {
		sprintf_s(s, "\n\t->[%d]  checking stage3 .. [failure] \n checkA:%d,checkB:%d\n", Sindex, checkA, checkB);
	}
	Console::Write(s);
#endif


	//check grouping table
	checkA = checkDT(Sindex, &(Pr.weeka));
	checkB = checkDT(Sindex, &(Pr.weekb));

	bool result3 = (Se.every2weeks && checkA)
		||
		(Se.every2weeks && checkB)
		||
		(checkA && checkB);


#if DEBUG_VERBOSE  
	Console::SetCursorPosition(3, Console::GetCursorY());
	if (result3)
		sprintf_s(s, "   ->[%d]  .. [ok]", Sindex);
	else
		sprintf_s(s, "\n   ->[%d]  checking stage4 .. [failure]\n", Sindex);
	Console::Write(s);
	*/

#endif 

		return (result0 && result1 && result2 && result3);
}

bool   ScheduleSolution::checkProcessedShifts()
{

	int si = 0;
	while (si < _dShifts.size())
	{

		if (!checkAllDT(si))
			return false;
		si++;
	}

	return true;
}


bool ScheduleSolution::setLink(const int s1, const int s2, const ELinkType Alink)
{

	if ((s1 == -1) || (s2 == -1))
		return false;
	//   gc_RaiseError('Error: setLink with invalid parameters!');

	switch (Alink)
	{
	case ltGroup: //set Grouped link Shift index
		_dShifts[s1].groupedwith = s2;
		_dShifts[s2].groupedwith = s1;
		_dShifts[s1].dogroupwith = s2;
		_dShifts[s2].dogroupwith = -1;
		_modified = true;
		break;

	case ltClear:
		_dShifts[s1].groupedwith = -1;
		_dShifts[s2].groupedwith = -1;
		_dShifts[s1].dogroupwith = -1;
		_dShifts[s2].dogroupwith = -1;
		_modified = true;
		break;

	default:
		_modified = false;
		break;
	}
	return true;
}

bool ScheduleSolution::fillDT(const int ind)
{

	CShift* pShift = Shift(ind);
	CProf* pProf = Prof(pShift->pindex);
	CClasse* pClasse = Classe(pShift->cindex);
	CCroom* pCroom = Croom(pShift->crindex);
	int gw = -1;
	if ((pShift->groupedwith != -1) && (pShift->dogroupwith == -1)) {
		gw = pShift->groupedwith;
	}
	
	const EFillMode fo =(EFillMode) pShift->fillMode;
	const int& d = pShift->day;
	const int& h = pShift->hour;

		for (int i = 0; i < pShift->length; i++)
		{
			if ((fo == foMixte) || (fo == foWeekA))
			{
				if (gw == -1)
					pClasse->weeka[d][h + i] = ind;
				pProf->weeka[d][h + i] = ind;
				pCroom->weeka[d][h + i] = ind;
			}
			if ((fo == foMixte) || (fo == foWeekB))
			{
				if (gw == -1)
					pClasse->weekb[d][h + i] = ind;
				pProf->weekb[d][h + i] = ind;
				pCroom->weekb[d][h + i] = ind;
			}


		}
	
	return true;


}

void ScheduleSolution::clearAllDT()
{

	for (int d = 0; d < MAX_DAY_COUNT; d++)
		for (int h = 0; h < HOUR_TICK_COUNT; h++)
		{
			for (int i = 0; i < _dClasses.size(); i++)
			{
				_dClasses[i].weeka[d][h] = -1;
				_dClasses[i].weekb[d][h] = -1;
			}
			for (int i = 0; i < CroomTableCount(); i++)
			{
				_dCRooms[i].weeka[d][h] = -1;
				_dCRooms[i].weekb[d][h] = -1;
			}
			for (int i = 0; i < ProfTableCount(); i++)
			{
				_dProfs[i].weeka[d][h] = -1;
				_dProfs[i].weekb[d][h] = -1;
			}

		}
}

void ScheduleSolution::rebuildSolution()
{

	clearAllDT();
	for (int e = 0; e < _dShifts.size(); e++)
	{
		fillDT(e);
	}

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
EFillMode ScheduleSolution::getShiftFillMode(const int Se)
{
	int aday, ahour, croomi;

	EFillMode result = foMixte;

	if (_dShifts[Se].every2weeks)
	{
		croomi = _dShifts[Se].crindex;
		aday = _dShifts[Se].day;
		ahour = _dShifts[Se].hour;
		if (_dCRooms[croomi].weeka[aday][ahour] == Se)
			result = foWeekA;
		else  if (_dCRooms[croomi].weekb[aday][ahour] == Se)
			result = foWeekB;
		else
			result = foNoWhere;
	}
	return result;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool ScheduleSolution::canAlternate(const int Se1, const int Se2, const int Aday, const int Ahour)
{
	//printf("\nentering canAlternate()... Se1=%d Se2=%d", Se1, Se2);
	assert(Se1 >= 0 && Se1 < _dShifts.size() && Se2 >= 0 && Se2 < _dShifts.size());
	CShift& sh1 = _dShifts[Se1];
	CShift& sh2 = _dShifts[Se2];
	return(
		(Se1 != -1) && (Se2 != -1) && (Ahour >= 0) && (Ahour < HOUR_TICK_COUNT) && (Se1 != Se2) &&
		sh1.bygroup &&
		sh2.bygroup &&
		sh2.dogroupwith == -1 &&
		sh1.groupedwith == -1 &&
		sh2.groupedwith == -1 &&
		sh1.every2weeks == sh2.every2weeks &&
		sh1.length == sh2.length &&
		Aday == sh2.day &&
		Ahour == sh2.hour
		);

}

void ScheduleSolution::shellSort()
{
	int n, p;
	CShift* pSe;

	int Le = _dShifts.size();
	n = 0;
	while (n < Le)
	{
		n = 3 * n + 1;
	}

	while (n != 0)
	{
		n = n / 3;
		for (int i = n; i < Le; i++)
		{
			pSe = &_dShifts[i];
			p = i;

			while ((p > (n - 1)) && (_dShifts[p - n].length > pSe->length))
			{
				_dShifts[p] = _dShifts[p - n];
				p = p - n;
			}
			_dShifts[p] = *pSe;
		}
	}
}

bool ScheduleSolution::clearShift(const int se,const bool bClearDT)
{
	CShift* pShift = Shift(se);
	if(bClearDT)
	for (int i = 0; i < pShift->length; i++)
	{

		if ((pShift->fillMode == foMixte) || (pShift->fillMode == foWeekA))
		{
			_dCRooms[pShift->crindex].weeka[pShift->day][pShift->hour + i] = -1;
			_dClasses[pShift->crindex].weeka[pShift->day][pShift->hour + i] = -1;
			_dProfs[pShift->pindex].weeka[pShift->day][pShift->hour + i] = -1;
		}
		if ((pShift->fillMode == foMixte) || (pShift->fillMode == foWeekB))
		{
			_dCRooms[pShift->crindex].weekb[pShift->day][pShift->hour + i] = -1;
			_dClasses[pShift->cindex].weekb[pShift->day][pShift->hour + i] = -1;
			_dProfs[pShift->pindex].weekb[pShift->day][pShift->hour + i] = -1;
		}

	}
	
	pShift->crindex = -1;
	pShift->hour = -1;
	pShift->day = -1;
	pShift->fillMode = (int)(foNoWhere);

	if (pShift->groupedwith != -1)
	{
		setLink(se, pShift->groupedwith, ltClear);

	}
	return true;
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
std::vector <CMat>& ScheduleSolution::MatTable()
{
	return _dMats;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
std::vector <CProf>& ScheduleSolution::ProfTable()
{
	return _dProfs;
}
CCroom* ScheduleSolution::Croom(const int ind) {
	assert(ind >= 0 && ind < _dCRooms.size());
	return &_dCRooms.at(ind);
}
CClasse* ScheduleSolution::Classe(const int ind) {
	assert(ind >= 0 && ind < _dClasses.size());
	return &_dClasses.at(ind);
}

CShift* ScheduleSolution::Shift(const int ind) {
	assert(ind >= 0 && ind < _dShifts.size());
	return &_dShifts.at(ind);
}

CProf* ScheduleSolution::Prof(const int ind) {
	assert(ind >= 0 && ind < _dProfs.size());
	return &_dProfs.at(ind);
}

CMat* ScheduleSolution::Mat(const int ind) {
	assert(ind >= 0 && ind < _dMats.size());
	return &_dMats.at(ind);
}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
std::vector <CCroom>& ScheduleSolution::CroomTable()
{
	return _dCRooms;
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


int ScheduleSolution::CroomCountByType(int cr_type) {
	int i = 0;
	int result = 0;
	for (i = 0; i < _stats.crooms; i++) {
		if (_dCRooms[i].stype == cr_type)
			result++;

	}

	return result;

}
