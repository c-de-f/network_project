#pragma once
#include "stdafx.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "Assignment.h"
//#include "Task.h"
class Assignment;
class Task;

class Resource {
public:
	Resource();
	Resource(const Resource& source);
	Resource(std::string n, float l) { name = n; quantity = l; points = std::map<float, float>(); }
	Resource& operator = (Resource& source);
	~Resource();

	std::vector<const Assignment *> assignments;
	std::string name;
	float quantity;

	std::map<float, float> points;
	void addTask(Assignment * a);
	void deleteTask(Assignment * a);
	std::vector<std::pair<float, float>> findResourceGaps(Assignment * a);
protected:
	std::pair<float, float> prev(float x);
	std::pair<float, float> next(float x);
};

