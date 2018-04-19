#pragma once
#include "stdafx.h"
#include <ctime>
#include "Problem.h"
#include "Heuristics.h"
class Solver {
public:
	float maxStart = 0;
	void setMaxStart() { return; }
	Solver(Problem * p, Heuristics* h, int depth);
	Task * getTaskToSchedule();
	void addSuccessors(Task * tt_index);
	time_t timer = clock();
	float max = INT32_MAX;
	virtual float solve() = 0;
	bool rule = true;
	//std::pair<Task *, float> solve(Task * index, int counter);
	float partialEarlySolve();
	float earlySolve();
	float earlyTime;
	void lateSolve(float maxEarlyFinishTime);
	
	//void optSolve();
	std::pair<std::map<std::string, Task>, float> solution;

	bool haveActiveTasks();
	std::map<Task *, bool> active;
	//std::map<Task*, TaskStatus> statuses;
	std::map<Resource*, std::vector<Task*>> sourceUsers;
	void setSourceUsers()
	{
		/*for (auto r: p->resources)
		for (auto i : p->tasks)
		{
			if (i.second.usesResource(r.second)) sourceUsers[&r.second].push_back(&i.second);
		}
		return;*/
	}

	Problem * p;
	Heuristics* h;
	int depth;
	int iterations = 0;

	float time();
	float makespan = INT32_MAX;
	Problem newProblem;
	void setStatuses();
	void tryTask(Task* t);
	int r = 0;
	std::vector<Task*> sortedByHeur(std::vector<Task*> vec);
	Task* bestByHeur(Task* a, Task* b);

	int CheckFeasibility();
	
	//static std::vector<std::string> heuristics;

};

class HeurSolver : public Solver {
public:
	HeurSolver(Problem * p, Heuristics* h) : Solver(p, h, 0) {}
	float solve() override;
	

};

class OptSolver : public Solver {
public:
	OptSolver(Problem * p) : Solver(p, 0, 0) {}

	float solve() override;
	//float solve();
	std::vector<Resource*> scheduledResourceUsage();
	std::vector<Resource> resources = p->getResources();
	int r = 0;

};

class ExactSolver : public Solver {
public:
	ExactSolver(Problem * p, Heuristics* h) : Solver(p, h, 0) {}
	std::vector<Resource> resources = p->getResources();
	int cuts1;
	int cuts2;
	float solve() override;
	//float solve();
};

class CompHeurSolver : public Solver {
public:
	CompHeurSolver(Problem * p) : Solver(p, 0, 0)
	{

		//for (auto i : p->tasks) wins[i.second] = 0;
		//std::cout << wins.size() << '\n';

	}
	float solve() override;
	//std::map<Task, int> wins;
	Task* setWins();

};

class MixedSolver : public Solver {
public:
	MixedSolver(Problem * p, Heuristics* h, int depth) : Solver(p, h, depth) { }
	float solve() override;
	int bound(int _depth); //Recurrent
};

class SprecherSolver : public Solver {
public:
	SprecherSolver(Problem * p, Heuristics* h) : Solver(p, h, 0) { }

	float solve() override;
	bool rule = true;
	void setMaxStart()
	{
		for (auto i : p->tasks) if (i.second.start() > maxStart) maxStart = i.second.start();
		return;
	}
};

