#ifndef CMM_UTILS_H
#define CMM_UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "Flag.h"
#include "FlagSet.h"
#include "utils.h"

using namespace std;

const Flag OUTPUT_FLAG = {"--output", {"-o"}}, FOLDER_FLAG = {"--folder", {"-f"}}, ARGS_FLAG = {"--args", {"-a"}}, RAW_FLAGS_FLAG = {"--raw-flags", {"-r"}},
		   VALGRIND_FLAGS_FLAG = {"--valgrind-flags", {"-v"}}, GDB_FLAGS_FLAG = {"--gdb-flags", {"-g"}};

const FlagSet CMM_FLAGS = {OUTPUT_FLAG, FOLDER_FLAG, ARGS_FLAG, RAW_FLAGS_FLAG, VALGRIND_FLAGS_FLAG, GDB_FLAGS_FLAG},
			  COLLATING_FLAGS = {ARGS_FLAG, RAW_FLAGS_FLAG, VALGRIND_FLAGS_FLAG, GDB_FLAGS_FLAG};

void printHelpMessage(const vector<string>& argsList);

// returns a path to the resulting executable.
string compileFile(const string& file, const map<Flag, string>& args, bool debug = false);

void compileAndRun(const string& file, const map<Flag, string>& args);

void compileAndDebug(const string& file, const map<Flag, string>& args);

void compileAndMemcheck(const string& file, const map<Flag, string>& args);

set<string> generateSources(const string& mainFile);

map<string, set<string>> generateDependencyMap();

void findHeaders(const string& fileName, set<string>& headersVisited);

set<string> findDependents(const string& headerFile, const set<string>& ignore, const map<string, set<string>>& dependencyMap);

bool isCollatingFlag(const string& flag);

string getIncludedFile(const string& line);

#endif