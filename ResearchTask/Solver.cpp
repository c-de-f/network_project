#pragma once
#include "stdafx.h"
#include <assert.h>
#include <time.h>
#include <algorithm>
#include "Solver.h"

bool Task::usesResource(Resource r) { for (auto i : assignments) if ((i->resource->name) == r.name) {return true; }   return false; }

Task* Solver::bestByHeur(Task* a, Task* b) {  return ((h->findMax) ^ ((h->function)(a) < (h->function)(b))) ? a : b; }

std::vector<Task*> Solver::sortedByHeur(std::vector<Task*> vec)
{
	if (h == NULL) return vec;
	std::sort(vec.begin(), vec.end(), [this](Task* a, Task* b) {if (h->findMax) return (h->function)(a) > (h->function)(b); return (h->function)(a) < (h->function)(b); });
	return vec;
}

int Solver::CheckFeasibility()
{
	int res = 0;
	for (auto &i : solution.first) if (i.second.finish() - i.second.start() != i.second.getDuration()) { std::cout << "Err finish\n"; res++; }
	//for (auto t : solution.first) std::cout << "Start: " << t.second.getName() << ' ' << t.second.start() << ' ';
	std::cout << '\n';
	for (auto t : solution.first)
	{
		
		float s = t.second.start(); float f = t.second.finish();
		std::map<Resource*, float> srcProfile;
		for (auto &r : p->resources) srcProfile[&r.second] = 0.0;
		for (auto tt : solution.first)
		{
		
			for (auto a : tt.second.assignments)
			{
				srcProfile[a->resource] += (float)tt.second.occupiesTime(s) * a->quantity;
				if (srcProfile[a->resource] > a->resource->quantity) { std::cout << "Err 4.1 at " << s << "with " << srcProfile[a->resource]  << "in " << a->resource->name<< '\n'; res++; }
			}
		}
		for (auto &r : p->resources) srcProfile[&r.second] = 0.0;
		for (auto tt : solution.first)
		{
			for (auto a : tt.second.assignments)
			{
				srcProfile[a->resource] += (float)tt.second.occupiesTime(f) * a->quantity;
				if (srcProfile[a->resource] > a->resource->quantity) { std::cout << "Err 4.2 at " << f << '\n'; res++; }
			}
			
		}
	}
	//std::cout << "Here\n";
	/*for (auto &i : p->tasks) std::cout << i.first << ' ' << i.second.getStatus() << ' ';
	std::cout << '\n';*/
	for (auto &i : p->tasks)
	{
		if (i.second.getStatus() != ::DONE) { for (auto j : i.second.next) if (j->getStatus() != ::PASSIVE) { res++; std::cout << "Err statuses with " << j->getName() << "\n"; } } //statuses
		for (auto j : i.second.next) if ((j->getStatus() == ::DONE) && (i.second.finish() > j->start()))
		{
			//std::cout << i.second.getName() << ' ' << i.second.finish() << ' ' << j->getName() << ' ' << j->start() << '\n';
			res++;
			std::cout << "Err links\n";
		}// links
	}
	for (auto &r : p->resources)
	{
		for (auto pt : r.second.points)
			if (pt.second > r.second.quantity) { res++; std::cout << "Err resources\n"; };// resources
	}
	return res; //overall
}


Solver::Solver(Problem * p, Heuristics* h, int depth)
{
	this->p = p;
	this->h = h;
	this->depth = depth;
	//this->hs = hs;
	float maxEarlyFinishTime = earlySolve();
	lateSolve(maxEarlyFinishTime);
	solution.second = INT32_MAX;
	solution.first = p->tasks;
	for (auto &i = p->tasks.begin(); i != p->tasks.end(); i++) { (&i->second)->setStatus(::PASSIVE); }
}


Task * Solver::getTaskToSchedule()
{
	return h->nextTask(p->tasks);
}

void Solver::addSuccessors(Task * task)
{
	task->setStatus(TaskStatus::DONE);
	std::vector<Task *> successors = task->next;
	for (unsigned i = 0; i < successors.size(); ++i) {
		Task * t = successors.at(i);
		bool alldone = true;
		for (unsigned j = 0; j < (*t).prev.size(); ++j) {
			Task * T = ((*t).prev).at(j);
			if (T->getStatus() != ::DONE) { alldone = false; break; }
		}
		if (alldone) t->setStatus(TaskStatus::ACTIVE);
		//active[t] = true;
	}
	//active[task] = false;
}

float HeurSolver::solve() {

	clock_t tStart = clock();

//	active.clear();
	//statuses.clear();

	for (std::map<std::string, Task>::iterator &x = p->tasks.begin(); x != p->tasks.end(); x++) {
		if ((x->second.prev.size() == 0) /* && (statuses[&x->second] == ::PASSIVE)*/) {
			(&x->second)->setStatus(TaskStatus::ACTIVE);
		//	active.insert(active.end(), std::pair<Task *, bool>(&x->second, true));
		} else {
			(&x->second)->setStatus(TaskStatus::PASSIVE);
		//	active.insert(active.end(), std::pair<Task *, bool>(&x->second, false));
		}
	}

	int c = 0;
	while (haveActiveTasks()) {

		c++; //if (depth == 0) { std::cout << "Calculating " << c << "/" << p->tasks.size() << "\n"; }
		//Task * tt = solve(nullptr, 0).first;
		Task * tt;
		tt = getTaskToSchedule();
		//DOESN"T WORK WITH NO HEUR!!! FIX!!!
		
		//if ((c == 1) && (depth == 0)) { p->tasks = newProblem.tasks; break; }
		//std::cout << tt->getName() << "\n";
		tt->forceCalculating();
		//std::cout << "B\n";
		addSuccessors(tt);
		//std::cout << "C\n";

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

bool Task::noResources() { if (assignments.size()) return false;  return true; }
bool Task::exclusiveUsage() { for (auto k = 0; k < assignments.size(); k++) if ((assignments[k]->quantity) && (assignments[k]->quantity != assignments[k]->resource->quantity)) return false; return true; }
bool Task::noConflicts() { for (auto k = 0; k < assignments.size(); k++) if (assignments[k]->quantity > 1) return false; return true; }

#include <iostream>     // std::cout
#include <iterator>     // std::next
#include <list>         // std::list
#include <algorithm>    // std::for_each
float Solver::partialEarlySolve()
{
	
	Problem tmP = *p;
	std::map<Task*, TaskStatus> statuses;
	//for (auto &i : p->tasks) std::cout << i.second.getName() << '\n';
	for (auto &i : p->tasks) {/* std::cout << i.second.getName() << '\n'; std::cout << i.second.getStatus() << '\n';*/ statuses.insert(std::pair<Task*, TaskStatus>(&i.second, i.second.getStatus())); }
	
	std::vector<Task*> act = {};
	for (auto &i : p->tasks) if (i.second.getStatus() == ::ACTIVE) act.push_back(&i.second);
	
	float max_time = 0.0;
	for (auto i : act)
	{
		float a = i->maxPrevFictiveFinish();
		float b = i->maxPrevFinish();
	//	std::cout << i->getName() << "has mpff " << a << " mpf " << b << '\n';
		i->fictiveStart = std::max (b, a);
		for (auto k : i->next)
		{
			if (k->fictiveStart > 0) continue;
			bool allDone = true;
			for (auto m : k->prev) if (m->getStatus() != ::DONE) allDone = false;
			if (allDone) act.push_back(k);

		}
		//tt->earlyFinish = tt->earlyStart() + tt->getDuration();
		//tt->setStatus(::DONE);

		//setStatuses();
	}
	for (auto &i : p->tasks) { float mm = i.second.fictiveStart + i.second.getDuration(); if (max_time < mm)  max_time = mm; (&i.second)->fictiveStart = 0; }
//	for (auto &i : p->tasks) (&i.second)->fictiveStart = 0;
	//p->tasks = tmp_tasks;
	//setStatuses();
	//for (auto &i : p->tasks) std::cout << i.second.getStatus();
	
	for (auto &i : p->tasks) if (statuses[&i.second] != i.second.getStatus()) std::cout << i.second.getName() << " GOT CHANGED from " << statuses[&i.second] << " to " << i.second.getStatus() << '\n'; 
	//for (auto &i : p->tasks) (&i.second)->fictiveStart = 0;
	//for (auto &i : p->tasks) { if (i.second.getStatus() != (&tmP)->tasks[i.first].getStatus()) std::cout << "Statuses changed from " << (&tmP)->tasks[i.first].getStatus() << " to " << i.second.getStatus() << '\n'; }
	return max_time;
}

float Solver::earlySolve()
{
	//active.clear(); //statuses.clear();
	for (auto i : p->tasks) (&i.second)->setStatus(::PASSIVE);
	float max_time = 0.0;
	for (auto &x : p->tasks) {
		if ((x.second.prev.size() == 0) /* && (statuses[&x->second] == ::PASSIVE)*/) {
			(&x.second)->setStatus(TaskStatus::ACTIVE);
			//active.insert(active.end(), std::pair<Task *, bool>(&x.second, true));
		}
		else {
			(&x.second)->setStatus(TaskStatus::PASSIVE);
			//active.insert(active.end(), std::pair<Task *, bool>(&x.second, false));
		}
	}

	while (haveActiveTasks()) {
		Task * tt = NULL; for (auto &i : p->tasks) { if (i.second.getStatus() == ::ACTIVE) { tt = &i.second; break; } }
		//std::pair<float, float> t;

		(*tt).forceEarlyCalculating();
		if (tt->earlyFinish() > max_time) { max_time = tt->earlyFinish(); }
		tt->setStatus(::DONE);
		std::vector<Task *> successors = tt->next;
		for (unsigned i = 0; i < successors.size(); ++i) {
			Task * t = successors.at(i);
			bool alldone = true;
			for (unsigned j = 0; j < (*t).prev.size(); ++j) {
				Task * T = ((*t).prev).at(j);
				if (T->getStatus() != ::DONE) { alldone = false; break; }
			}
			t->setStatus(TaskStatus::ACTIVE);
			(*t).total_prev += (*tt).total_prev + 1;
			//active[t] = true;
		}
		//active[tt] = false;
	}
	earlyTime = max_time;
	return max_time;
}

void Solver::lateSolve(float maxEarlyFinishTime)
{
	//active.clear(); //statuses.clear();
	for (auto &x : p->tasks) {
		if ((x.second.next.size() == 0) /* && (statuses[&x->second] == ::PASSIVE)*/) {
			(&x.second)->setStatus(TaskStatus::ACTIVE);
			//active.insert(active.end(), std::pair<Task *, bool>(&x.second, true));
		}
		else {
			(&x.second)->setStatus(TaskStatus::PASSIVE);
			//active.insert(active.end(), std::pair<Task *, bool>(&x.second, false));
		}
	}

	while (haveActiveTasks()) {
		Task * tt = NULL; for (auto &i : p->tasks) { if (i.second.getStatus() == ::ACTIVE) { tt = &i.second; break; } }
	//	std::pair<float, float> t;

		(*tt).forceLateCalculating(maxEarlyFinishTime);
		tt->setStatus(TaskStatus::DONE);
		for (unsigned i = 0; i < tt->prev.size(); ++i) {
			Task * t = tt->prev.at(i);
			bool alldone = true;
			for (unsigned j = 0; j < (*t).next.size(); ++j) {
				Task * T = ((*t).next).at(j);
				if (T->getStatus() != ::DONE) { alldone = false; break; }
			}
			t->setStatus(TaskStatus::ACTIVE);
			(*t).total_next += (*tt).total_next + 1;
			//active[t] = true;
		}

		if (tt->next.size() == 0) {
			tt->maxNextChain = 0; tt->minNextChain = 0;
		} else {
			for (auto &i : tt->next) {
				if (i->maxNextChain > tt->maxNextChain) { tt->maxNextChain = i->maxNextChain; }
				if (i->minNextChain < tt->minNextChain) { tt->minNextChain = i->minNextChain; }
			}
			tt->maxNextChain += 1;
			tt->minNextChain += 1;
		}

		//active[tt] = false;
	}

}

float Solver::time()
{
	float max = 0.0;
	for (std::map<std::string, Task>::iterator i = p->tasks.begin(); i != p->tasks.end(); i++) {
		if (max < (i->second).finish()) { max = (i->second).finish(); }
	}
	return max;
}

bool Solver::haveActiveTasks() {
	/*for (auto &i : p->tasks) std::cout << i.second.getStatus();
	std::cout << '\n';*/
	for (auto &i : p->tasks) if (i.second.getStatus() == ::ACTIVE) {  return true; }
	return false;
}

void Solver::setStatuses()
{
	
	for (auto &i : p->tasks)
	{
		//std::cout << "Entered\n";
		Task* t = &(i.second);
		
		//std::cout << "Mein Name " << t->getName() << '\n';
		if (t->getStatus() == TaskStatus::DONE)
		{
			continue;
		}
		bool alldone = true;
		if (t->prev.size()) for (unsigned j = 0; j < t->prev.size(); ++j)
		{
			
			int k;
			Task* tt = (t->prev.at(j));
			if (tt->getStatus() != TaskStatus::DONE)
			{
				alldone = false;
				break;

				
			}
		}
			//std::cout << '\n';
		if (alldone) t->setStatus(TaskStatus::ACTIVE);
		if (!alldone) t->setStatus(TaskStatus::PASSIVE);
	}
}

void Solver::tryTask(Task* t)
{
	int _r = r;
	//setMaxStart();
	//t->forceCalculating(maxStart);
	t->forceCalculating();
	 //simple enumeration rule
	/*if ((t->start() != 0) && (h))
	{
		for (auto k : p->tasks)
		{
			if (t == &k.second) continue;
			if (k.second.getStatus() != ::DONE) continue;
			if (t->start() != k.second.start()) continue;
			//rule = rule && (t != bestByHeur(t, &k.second));
		}
	}*/
	//if ((!t->exceeds(t->bound)) /*&& (earlySolve() < t->bound) && (rule)*/) //Second condition needs analysing cause should not eliminate optimum.
	{
		//t->setStatus(TaskStatus::DONE);
		addSuccessors(t);
		r++;
		if (r == p->resources.size()) r = 0;
		//setStatuses();
		solve();
	}
	for (auto & j : (t)->assignments)
	{
		j->resource->deleteTask(j);
	}
	r = _r;
	t->setStatus(TaskStatus::ACTIVE);
	setStatuses();
	return;
}

std::vector<Resource*> OptSolver::scheduledResourceUsage()
{
	std::vector<Resource*> res;
	for (auto &i : p->resources)
	{
		for (auto &j : p->tasks)
			if ((j.second.usesResource(i.second)) && (j.second.getStatus() == TaskStatus::DONE))
			{
				res.push_back(&i.second);
				break;
			}
	}
	return res;
}

