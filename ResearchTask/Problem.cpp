#pragma once
#include "stdafx.h"
#include <sstream>
#include "Problem.h"

Problem::Problem()
{
	name = "Name";
}

std::string remove_extra_whitespaces(std::string input)
{
	char output[0x255] = "";
	char writable[0x255];
	strcpy_s(writable, input.c_str());

	int inputIndex = 0;
	int outputIndex = 0;
	while (writable[inputIndex] != '\0')
	{
		output[outputIndex] = writable[inputIndex];

		if (writable[inputIndex] == ' ')
		{
			while (writable[inputIndex + 1] == ' ')
			{
				// skip over any extra spaces
				inputIndex++;
			}
		}

		outputIndex++;
		inputIndex++;
	}

	// null-terminate output
	output[outputIndex] = '\0';

	return std::string(output);

}

Problem::Problem(std::string src) {

	if (src[0] == '*') {
		int totalTasks = INT32_MAX;
		std::istringstream stream{ src };
		std::string line;
		std::vector<std::string> parts;
		// These bools identify what element is being parsed
		int level = -1;
		int lcode = -1;
		int countTasks = 0;
		int countResources = 0;
		// Let's start parsing
		while (std::getline(stream, line)) {
			parts.clear();
			if (stringHasPrefix("*", line)) { level++; lcode = -1; continue; }
			if (level == 1) {
				lcode++;
				if (lcode == 1) {
					split(remove_extra_whitespaces(line), ':', parts);
					//remove_if(parts[1].begin(), parts[1].end(), isspace);
					countTasks = std::min(std::stof(parts[1].c_str()), (float)totalTasks);
					for (unsigned i = 0; i < countTasks; i++) {
						tasks[Convert(i + 1)] = (Task(Convert(i + 1), 0.0));
					}
				}
				if (lcode == 4) {
					split(remove_extra_whitespaces(line), ' ', parts);
					//remove_if(parts[1].begin(), parts[1].end(), isspace);
					countResources = std::stof(parts[3].c_str());
					for (unsigned i = 0; i < countResources; i++) {
						resources["R" + Convert(i + 1)] = (Resource("R" + Convert(i + 1), 0.0));
					}
				}
			}
			if (level == 3) {
				lcode++;
				if (lcode >= 2) {
					split(remove_extra_whitespaces(line), ' ', parts);
					if (parts.empty()) { continue; }
					if (std::stof(parts[0].c_str()) > countTasks) { continue; }
					for (unsigned i = 0; i < std::stof(parts[2].c_str()); i++) {
						if (tasks.find(parts[3 + i]) != tasks.end()) {
							tasks[parts[0]].addNextLink(&tasks[parts[3 + i]]);
							tasks[parts[3 + i]].addPrevLink(&tasks[parts[0]]);
						}
					}
				}
			}
			if (level == 4) {
				lcode++;
				if (lcode >= 3) {
					split(remove_extra_whitespaces(line), ' ', parts);
					if (parts.empty()) { continue; }
					if (std::stof(parts[0].c_str()) > countTasks) { continue; }
					tasks[parts[0]].setDuration(std::stof(parts[2].c_str()));
					for (unsigned i = 0; i < countResources; i++) {
						if (parts.at(3 + i) != "0") {
							assignments.push_back(Assignment(&resources.at("R" + Convert(i + 1)), &tasks[parts[0]], std::stof(parts.at(3 + i).c_str())));
						}
					}
				}
			}
			if (level == 5) {
				lcode++;
				if (lcode == 2) {
					split(remove_extra_whitespaces(line), ' ', parts);
					for (unsigned i = 0; i < countResources; i++) {
						resources["R" + Convert(i + 1)].quantity = std::stof(parts[i].c_str());
					}
				}
			}

		}

		for (size_t i = 0; i < assignments.size(); i++) {
			Assignment*a = &assignments[i];
			a->task->assignments.push_back(a);
		}
		float x = 0.0;

	} else {

		std::istringstream stream{ src };
		std::string line;
		// These bools identify what element is being parsed
		bool readingProblem = false;
		bool readingTasks = false;
		bool readingLinks = false;
		bool readingResources = false;
		bool readingAssignments = false;
		// Let's start parsing
		while (std::getline(stream, line)) {
			std::vector<std::string> parts;
			split(line, ' ', parts);
			if (stringHasPrefix("#", line)) {
				continue;
			}
			if (line == "") {
				continue;
			}
			if (!readingProblem && stringHasPrefix("problem", line)) {
				name = parts.at(1);
				readingProblem = true;
			}
			if (readingProblem && stringHasPrefix("end_problem", line)) { readingProblem = false; continue; }
			if (!readingTasks && stringHasPrefix("tasks", line)) { readingTasks = true; continue; }
			if (readingTasks && stringHasPrefix("end_tasks", line)) {
				std::map<std::string, Task> tmp;
				tmp.insert(tasks.rbegin(), tasks.rend());
				tasks = tmp;
				readingTasks = false; continue;
			}
			if (!readingLinks && stringHasPrefix("links", line)) { readingLinks = true; continue; }
			if (readingLinks && stringHasPrefix("end_links", line)) { readingLinks = false; continue; }
			if (!readingResources && stringHasPrefix("resources", line)) { readingResources = true; continue; }
			if (readingResources && stringHasPrefix("end_resources", line)) {
				std::map<std::string, Resource> tmp2;
				tmp2.insert(resources.rbegin(), resources.rend());
				resources = tmp2;
				readingResources = false; continue;
			}
			if (!readingAssignments && stringHasPrefix("assignments", line)) { readingAssignments = true; continue; }
			if (readingAssignments && stringHasPrefix("end_assignments", line)) { readingAssignments = false; continue; }

			if (readingProblem) {
				if (readingTasks) {
					if (parts.size() < 2) {
						error = true; msg += "INCORRECT FORMAT: " + line + "\n"; continue;
					}
					if (tasks.count(parts.at(0)) > 0) {
						error = true; msg += "REWRITING TASK ON: " + line + "\n"; continue;
					}
					if (!Problem::isFloat(parts.at(1))) {
						error = true; msg += "INCORRECT FORMAT: " + line + "\n"; continue;
					}
					//std::cout << "willBeAdded: " + line + "\n";
					if (tasks.size() <= INT32_MAX) {
						tasks[parts.at(0)] = Task(parts.at(0), std::stof(parts.at(1).c_str()));
					}
					//tasks[parts.at(0)] = Task(parts.at(0), std::stof(parts.at(1).c_str()));
				}
				if (readingLinks) {
					if ((tasks.count(parts.at(0)) > 0) && (tasks.count(parts.at(1)) > 0)) {
						countLinks++;
						tasks[parts.at(1)].addPrevLink(&tasks[parts.at(0)]);
						tasks[parts.at(0)].addNextLink(&tasks[parts.at(1)]);
					}
					else { error = true; msg += "UNKNOWN TASKS AT LINK: " + line + "\n"; continue; }
				}
				if (readingResources) {
					if (parts.size() < 2) {
						error = true; msg += "INCORRECT FORMAT: " + line + "\n"; continue;
					}
					if (resources.count(parts.at(0)) > 0) {
						error = true; msg += "REWRITING RESOURCE ON: " + line + "\n"; continue;
					}
					if (!Problem::isFloat(parts.at(1))) {
						error = true; msg += "INCORRECT FORMAT: " + line + "\n"; continue;
					}
					
					resources[parts.at(0)] = Resource(parts.at(0), std::stof(parts.at(1).c_str()));
				}
				if (readingAssignments) {
					if (parts.size() < 3) {
						error = true; msg += "INCORRECT FORMAT: " + line + "\n"; continue;
					}
					if ((resources.count(parts.at(0)) > 0) && (tasks.count(parts.at(1)) > 0)) {
						if (tasks.count(parts.at(1)) > 0) {
							assignments.push_back(Assignment(&resources.at(parts.at(0)), &tasks.at(parts.at(1)), std::stof(parts.at(2).c_str())));
						}
						//(resources[parts.at(0)]).assignments.push_back(&assignments.at(assignments.size() - 1));
						//(tasks.at(parts.at(1))).assignments.push_back(assignments.size() - 1);
					}
					else { error = true; msg += "UNKNOWN PAIR RESOURCE-TASK AT ASSIGNMENT: " + line + "\n"; continue; }
				}
			}
		}

		for (size_t i = 0; i < assignments.size(); i++) {
			Assignment*a = &assignments[i];
			a->task->assignments.push_back(a);
		}

		if (readingProblem) { error = true; msg += "PARSE ERROR: END_PROBLEM NOT FOUND\n"; }
		if (readingTasks) { error = true; msg += "PARSE ERROR: END_TASKS NOT FOUND\n"; }
		if (readingLinks) { error = true; msg += "PARSE ERROR: END_LINKS NOT FOUND\n"; }
		if (readingResources) { error = true; msg += "PARSE ERROR: END_RESOURCES NOT FOUND\n"; }
		if (readingAssignments) { error = true; msg += "PARSE ERROR: END_ASSIGNMENTS NOT FOUND\n"; }
	}
}

std::string Problem::dump() {
	std::string r = "";
	r += "PROBLEM " + name + '\n';
	r += "STATISTICS \n";
	r += "TASKS " + Problem::Convert(tasks.size()) + '\n';
	r += "LINKS " + Problem::Convert(countLinks) + '\n';
	r += "RESOURCES " + Problem::Convert(resources.size()) + '\n';
	r += "ASSIGNMENTS " + Problem::Convert(assignments.size()) + '\n';
	r += "CPU " + Problem::Convert(cpu_time) + '\n';
	r += "END_STATISTICS \n";
	if (notsolved) {
		r += "ERRORS \n";
		r += msg;
		r += "END_ERRORS \n";
	}
	else {
		r += "SCHEDULE \n";
		r += "name s-f es-ef ls-lf prev next\n";
		for (auto &y : tasks) {
			Task & x = y.second;
			r += x.getName() + " " + Problem::Convert(x.start()) + "-" + Problem::Convert(x.finish()) + " " + Problem::Convert(x.earlyStart()) + "-" + Problem::Convert(x.earlyFinish()) + " " + Problem::Convert(x.lateStart()) + "-" + Problem::Convert(x.lateFinish()) + " " + Problem::Convert(x.total_prev) + " " + Problem::Convert(x.total_next) + '\n';
		}
		r += "END_SCHEDULE \n";

		if (error) {
			r += "ERRORS \n";
			r += msg;
			r += "END_ERRORS \n";
		}
	}

	return r;
}

std::string Problem::definition()
{
	std::string r = "";
	r += "Problem Definition\n";
	r += "Tasks\n";
	for (auto &i : tasks) {
		r += i.first + ": ";
		for (auto &j : i.second.prev) {
			r += j->getName() + " ";
		}
		r += "| ";
		for (auto &j : i.second.next) {
			r += j->getName() + " ";
		}
		r += "| ";
		for (auto &j : i.second.assignments) {
			r += j->resource->name + " (" + Convert(j->quantity) + ") ";
		}
		r += "\n";
	}
	return r;
}

// Parsing & Printing
bool Problem::stringHasPrefix(std::string p, std::string str) {
	//std::cout << str;
	/*
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::transform(p.begin(), p.end(), p.begin(), ::tolower);
	auto res = std::mismatch(p.begin(), p.end(), str.begin());
	return res.first == p.end(); */
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::transform(p.begin(), p.end(), p.begin(), ::tolower);
	/*std::cout << p + " " + str + "\n";
	if (str.substr(0, p.size()) == p) {
	std::cout << "true\n";
	}*/
	return str.substr(0, p.size()) == p;
}

std::vector<std::string> &Problem::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s + ' ');
	std::string item;
	while (std::getline(ss, item, delim)) { elems.push_back(item); }
	if (elems.size() >= 1) { if (elems[0] == "") { elems.erase(elems.begin() + 0); } }
	return elems;
}

std::string Problem::Convert(float number) {
	std::ostringstream buff;
	buff << number;
	return buff.str();
}

#include <sstream>
bool Problem::isFloat(std::string myString) {
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
							   // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}