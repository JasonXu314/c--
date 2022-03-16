#ifndef CMM_UTILS_H
#define CMM_UTILS_H

#include <dirent.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "Flag.h"
#include "FlagSet.h"
#include "SourceSet.h"
#include "utils.h"

using namespace std;

const regex INCLUDE_REGEX("^\\s*#include \"(.*)\""),
	MAIN_REGEX(
		"int\\s+main\\s*\\(\\s*(int\\s+[a-zA-Z_][a-zA-Z_0-9]*|int\\s+[a-zA-Z_][a-zA-Z_0-9]*,\\s*(char\\*\\*\\s+[a-zA-Z_][a-zA-Z_0-9]*|char\\*"
		"\\s+[a-zA-Z_][a-zA-Z_0-9]*\\[\\d*\\]))?\\s*\\)");

const Flag OUTPUT_FLAG = {"--output", {"-o"}, FlagType::NORMAL}, FOLDER_FLAG = {"--folder", {"-f"}, FlagType::NORMAL},
		   ARGS_FLAG = {"--args", {"-a"}, FlagType::COLLATING}, RAW_FLAGS_FLAG = {"--raw-flags", {"-r"}, FlagType::COLLATING},
		   VALGRIND_FLAGS_FLAG = {"--valgrind-flags", {"-v"}, FlagType::COLLATING}, GDB_FLAGS_FLAG = {"--gdb-flags", {"-g"}, FlagType::COLLATING},
		   WATCH_FLAG = {"--watch", {"-w"}, FlagType::BOOLEAN};

const FlagSet CMM_FLAGS = {OUTPUT_FLAG, FOLDER_FLAG, ARGS_FLAG, RAW_FLAGS_FLAG, VALGRIND_FLAGS_FLAG, GDB_FLAGS_FLAG, WATCH_FLAG};

void printHelpMessage(const vector<string>& argsList);

void compileCommand(const string& file, const map<Flag, string>& args);

// returns a path to the resulting executable.
string compile(const SourceSet& sources, const map<Flag, string>& args, bool debug = false);

void compileAndRun(const string& file, const map<Flag, string>& args);

void compileAndDebug(const string& file, const map<Flag, string>& args);

void compileAndValgrind(const string& file, const map<Flag, string>& args);

SourceSet generateSources(const string& mainFile);

map<string, set<string>> generateDependencyMap();

void findHeaders(const string& fileName, set<string>& headersVisited);

set<string> findDependents(const string& headerFile, const set<string>& ignore, const map<string, set<string>>& dependencyMap);

void reconcileSources(const int fileWatcher, const SourceSet& oldSources, const SourceSet& newSources, const hash<string>& hash,
					  map<int, string>& watchDescriptorToPath, map<string, int>& pathToWatchDescriptor, map<string, size_t>& lastContents);

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& onChange);

#endif