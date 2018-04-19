// ResearchTask.cpp: 
//
#pragma once
#include "stdafx.h"

#include <iostream>
#include "string"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>

#include <ctime>

#include <comutil.h>  
#include <stdio.h>  
#pragma comment(lib, "comsuppw.lib")  
#pragma comment(lib, "kernel32.lib")  

#define NOMINMAX
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#include "Solver.h"


std::map<std::string, float> getOptSolutions(std::string dir, std::string fn, int tasksNum)
{
	std::map<std::string, float> res;
	std::string nm = dir + "\\" + fn;
	std::ifstream file(nm);
	std::string line;
	int param, inst;
	float mspan;
	while (std::getline(file, line))
	{
		//std::cout << "Something\n";
		std::stringstream ss;
		ss << line;
		ss >> param >> inst >> mspan;
		std::string name = "j" + std::to_string(tasksNum) + std::to_string(param) + "_" + std::to_string(inst) + ".sm";
		res[name] = mspan;
	}
	return res;
}

// File read&write functions
std::string readFile(std::string fn) {
	std::ifstream file(fn);
	std::string line, src;
	while (std::getline(file, line)) { src += line; src += '\n'; }
	return src;
}

std::vector<std::string> readDirectory(STRSAFE_LPWSTR dir) {

	std::vector<std::string> r;

	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, dir);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		return std::vector<std::string>();
	}

	// List all the files in the directory with some info about them.

	do {
		std::wstring ws(ffd.cFileName);
		std::string str(ws.begin(), ws.end());
		r.push_back(str);
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return r;
}

std::wstring s2ws(const std::string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void OutputResults(std::vector<std::string> fns, std::string dir)
{
	std::vector<int> nums = { 2, 3, 9, 10, 11, 12, 15, 16, 21 };
	std::map<std::pair<int, int>, int> wins;
	std::map<std::pair<int, int>, int> hits;
	std::map<std::pair<int, int>, float> avg_loss;
	for (auto i = 0; i <= 4; i++)
	{
		wins[std::pair<int, int>(0, i)] = 0;
		hits[std::pair<int, int>(0, i)] = 0;
		avg_loss[std::pair<int, int>(0, i)] = 0.0;
		for (auto j : nums)
		{
			wins[std::pair<int, int>(j, i)] = 0;
			hits[std::pair<int, int>(j, i)] = 0;
			avg_loss[std::pair<int, int>(j, i)] = 0.0;
		}
	}
	std::ofstream output;
	output.open(dir + "\\output.txt");
	int bugs = 0;
	std::map<std::string, float> optTimes = getOptSolutions(dir, "optSol.txt", 30);
	for (unsigned i = 2; i < fns.size(); i++) {
		float bestTime = INT32_MAX;
		std::map<std::pair<int, int>, float> tim;
		

		std::string src = readFile(dir + "\\" + fns.at(i));
		std::cout << fns.at(i) + ":\n";

		float optTime = optTimes[fns.at(i)];
		
		int ch;
		for (int n = 0; n <= 2; n++)
		{
			Problem * t1 = new Problem(src);
			MixedSolver s1(t1, NULL, n);

			tim[std::pair<int, int>(0, n)] = s1.solve();
			//std::cout << "In here\n";
			ch = s1.CheckFeasibility();
			if (ch > 0) {
				std::cout << "ERROR " << ch << '\n'; //output.close(); exit(0);
				bugs++;
			}
			std::cout << "Mixed with critical path and depth " << n << ": " << tim[std::pair<int, int>(0, n)] << '\n';
			output << fns.at(i) << ' ' << tim[std::pair<int, int>(0, n)] << ' ' << 0 << ' ' << n << '\n';
			if (tim[std::pair<int, int>(0, n)] == optTime) hits[std::pair<int, int>(0, n)]++;
			avg_loss[std::pair<int, int>(0, n)] += tim[std::pair<int, int>(0, n)] / optTime;
			if (tim[std::pair<int, int>(0, n)]  < bestTime) { bestTime = tim[std::pair<int, int>(0, n)]; }
			if (tim[std::pair<int, int>(0, n)]  < optTime) {
				std::cout << "Error!!!!!!!\n"; exit(0);
			}
		}
		//continue;
		std::cout << fns.at(i) << ' ' << optTime << ' ' << "Optimal Time" << '\n';
		output << fns.at(i) << ' ' << optTime << ' ' << "Optimal Time" << '\n';
		//continue;
		for (auto h : nums) for (int n = 0; n <= 4; n++)
		{
			Problem * t1 = new Problem(src);
			MixedSolver s1(t1, new SimpleHeuristics(h), n);

			tim[std::pair<int, int>(h, n)] = s1.solve();
			//std::cout << "In here\n";
			ch = s1.CheckFeasibility();
			if (ch > 0) {
				std::cout << "ERROR " << ch << '\n'; //output.close(); exit(0);
				bugs++;
			}
			std::cout << "Mixed with heur " << h << " and depth " << n << ": " << tim[std::pair<int, int>(h, n)] << '\n';
			output << fns.at(i) << ' ' << tim[std::pair<int, int>(h, n)] << ' ' << h << ' ' << n << '\n';
			if (tim[std::pair<int, int>(h, n)] == optTime) hits[std::pair<int, int>(h, n)]++;
			avg_loss[std::pair<int, int>(h, n)] += tim[std::pair<int, int>(h, n)] / optTime;
			if (tim[std::pair<int, int>(h, n)] < bestTime) { bestTime = tim[std::pair<int, int>(h, n)]; }
			if (tim[std::pair<int, int>(h, n)] < optTime) {
				std::cout << "Error!!!!!!!\n"; exit(0);
			}
		}
		//output << fns.at(i) << " Optimal time: " << optTime << '\n';
		//if (i == 3)	exit(0);
		for (auto t : tim) if (t.second == bestTime) wins[t.first]++;
		std::cout << bugs << '\n';
		std::cout << "So far...\n";
		for (auto i = 0; i <= 4; i++)
		{
			std::cout << "0 " << i << ": wins = " << wins[std::pair<int, int>(0, i)] << " hits = " << hits[std::pair<int, int>(0, i)] << " avgloss = " << avg_loss[std::pair<int, int>(0, i)] << '\n';
			for (auto j : nums)
			{
				std::cout << j << ' ' << i << ": wins = " << wins[std::pair<int, int>(j, i)] << " hits = " << hits[std::pair<int, int>(j, i)] << " avgloss = " << avg_loss[std::pair<int, int>(j, i)] << '\n';
				wins[std::pair<int, int>(j, i)];
				hits[std::pair<int, int>(j, i)];
				avg_loss[std::pair<int, int>(j, i)];
		continue;
		Problem * t2 = new Problem(src);
		ExactSolver s2(t2, new SimpleHeuristics(5));
		s2.solve();
		std::cout << "ExactSolver: " << s2.solution.second << '\n';

		ch = s2.CheckFeasibility();
		if (ch > 0) {
			std::cout << "ERROR " << ch << '\n'; //output.close(); exit(0);
		}

			}
		}
		
	}
	for (auto i = 0; i <= 4; i++)
	{
		output << "0 " << i << ": wins = " << wins[std::pair<int, int>(0, i)] << " hits = " << hits[std::pair<int, int>(0, i)] << " avgloss = " << avg_loss[std::pair<int, int>(0, i)] << '\n';
		for (auto j : nums)
		{
			output << j << ' ' << i << ": wins = " << wins[std::pair<int, int>(j, i)] << " hits = " << hits[std::pair<int, int>(j, i)] << " avgloss = " << avg_loss[std::pair<int, int>(j, i)] << '\n';
			wins[std::pair<int, int>(j, i)];
			hits[std::pair<int, int>(j, i)];
			avg_loss[std::pair<int, int>(j, i)];
		}
	}
	output.close();
	return;
}

int main()
{
	std::cout << "Specify problem directory name:" << "\n";
	std::string dir; std::getline(std::cin, dir);
	if (dir == "") { dir = "ResearchTests30"; }
	std::cout << dir << '\n';
	BSTR lp = _com_util::ConvertStringToBSTR(dir.c_str());
	LPWSTR bb = lp;
	std::vector<std::string> fns = readDirectory(bb);
	
	OutputResults(fns, dir);
	return 0;
}

