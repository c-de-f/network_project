#pragma once
#include "stdafx.h"
#include "Assignment.h"
#include "algorithm"

class Problem {
public:

	Problem();
	Problem(std::string);

	std::string name = "";

	std::map<std::string, Task> tasks;
	std::map<std::string, Resource> resources;
	std::vector<Assignment> assignments;




	std::vector<Resource> getResources()
	{
		std::vector<Resource> res;
		for (auto i : resources) res.push_back(i.second);
		return res;
	}
	float getLowerBound()
	{
		float lb = 0;
		//std::cout << tasks.size() << '\n';
		for (auto i : tasks)
		{
			//std::cout << i.second.getDuration() << '\n';
			float rp = i.second.rankPositionalWeight();
			if (rp > lb) lb = rp;
		}
		return lb;
	}

	int countLinks = 0;
	float makespan() { float m = 0.0; for (auto &i : tasks) { if (i.second.finish() > m) { m = i.second.finish(); } } return m; }
	bool error = false;
	bool notsolved = false;
	std::string msg = "";
	std::string dump();
	std::string definition();

	float cpu_time = 0.0;

	static bool stringHasPrefix(std::string p, std::string str);
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::string Convert(float number);
	static bool isFloat(std::string myString);

};