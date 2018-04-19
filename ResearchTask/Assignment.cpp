#pragma once
#include "stdafx.h"
#include "Assignment.h"

Assignment::Assignment()
{
}

Assignment::Assignment(Resource * r, Task * t, float q) {
	resource = r;
	task = t;
	//task->assignments.push_back(this);
	quantity = q;
}

Assignment::Assignment(const Assignment & source)
{
	resource = source.resource;
	task = source.task;
	quantity = source.quantity;
}

Assignment & Assignment::operator=(Assignment & source)
{
	resource = source.resource;
	task = source.task;
	quantity = source.quantity;
	return *this;
}

Assignment::~Assignment()
{
}
