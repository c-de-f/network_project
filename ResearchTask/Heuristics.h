#pragma once
#include "stdafx.h"
#include "Assignment.h"
#include <algorithm>
#undef min
#undef max

//#include "heur.h"


class Heuristics
{
public:
	Heuristics() {}
	~Heuristics() {}
	virtual Task* nextTask(std::map<std::string, Task> tasks) = 0;
	float(*function)(Task*);
	bool findMax;
	void setById(int id);
	
	std::string name;
	std::string description;
	
	/*float IS(Task * t);
	float LSC(Task * t);
	float SSC(Task * t);
	float PT(Task * t);
	float RPW(Task * t);
	float TF(Task * t);
	float LS(Task * t);
	float LF(Task * t);
	float FES(Task * t);
	float FEF(Task * t);
	float FFD(Task * t);
	float TC(Task * t);
	float TR(Task * t);
	float TUR(Task * t);
	float RUR(Task * t);*/
};


class SimpleHeuristics : public Heuristics
{
public:
	SimpleHeuristics(int id);
	/*{ 
		this->id = id; 
		this->name = heuristics[id]; 
		this->description = descr[id];  
		this->setById(id);
	}*/
	~SimpleHeuristics() {};
	Task* nextTask(std::map<std::string, Task> tasks) override;
	

	int id;
};

class HierHeuristics : public Heuristics
{
public:
	HierHeuristics(std::vector<int> ids);
	/*{
		this->ids = ids;
		this->name = heuristics[ids[0]];
			
		this->description = descr[ids[0]]; // descr[id];
		for (auto i = 1; i < ids.size(); i++) { name += " +" + heuristics[ids[i]]; description += " +" + descr[ids[i]]; }	
	}*/
	~HierHeuristics() {};

	Task* nextTask(std::map<std::string, Task> tasks) override;
	

	std::vector<int> ids;

};


static std::vector<std::string> heuristics = {
	"NONE",  // 0
	"LIS",   // 1 min next.size() 
	"MIS",   // 2 max next.size() 

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
	"MRUR",   // 18 max rratio RUR
	"ITUR", //19 - mIn TUR
	"IRUR", //20 - mIn RUR
	"LRPW" // 21 - opposite to GRPW
	//"MRCP" //22 - min remaining critical path == estimation without resources
};

