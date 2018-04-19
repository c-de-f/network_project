#include "stdafx.h"
#include "Heuristics.h"
#pragma once



static std::vector<std::string> descr = {
	"0",
	"min successors number",
	"max successors number",

	"max maxNextChain",
	"min minNextChain",
	"min duration",
	"max duration",
	"max rankposweight",
	"min totalfloat",
	"min latestart",
	"min latefinish",
	"min feasible start",
	"min feasible finish",
	"max theoryCalculating() - lateStart()",
	"min theoryCalculating() - lateStart()",
	"max timeToCompletion()",
	"max assignments.size()",
	"max sum(j) assignments[j]->quantity / assignments[j]->resource->quantity;",
	"max max(j) assignments[j]->quantity / assignments[j]->resource->quantity;",
	"min sum(j) assignments[j]->quantity / assignments[j]->resource->quantity;",
	"min max(j) assignments[j]->quantity / assignments[j]->resource->quantity;",
	"min rankposweight"
	//"min remaining critical path == estimation without resources"
};




static float IS(Task * t) {
	//std::cout << "1\n";
	return t->next.size();
}

static float LSC(Task * t) {
	//std::cout << "3\n";
	return t->maxNextChain;
}


SimpleHeuristics::SimpleHeuristics(int id)
{
	this->id = id;
	this->name = heuristics[id];
	this->description = descr[id];
	this->setById(id);
}



HierHeuristics::HierHeuristics(std::vector<int> ids)
{
	this->ids = ids;
	this->name = heuristics[ids[0]];

	this->description = descr[ids[0]]; // descr[id];
	for (auto i = 1; i < ids.size(); i++) { name += " + " + heuristics[ids[i]]; description += " + " + descr[ids[i]]; }
}



static float SSC(Task * t) {
	//std::cout << "4\n";
	return t->minNextChain;
}

static float PT(Task * t) {
	//std::cout << "5\n";
	return t->getDuration();
}

static float RPW(Task * t) {
	//std::cout << "6\n";
	return t->rankPositionalWeight();
}

static float TF(Task * t) {
	//std::cout << "7\n";
	return t->totalFloat();
}

static float LS(Task * t) {
	//std::cout << "8\n";
	return t->lateStart();
}

static float LF(Task * t) {
	//std::cout << "9\n";
	return t->lateFinish();
}

static float FES(Task * t) {
	//std::cout << "10\n";
	return t->theoryCalculating();
}

static float FEF(Task * t) {
	//std::cout << "11\n";
	return t->theoryCalculating() + t->getDuration();
}

static float FFD(Task * t) {
	//std::cout << "12\n";
	return t->theoryCalculating() - t->lateStart();
}

static float TC(Task * t) {
	//std::cout << "13\n";
	return (std::max(t->theoryCalculating(), t->lateStart()) + t->getDuration() - t->lateFinish());
}

static float TR(Task * t) {
	//std::cout << "14\n";
	return t->assignments.size();
}

static float TUR(Task * t) {
	//std::cout << "15\n";
	return t->totalUtilizationRatio();
}

static float RUR(Task * t) {
	//std::cout << "16\n";
	return t->resourceUtilizationRatio();
}





void Heuristics::setById(int id)
{
	switch (id) {
		case 0: break;
		case 1: findMax = false; this->function = &IS; break;
		case 2: findMax = true;	this->function = &IS; break;
		case 3: findMax = true; this->function = &LSC; break;
		case 4: findMax = false; this->function = &SSC; break;
		case 5: findMax = false; this->function = &PT; break;
		case 6: findMax = true; this->function = &PT; break;
		case 7: findMax = true; this->function = &RPW; break;
		case 8: findMax = false; this->function = &TF; break;
		case 9: findMax = false; this->function = &LS; break;
		case 10: findMax = false; this->function = &LF; break;
		case 11: findMax = false; this->function = &FES; break;
		case 12: findMax = false; this->function = &FEF; break;
		case 13: findMax = true; this->function = &FFD; break;
		case 14: findMax = false; this->function = &FFD; break;
		case 15: findMax = true; this->function = &TC; break;
		case 16: findMax = true; this->function = &TR; break;
		case 17: findMax = true; this->function = &TUR; break;
		case 18: findMax = true; this->function = &RUR; break;
		case 19: findMax = false; this->function = &TUR; break;
		case 20: findMax = false; this->function = &RUR; break;
		case 21: findMax = false; this->function = &RPW; break;
		//case 22: findMax = false; this->function = &RCP; break;
		default: break;
	}
return;
}



Task* SimpleHeuristics::nextTask(std::map<std::string, Task> tasks)
{
	Task* task = NULL;
	for (auto t : tasks) if (t.second.getStatus() == ::ACTIVE) {
		task = &t.second; break;
	}
	if (findMax) {
		float max = INT32_MIN;

		for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } float t = function(&j.second); if (t > max) { max = t; task = &j.second; } }

	}
	else {
		float min = INT32_MAX;
		for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } float t = function(&j.second); if (t < min) { min = t; task = &j.second; } }

	}
	return task;
}



Task* HierHeuristics::nextTask(std::map<std::string, Task> tasks)
{
	Task* task = NULL;
	for (auto t : tasks) if (t.second.getStatus() == ::ACTIVE) {
		task = &t.second; break;
	}
	//if (tasks.size() == 1) return task;
	std::map<std::string, Task> tmp = tasks;
	std::map<std::string, Task> tmp2;
	for (auto i : ids)
	{
		setById(i);
		if (findMax) {
			float max = INT32_MIN;

			for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } float t = function(&j.second); if (t > max) { max = t; } }
			for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } if (function(&j.second) == max) tmp2[j.first] = j.second; }
		}
		else {
			float min = INT32_MAX;
			for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } float t = function(&j.second); if (t < min) { min = t; } }
			for (auto j : tasks) { if (j.second.getStatus() != ::ACTIVE) { continue; } if (function(&j.second) == min) tmp2[j.first] = j.second; }
		}
		tmp.clear();
		tmp = tmp2;
		tmp2.clear();
		task = &tmp.begin()->second;
		if (tmp.size() == 1) break;
	}
	return task;
}



