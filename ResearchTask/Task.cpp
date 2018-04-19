#pragma once
#include "stdafx.h"
#include "Task.h"

Task::Task()
{
	name = "";
	duration = 0.0;
}


Task::Task(const Task & source)
{
	name = source.name;
	duration = source.duration;
	es = source.es;
	ls = source.ls;
	s = source.s;
	prev = source.prev;
	next = source.next;
	assignments = source.assignments;
}

Task::Task(const Task & source, bool e)
{
	name = source.name;
	duration = source.duration;
	es = source.es;
	ls = source.ls;
	s = source.s;
}

Task::Task(float es, float ef)
{
	this->es = es;
	this->duration = ef - es;
}

Task::~Task() {}

Task & Task::operator=(Task & source) {
	std::swap(name, source.name);
	std::swap(duration, source.duration);
	es = source.es;
	ls = source.ls;
	s = source.s;
	prev = source.prev;
	next = source.next;
	assignments = source.assignments;
	return *this;
}


/*
// Calculates the task
#include <algorithm>
void Task::forceCalculating() {
	setStart(theoryCalculating());
	for (unsigned i = 0; i < assignments.size(); i++) {
		assignments.at(i)->resource->addTask(assignments.at(i));
	}
}*/

// Calculates the task
#include <algorithm>

void Task::forceCalculating(float maxStart)
{
	float l = std::max(maxPrevFinish(), maxStart);
	if (assignments.size() == 0) { setStart(l); return; }
	float r = -1;
	std::vector<std::vector<std::pair<float, float>>> intervals;
	std::vector<std::pair<float, float>> sum;
	for (unsigned i = 0; i < assignments.size(); i++) {
		//Assignment & a = * assignments.at(i);
		//a.resource->points.clear();
		//a.resource->points.insert(std::pair<float,float>(0,0));
		//std::cout << assignments.at(i)->resource->name;
		std::vector<std::pair<float, float>> f = assignments.at(i)->resource->findResourceGaps(assignments.at(i));
		if (i == 0) { sum = f; }
		intervals.push_back(f);
	}
	for (unsigned i = 0; i < intervals.size() - 1; i++) {
		sum = sumOfIntervals(sum, intervals[i + 1]);
	}
	for (unsigned i = 0; i < sum.size(); i++) {
		if (sum[i].second == UINT32_MAX) { r = sum[i].first; break; }
		if (sum[i].second - std::max(l, sum[i].first) >= getDuration()) {
				r = sum[i].first; break;
		}
	}
	setStart(std::max(l, r));
	for (unsigned i = 0; i < assignments.size(); i++) {
		assignments.at(i)->resource->addTask(assignments.at(i));
	}
}

void Task::forceCalculating() {
	float l = maxPrevFinish();
	if (assignments.size() == 0) { setStart(l); return; }
	float r = -1;
	std::vector<std::vector<std::pair<float, float>>> intervals;
	std::vector<std::pair<float, float>> sum;
	for (unsigned i = 0; i < assignments.size(); i++) {
		//Assignment & a = * assignments.at(i);
		//a.resource->points.clear();
		//a.resource->points.insert(std::pair<float,float>(0,0));
		//std::cout << assignments.at(i)->resource->name;
		std::vector<std::pair<float, float>> f = assignments.at(i)->resource->findResourceGaps(assignments.at(i));
		if (i == 0) { sum = f; }
		intervals.push_back(f);
	}
	for (unsigned i = 0; i < intervals.size() - 1; i++) {
		sum = sumOfIntervals(sum, intervals[i + 1]);
	}
	for (unsigned i = 0; i < sum.size(); i++) {
		if (sum[i].second == UINT32_MAX) { r = sum[i].first; break; }
		if (sum[i].second - sum[i].first >= getDuration()) {
			if (sum[i].first >= l) {
				r = sum[i].first; break;
			}
		}
		/*if (sum[i].second - std::max(l, sum[i].first) >= getDuration()) {
			r = sum[i].first; break;
		}*/
	}
	setStart(std::max(l, r));
	for (unsigned i = 0; i < assignments.size(); i++) {
		assignments.at(i)->resource->addTask(assignments.at(i));
	}
}

void Task::forceEarlyCalculating()
{
	setEarlyStart(maxPrevEarlyFinish());
}

void Task::forceLateCalculating(float time)
{
	setLateStart(minNextLateStart(time) - duration);
}

float Task::theoryCalculating()
{
	float l = maxPrevFinish();
	if (assignments.size() == 0) { return l; }
	float r = -1;
	std::vector<std::vector<std::pair<float, float>>> intervals;
	std::vector<std::pair<float, float>> sum;
	for (unsigned i = 0; i < assignments.size(); i++) {
		std::vector<std::pair<float, float>> f = assignments.at(i)->resource->findResourceGaps(assignments.at(i));
		if (i == 0) { sum = f; }
		intervals.push_back(f);
	}
	for (unsigned i = 0; i < intervals.size() - 1; i++) {
		sum = sumOfIntervals(sum, intervals[i + 1]);
	}
	for (unsigned i = 0; i < sum.size(); i++) {
		if (sum[i].second == UINT32_MAX) { r = sum[i].first; break; }
		if (sum[i].second - sum[i].first >= duration) {
			if (sum[i].first >= l) {
				r = sum[i].first; break;
			}
		}
	}
	return std::max(l, r);
}

// Returns max earlyFinish of all the previous tasks
float Task::maxPrevFinish() {
	float max = 0.0;
	for (unsigned i = 0; i < prev.size(); ++i) {
		if (prev.at(i)->getStatus() != ::DONE) continue;
		if (prev.at(i)->finish() > max) {
			max = prev.at(i)->finish();
		}
	}
	return max;
}

// Returns max earlyFinish of all the previous tasks
float Task::maxPrevEarlyFinish() {
	float max = 0.0;
	for (unsigned i = 0; i < prev.size(); ++i) {
		if (prev.at(i)->earlyFinish() > max) {
			max = prev.at(i)->earlyFinish();
		}
	}
	return max;
}

// Returns min lateStart of all the next tasks
float Task::minNextLateStart(float time)
{
	if (next.size() == 0) {
		return time;
	}
	float min = next.at(0)->lateStart();
	for (unsigned i = 1; i < next.size(); ++i) {
		if (next.at(i)->lateStart() < min) {
			min = next.at(i)->lateStart();
		}
	}
	//std::cout << name << " " << min << "\n";
	return min;
}

#include <algorithm>
#include <list>
std::vector<std::pair<float, float>> Task::sumOfIntervals(std::vector<std::pair<float, float>> n1, std::vector<std::pair<float, float>> n2) {

	size_t i1 = 0;
	size_t i2 = 0;
	typedef std::pair<float, float> intl;
	std::vector<intl> r;

	while ((i1 < n1.size()) && (i2 < n2.size())) {
	//for (i1 = 0; i1 < n1.size(); i1++) {
		//for (i2 = 0; i2 < n2.size(); i2++)
		//{
			float b = std::max(n1[i1].first, n2[i2].first);
			float f = std::min(n1[i1].second, n2[i2].second);
			if (f > b)
			{
				intl p(b, f);
				r.push_back(p);
			}
			if (n1[i1].second < n2[i2].second) i1++;
			else i2++;
		//}
		/*
		//bool earlier = (n1[i1].first < n2[i2].first);
		if (n1[i1].first < n2[i2].first) {
			// [   )
			//	[   )
			if (n1[i1].second < n2[i2].first) {
				// [   )
				//        [    )
				i1++; continue;
			} else {
				// [          )
				//   [    )
				r.push_back(intl(n2[i2].first, std::min(n1[i1].second, n2[i2].second)));
				if (n1[i1].second > n2[i2].second) {
					i2++; continue;
				}
				if (n1[i1].second < n2[i2].second) {
					i1++; continue;
				}
				i1++; i2++; continue;
			}
		}
		if (n1[i1].first > n2[i2].first) {
			//   [    )
			// [    )
			if (n1[i1].first > n2[i2].second) {
				//         [   )
				// [    )
				i2++; continue;
			} else {
				//    [    )
				// [    )
				r.push_back(intl(n1[i1].first, std::min(n1[i1].second, n2[i2].second)));
				if (n1[i1].second > n2[i2].second) {
					i2++; continue;
				}
				if (n1[i1].second < n2[i2].second) {
					i1++; continue;
				}
				i1++; i2++; continue;
			}
		}
		if (n1[i1].first == n2[i2].first) {
			r.push_back(intl(n1[i1].first, std::min(n1[i1].second, n2[i2].second)));
			if (n1[i1].second > n2[i2].second) {
				i2++; continue;
			}
			if (n1[i1].second < n2[i2].second) {
				i1++; continue;
			}
			i1++; i2++; continue;
		}
		if (n1[i1].second == n2[i2].second) {
			r.push_back(intl(std::max(n1[i1].first, n2[i2].first), n2[i2].second));
			if (n1[i1].second > n2[i2].second) {
				i2++; continue;
			}
			if (n1[i1].second < n2[i2].second) {
				i1++; continue;
			}
			i1++; i2++; continue;
		}
		*/
	}
	
	return r;

}

float Task::totalUtilizationRatio() {
	float r = 0.0;
	for (int j = 0; j < assignments.size(); j++) { r += assignments[j]->quantity / assignments[j]->resource->quantity; }
	return r;
}
float Task::resourceUtilizationRatio() {
	float r = 0.0;
	for (int j = 0; j < assignments.size(); j++) { if (r < assignments[j]->quantity / assignments[j]->resource->quantity) { r = assignments[j]->quantity / assignments[j]->resource->quantity; } }
	return r;
}






