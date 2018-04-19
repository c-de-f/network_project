#pragma once
#include "Assignment.h"
#include "stdafx.h"
#include <algorithm>
#undef min
#undef max

class Task;

std::vector<std::string> heuristics = {
	"NONE",  // 0
	"LIS",   // 1 min next.size() IS
	"MIS",   // 2 max next.size() IS

	"LSC",   // 3 max maxNextChain LSC
	"SSC",   // 4min minNextChain SSC
	"SPT",   // 5min duration PT
	"LPT",   // 6max duration PT
	"GRPW",   //7max rankposweight RPW
	"MSLK",  // 8 min totalfloat TF
	"MLST",   //9 min latestart LS
	"MLFT",   //10 min latefinish LF
	"MFEST",   //11 min start FES
	"MFEFT",   //12 min finish FEF
	"MFFD",  //  13 max ffd FFD
	"LFFD",   // 14min ffd FFD
	"MTC",   //  15max timeToCompletion() TC
	"MTR",   // 16max assignments.size() TR
	"MTUR",   // 17max tratio TUR
	"MRUR"   // 18 max rratio RUR
	"ITUR", //19 - mIn TUR
	"IRUR" //20 - mIn RUR
};

std::vector<std::string> descr = {
	"0",
	"min next.size() IS",
	"max next.size() IS",

	"max maxNextChain LSC",
	"min minNextChain SSC",
	"min duration PT",
	"max duration PT",
	"max rankposweight RPW",
	"10 min totalfloat TF",
	"min latestart LS",
	"min latefinish LF",
	"min start FES",
	"min finish FEF",
	"max ffd FFD",
	"min ffd FFD",
	"max timeToCompletion() TC",
	"max assignments.size() TR",
	"max tratio TUR",
	"max rratio RUR",
	"min TUR",
	"min RUR"
};

float IS(Task * t) {
	//std::cout << "1\n";
	return t->next.size();
}



float LSC(Task * t) {
	//std::cout << "3\n";
	return t->maxNextChain;
}

float SSC(Task * t) {
	//std::cout << "4\n";
	return t->minNextChain;
}

float PT(Task * t) {
	//std::cout << "5\n";
	return t->getDuration();
}

float RPW(Task * t) {
	//std::cout << "6\n";
	return t->rankPositionalWeight();
}

float TF(Task * t) {
	//std::cout << "7\n";
	return t->totalFloat();
}

float LS(Task * t) {
	//std::cout << "8\n";
	return t->lateStart();
}

float LF(Task * t) {
	//std::cout << "9\n";
	return t->lateFinish();
}

float FES(Task * t) {
	//std::cout << "10\n";
	return t->theoryCalculating();
}

float FEF(Task * t) {
	//std::cout << "11\n";
	return t->theoryCalculating() + t->getDuration();
}

float FFD(Task * t) {
	//std::cout << "12\n";
	return t->theoryCalculating() - t->lateStart();
}

float TC(Task * t) {
	//std::cout << "13\n";
	return (std::max(t->theoryCalculating(), t->lateStart()) + t->getDuration() - t->lateFinish());
}

float TR(Task * t) {
	//std::cout << "14\n";
	return t->assignments.size();
}

float TUR(Task * t) {
	//std::cout << "15\n";
	return t->totalUtilizationRatio();
}

float RUR(Task * t) {
	//std::cout << "16\n";
	return t->resourceUtilizationRatio();
}

