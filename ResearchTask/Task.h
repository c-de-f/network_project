#pragma once
#include "stdafx.h"

#include "string"
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <map>

#include "Assignment.h"
#include "Resource.h"
class Assignment;
class Resource;

enum TaskStatus {
	DONE,
	ACTIVE,
	PASSIVE
};

class Task {
public:

	// Constructors & Operators
	Task();
	Task(const Task& source);
	Task(const Task& source, bool e);
	Task(float es, float ef);
	Task(std::string n, float d) { name = n; duration = d; }
	Task& operator = (Task& source);
	
	friend int operator<(const Task& left,

		const Task& right)
	{
		//std::cout << left.name << "  " << right.name << std::endl;
		return stoi(left.name) < stoi(right.name);
	}

	~Task();

	bool noResources(); //{ if (assignments.size()) return false; return true; }
	bool exclusiveUsage(); //{ for (auto k = 0; k < assignments.size(); k++) if ((assignments[k]->quantity) && (assignments[k]->quantity != assignments[k]->resource->quantity)) return false; return true; }
	bool noConflicts(); //{ for (auto k = 0; k < assignments.size(); k++) if (assignments[k]->quantity > 1) return false; return true; }
	// Getters & Setters
	std::string getName() { return name; }
	float fictiveStart = 0.0;


	float Task::maxPrevFictiveFinish() {
		float max = 0.0;
		for (unsigned i = 0; i < prev.size(); ++i) {
			if (prev.at(i)->fictiveStart + prev.at(i)->duration > max) {
				max = prev.at(i)->fictiveStart + prev.at(i)->duration;
			}
		}
		//std::cout << name << " may fictively start at " << max << '\n';
		return max;
	}
	/*float Task::maxPrevFictiveFinish() {
		float max = 0.0;
		for (unsigned i = 0; i < prev.size(); ++i) {
			
				max = (std::max)(max, prev.at(i)->fictiveStart + prev.at(i)->duration);
			}
		return max;
	}*/
	float getDuration() { return duration; }
	float earlyStart() { return es; }
	float earlyFinish() { return es + duration; }
	float lateStart() { return ls; }
	float lateFinish() { return ls + duration; }
	float start() { return s; }
	float finish() { return s + duration; }
	void setName(std::string n) { name = n; }
	void setDuration(float d) { duration = d; }
	void setEarlyStart(float f) { es = f; }
	void setLateStart(float f) { ls = f; }
	void setStart(float f) { s = f; /*std::cout << name << " set to " << f << '\n';*/ }
	int occupiesTime(float time)
	{
		//std::cout << "Checking: " << name << ' ' << start() << ' ' << finish() << '\n';
		if ((start() <= time) && (time < finish())) {  return 1; }
		return 0;
	}
	void forcePartialCalculating()
	{
		std::cout << "Shouldnot get here\n";
		float t = (std::max)(maxPrevEarlyFinish(), maxPrevFinish());
		setEarlyStart(t);
	}
	float totalFloat() { return ls - es; }
	float rankPositionalWeight() { return maxPrevFinish() + getDuration(); }
	float feasibleFinishDeviation() { return finish() - lateFinish(); }
	float timeToCompletion(float pm) { return (std::max)(theoryCalculating(), lateFinish()) + pm - lateFinish(); }
	float totalUtilizationRatio();
	float resourceUtilizationRatio();

	// Linkers
	void addPrevLink(Task *t) { prev.push_back(t); }
	void addNextLink(Task *t) { next.push_back(t); }

	// Logic
	void forceCalculating();
	void forceCalculating(float maxStart);
	void forceEarlyCalculating();
	void forceLateCalculating(float time);
	float theoryCalculating();

	float bound = INT32_MAX;
	void setBound(float bnd) { bound = bnd; }
	bool exceeds(float maxbound) { if (s + duration > maxbound) return true; return false; }

	std::vector<Task *> prev;
	std::vector<Task *> next;
	std::vector<Assignment *> assignments;
	bool usesResource(Resource r);
	//{ for (auto i : assignments) if (i->resource == r) return true; return false; }

	int total_prev = 0;
	int total_next = 0;

	int maxNextChain = 0;
	int minNextChain = 0;
	bool scheduled = false;

	void setStatus(TaskStatus st) { status = st; }
	TaskStatus getStatus() { return status; }
	float maxPrevFinish();
protected:
	std::string name;
	float duration;
	TaskStatus status = ::PASSIVE;

	float s = 0.0;
	float es = 0.0;
	float ls = 0.0;
	

	float maxPrevEarlyFinish();
	float minNextLateStart(float time);
public:
	static std::vector<std::pair<float, float>> sumOfIntervals(std::vector<std::pair<float, float>> n1, std::vector<std::pair<float, float>> n2);
};
