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

const regex INCLUDE_REGEX("^\\s*#include \"(.*)\""),
	MAIN_REGEX(
		"int\\s+main\\s*\\(\\s*(int\\s+[a-zA-Z_][a-zA-Z_0-9]*|int\\s+[a-zA-Z_][a-zA-Z_0-9]*,\\s*(char\\*\\*\\s+[a-zA-Z_][a-zA-Z_0-9]*|char\\*"
		"\\s+[a-zA-Z_][a-zA-Z_0-9]*\\[\\d*\\]))?\\s*\\)");

const Flag OUTPUT_FLAG = {"--output", {"-o"}, false}, FOLDER_FLAG = {"--folder", {"-f"}, false}, ARGS_FLAG = {"--args", {"-a"}, true},
		   RAW_FLAGS_FLAG = {"--raw-flags", {"-r"}, true}, VALGRIND_FLAGS_FLAG = {"--valgrind-flags", {"-v"}, true},
		   GDB_FLAGS_FLAG = {"--gdb-flags", {"-g"}, true};

const FlagSet CMM_FLAGS = {OUTPUT_FLAG, FOLDER_FLAG, ARGS_FLAG, RAW_FLAGS_FLAG, VALGRIND_FLAGS_FLAG, GDB_FLAGS_FLAG};

void printHelpMessage(const vector<string>& argsList);

// returns a path to the resulting executable.
string compileFile(const string& file, const map<Flag, string>& args, bool debug = false);

void compileAndRun(const string& file, const map<Flag, string>& args);

void compileAndDebug(const string& file, const map<Flag, string>& args);

void compileAndValgrind(const string& file, const map<Flag, string>& args);

set<string> generateSources(const string& mainFile);

map<string, set<string>> generateDependencyMap();

void findHeaders(const string& fileName, set<string>& headersVisited);

set<string> findDependents(const string& headerFile, const set<string>& ignore, const map<string, set<string>>& dependencyMap);

#endif