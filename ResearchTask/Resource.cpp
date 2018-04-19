#pragma once
#include "stdafx.h"
#include "Resource.h"


Resource::Resource() {

}

Resource::Resource(const Resource & source)
{
	assignments = source.assignments;
	name = source.name;
	quantity = source.quantity;
	points = source.points;
}


Resource & Resource::operator=(Resource & source)
{
	assignments = source.assignments;
	name = source.name;
	quantity = source.quantity;
	points = source.points;
	return *this;
}

Resource::~Resource()
{

}


void Resource::addTask(Assignment * a) {
	if (a->quantity > this->quantity)
	{
		std::cout << "\nERROR1!!\n";
		return;
	}

/*
	points[0.0] = 0.0;
	points[2.0] = 0.0;
	points[4.0] = 0.0;
	points[6.0] = 0.0;

	std::cout << "_:  " << prev(1.0).first << "\n";
	std::cout << "_:  " << prev(2.0).first << "\n";
	std::cout << "_:  " << prev(3.0).first << "\n";
	std::cout << "-:  " << next(1.0).first << "\n";
	std::cout << "-:  " << next(2.0).first << "\n";
	std::cout << "-:  " << next(3.0).first << "\n";
*/

	//std::cout << a->task->getName() << " added\n";
	//std::cout << "-";
	if (points.size() != 0) {
		if (points.count(a->task->finish()) == 0) {
			points[a->task->finish()] = prev(a->task->finish()).second;
		}
		if (points.count(a->task->start()) == 0) {
			if (prev(a->task->start()).second + a->quantity > this->quantity)
			{
				std::cout << "\nERROR2!!\n";
				return;
			}
			points[a->task->start()] = prev(a->task->start()).second + a->quantity;
		}
		
		else {
			if (points[a->task->start()] + a->quantity > this->quantity)
			{
				std::cout << "\nERROR3!!\n";
				return;
			}
			points[a->task->start()] += a->quantity;
		}
		std::pair<float, float> startPoint = next(a->task->start());
		std::pair<float, float> endPoint = prev(a->task->finish());
		for (auto&i : points) {
			if ((i.first >= startPoint.first) && (i.first <= endPoint.first) && (startPoint.first <= endPoint.first)) {
				if (i.second + a->quantity > this->quantity)
				{
					std::cout << "\nERROR4!!\n";
					return;
				}
				i.second += a->quantity;
			} else { continue; }
		}
	} else {

		//std::cout << "love";
		points.insert(std::pair<float, float>(a->task->start(), a->quantity));
		points.insert(std::pair<float, float>(a->task->finish(), 0.0));
	}
	//std::cout << prev(a->task->start()).first << " " << prev(a->task->start()).second << '\n';
}

void Resource::deleteTask(Assignment * a)
{
	//std::cout << a->task->getName() << " deleted\n";
	if (a->task->getName() == "C") {
		float love = 0.0;
	}
	points[a->task->start()] -= a->quantity;

	std::pair<float, float> startPoint = next(a->task->start());
	std::pair<float, float> endPoint = prev(a->task->finish());
	for (std::pair<const float, float> &i : points) {
		if ((i.first >= startPoint.first) && (i.first <= endPoint.first) && (startPoint.first <= endPoint.first)) {
			float love = i.first;
			i.second -= a->quantity;
		} else { continue; }
	}

}

#include <algorithm> 
std::vector<std::pair<float, float>> Resource::findResourceGaps(Assignment * a) {
	//int n = points.size();
	//if (n == 0) { return{ std::pair < float,float >(0, UINT32_MAX) }; }
	std::vector<std::pair<float, float>> m;
	typedef std::map<float,float >::iterator ir;

	/*for (ir i = p.begin(); i != p.end(); i++) {
		std::cout << "[" << i->first << "]:" << i->second << " ";
	}

	std::cout << "\n"; */

	bool f = false;
	if (points.count(0.0) == 0) { points[0.0] = 0.0; f = true; }
	for (ir i = points.begin(); i != points.end();) {
		//std::cout << a->resource->quantity << " " << i->second << " " << a->quantity << "\n";
		if (a->resource->quantity - i->second >= a->quantity) {
			float start = i->first;
			bool c = true;
			float end = start;
			ir j;
			for (j = i; (j != points.end()) && c; j++) {
				//std::cout << "  " << a->resource->quantity << " " << j->second << " " << a->quantity << "\n";
				
				if (a->resource->quantity - j->second < a->quantity) { c = false; continue; }
				end = j->first;
			}
			if (c) { end = UINT32_MAX; m.push_back(std::pair<float,float>(start,end)); break; }
			m.push_back(std::pair<float, float>(start, end));
			i = j++;
		} else { i++; continue; }
	}

	/*std::cout << a->task->getName() << ": ";
	for (unsigned i = 0; i < m.size(); i++) {
		std::cout << "[" << m.at(i).first << "-" << m.at(i).second << ")";
	}
	std::cout << "\n"; */

	return m;
}

std::pair<float, float> Resource::prev(float x) {
	std::pair<float, float> itlow(0.0, 0.0);
	for (auto i : points)
	{
		if ((i.first >= itlow.first) && (i.first < x)) itlow = i;
	}
	//auto itlow = points.lower_bound(x);
	//if (itlow == points.end()) && () { return std::pair<float, float>(0.0, 0.0); }
	
	//if (itlow->first == x) { --itlow; }
	//if (itlow == points.end()) { return std::pair<float, float>(0.0, 0.0); }
	
	//return std::pair<float,float>(*(itlow));
	return itlow;
}

std::pair<float, float> Resource::next(float x) {
	std::pair<float, float> itlow(INT32_MAX, 0.0);
	for (auto i : points)
	{
		if ((i.first <= itlow.first) && (i.first > x)) itlow = i;
	}
	return itlow;
	/*auto itlow = points.upper_bound(x);
	if (itlow == points.end()) { return std::pair<float, float>(0.0, 0.0); }
	if (itlow->first == x) { ++itlow; }
	if (itlow == points.end()) { return std::pair<float, float>(0.0, 0.0); }
	return std::pair<float, float>(*(itlow));*/
}