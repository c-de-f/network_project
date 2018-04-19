#include "stdafx.h"
#include <ctime>
#include "Solver.h"

#pragma once

float ExactSolver::solve() {
	//std::cout << p->tasks.size() << '\n';
	bool sch = true;
	for (auto &i = p->tasks.begin(); i != p->tasks.end(); i++) if (i->second.getStatus() != ::DONE) sch = false;
	if ((p->getLowerBound() > solution.second) && (sch)) { cuts2++; return makespan; } // BnB, lowerbound is min links-feasible finish time
	if ((sch) && (p->makespan() > solution.second)) { cuts2++;  return makespan; }
	

	bool actives = false;
	bool passives = false;

	setStatuses();
	//std::cout << "C\n";

	for (auto &i = p->tasks.begin(); i != p->tasks.end(); i++)
	{
		Task *t = &i->second;
		//if (statuses[t] != TaskStatus::ACTIVE) actives = true;
		if ((t->getStatus() == TaskStatus::ACTIVE) && (t->noResources()))
		{
			t->forceCalculating();
			t->setStatus(TaskStatus::DONE);
		}
	}
	setStatuses();
	std::vector<Task*> act;
	for (auto &i = p->tasks.begin(); i != p->tasks.end(); i++)
	{
		Task *t = &i->second;
		if (t->getStatus() != TaskStatus::ACTIVE)
		{
			if (t->getStatus() == TaskStatus::PASSIVE) passives = true;
			continue;
		}
		actives = true;
		act.push_back(t);
	}

	act = sortedByHeur(act);

	for (auto t : act)
	{
		//t->setBound(solution.second);
		tryTask(t);

		for (auto &l = p->tasks.begin(); l != p->tasks.end(); l++) if (l->second.getStatus() != TaskStatus::DONE) l->second.setStart(0);

		setStatuses();
	}


	if (!actives)
	{
		if (!passives)
		{
			iterations++;
			if (!(iterations % 10000)) std::cout << iterations << "  " << (clock() - timer) / CLOCKS_PER_SEC << " cuts: " << cuts2 << '\n';

			std::pair<std::map<std::string, Task>, float> m;

			m.first = p->tasks;
			m.second = p->makespan();
			if ((m.second) < solution.second)
			{
				solution.first = m.first;
				solution.second = m.second;
				//makespan = std::min(makespan, solution.second);

			}
		}
		return makespan;
	}
}