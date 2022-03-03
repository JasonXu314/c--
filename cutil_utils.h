#ifndef CUTIL_UTILS_H
#define CUTIL_UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;

const set<string> CUTIL_FLAGS = {"--output", "--folder", "--args", "--raw-flags", "--valgrind-flags", "--gdb-flags", "-o", "-f", "-a", "-r", "-v", "-g"};

void printHelpMessage(const vector<string>& argsList);

// returns a path to the resulting executable.
string compileFile(const vector<string>& argsList, const set<string>& flags, bool debug = false);

void compileAndRun(const vector<string>& argsList, const set<string>& flags);

void compileAndDebug(const vector<string>& argsList, const set<string>& flags);

void compileAndMemcheck(const vector<string>& argsList, const set<string>& flags);

set<string> generateSources(const string& fileName);

map<string, set<string>> generateDependencyMap();

void findHeaders(const string& fileName, set<string>& headersVisited);

set<string> findDependents(const string& headerFile, const set<string>& ignore, const map<string, set<string>>& dependencyMap);

bool isCollatingFlag(const string& flag);

string getIncludedFile(const string& line);

#endif