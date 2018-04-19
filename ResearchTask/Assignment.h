#pragma once
#include "stdafx.h"

#include "Task.h"
//#include "Resource.h"
class Resource;
class Task;

class Assignment {
public:
	Assignment();
	Assignment(Resource * r, Task * t, float q);
	Assignment(const Assignment& source);
	Assignment& operator = (Assignment& source);
	~Assignment();

	Resource * resource;
	Task * task;
	float quantity = 0.0;
};

