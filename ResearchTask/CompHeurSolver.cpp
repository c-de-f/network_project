#include "stdafx.h"
#include <ctime>
#include "Solver.h"

static std::vector<double> weights
{
	3.75726,
	1.89387,
	1.09758,
	4.73431,
	3.15727,
	3.70132,
	7.74608,
	3.17136,
	1.07944,
	1.08083,
	1.14945,
	1.22765,
	3.69512,
	2.51469,
	2.85347,
	2.8098,
	3.70161,
	3.70693,
	3.20607,
	3.17545,
	1.11526
};

Task* CompHeurSolver::setWins()
{
	std::map<Task*, double> wins;

	for (int i = 1; i < heuristics.size(); i++)
	{
		SimpleHeuristics* _h = new SimpleHeuristics(i);

		Task *t = (_h->nextTask(p->tasks));
		//std::cout << t->getName() << '\n';
		if (wins.count(t) == 0) wins[t] =  1.0 / weights[_h->id];
		else wins[t] += 1.0 / weights[_h->id];
	}

	int max_wins = INT32_MIN;
	auto j = wins.begin();
	for (auto i = wins.begin(); i != wins.end(); i++) if (i->second > max_wins) { j = i; max_wins = i->second; }
	//std::cout << j->second << '\n';
	return j->first;

}

float CompHeurSolver::solve()
{
	clock_t tStart = clock();

	//active.clear();
	//statuses.clear();

	for (std::map<std::string, Task>::iterator &x = p->tasks.begin(); x != p->tasks.end(); x++) {
		if ((x->second.prev.size() == 0) /* && (statuses[&x->second] == ::PASSIVE)*/) {
			(&x->second)->setStatus(TaskStatus::ACTIVE);
			//tt = x->second;
			//active.insert(active.end(), std::pair<Task*, bool>(&x->second, true));
		}
		else {
			(&x->second)->setStatus(TaskStatus::PASSIVE);
			//active.insert(active.end(), std::pair<Task*, bool>(&x->second, false));
		}
	}
	//std::cout << active.size() << '\n';
	int c = 0;
	while (haveActiveTasks()) {

		c++;// { std::cout << "Calculating " << c << "/" << p->tasks.size() << "\n"; }


		Task* tt = setWins();

		tt->forceCalculating();

		addSuccessors(tt);


	}

	for (auto &x : p->tasks) {
		if (x.second.getStatus() != ::DONE) {
			p->error = true; p->msg += "NOT SOLVED\n";
			p->notsolved = true;
			break;
		}
	}

	p->cpu_time = (clock() - tStart);
	return p->makespan();
}