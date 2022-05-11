#ifndef CMM_UTILS_H
#define CMM_UTILS_H

#include <dirent.h>
#include <signal.h>
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

#include "C--Config.h"
#include "C--Script.h"
#include "Flag.h"
#include "FlagSet.h"
#include "SourceDiff.h"
#include "SourceFiles.h"
#include "SourceSet.h"
#include "SystemRequirements.h"
#include "colors.h"
#include "utils.h"

using namespace std;

const regex INCLUDE_REGEX("^\\s*(?:#|%:)\\s*include \"(.*)\"\\s*"),
	MAIN_REGEX(
		"int\\s+main\\s*\\(\\s*(int\\s+[a-zA-Z_][a-zA-Z_0-9]*|int\\s+[a-zA-Z_][a-zA-Z_0-9]*,\\s*(char\\*\\*\\s+[a-zA-Z_][a-zA-Z_0-9]*|char\\*"
		"\\s+[a-zA-Z_][a-zA-Z_0-9]*\\[\\d*\\]))?\\s*\\)"),
	CATCH_REGEX("\\s*(?:#|%:)define\\s+CATCH_CONFIG_MAIN"), CONFIG_LINE_REGEX("^([a-z\\-]+): (.+)$"), CONFIG_SCRIPT_REGEX("^([a-zA-Z0-9_\\-]+):$"),
	CONFIG_SCRIPT_LINE_REGEX("^(?:\t| {4}| {2})([a-z\\-]+): (.+)$");
const Flag OUTPUT_FLAG = {"--output", {"-o"}, FlagType::NORMAL}, FOLDER_FLAG = {"--folder", {"-f"}, FlagType::NORMAL},
		   ARGS_FLAG = {"--args", {"-a"}, FlagType::COLLATING}, RAW_FLAGS_FLAG = {"--raw-flags", {"-r"}, FlagType::COLLATING},
		   VALGRIND_FLAGS_FLAG = {"--valgrind-flags", {"-v"}, FlagType::COLLATING}, GDB_FLAGS_FLAG = {"--gdb-flags", {"-g"}, FlagType::COLLATING},
		   WATCH_FLAG = {"--watch", {"-w"}, FlagType::BOOLEAN}, DEBUG_FLAG = {"--debug", {"-d"}, FlagType::BOOLEAN},
		   IGNORE_MOLD_FLAG = {"--ignore-mold", {"-m"}, FlagType::BOOLEAN}, IGNORE_LLD_FLAG = {"--ignore-lld", {"-l"}, FlagType::BOOLEAN};
const FlagSet CMM_FLAGS = {OUTPUT_FLAG,	   FOLDER_FLAG, ARGS_FLAG,	RAW_FLAGS_FLAG,	  VALGRIND_FLAGS_FLAG,
						   GDB_FLAGS_FLAG, WATCH_FLAG,	DEBUG_FLAG, IGNORE_MOLD_FLAG, IGNORE_LLD_FLAG};

enum CompileModes { TO_OBJECT, TO_EXECUTABLE };

// Pre: mainFile must be a normalized file name
SourceSet generateSources(const string& mainPath);

map<string, set<string>> generateDependencyMap();

void findHeaders(const string& filePath, FileSet<Header>& headersVisited);

FileSet<Implementation> findDependents(const Header& headerFile, const FileSet<Implementation>& ignore, const map<string, set<string>>& dependencyMap);

string buildCommand(const string& files, const string& outputFolder, const string& outputFile, const string& rawFlags, CompileModes mode, bool mold, bool lld,
					bool debug = false, bool gcov = false);

// Returns a path to the resulting executable.
string directCompile(const SourceSet& sources, const map<Flag, string>& args, const SystemRequirements& sys, bool debug = false, bool gcov = false);

// Pre: file must be a normalized file name, and bin/.objects directory must exist
void compileToObject(const string& file, const map<Flag, string>& args, const SystemRequirements& sys, bool debug = false, bool gcov = false);

// mainFile isn't really used, it's just for determining the output file name if not explicitly set
string compileObjects(const string& mainFile, const map<Flag, string>& args, const SystemRequirements& sys, bool debug = false, bool gcov = false);

SourceDiff reconcileSources(const int fileWatcher, const SourceSet& oldSources, const SourceSet& newSources, const hash<string>& hash,
							map<int, string>& watchDescriptorToPath, map<string, int>& pathToWatchDescriptor, map<string, size_t>& lastContents);

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& initialCompile,
				  const function<void(const SourceDiff&, const string&, const SourceSet&)>& onChange);

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& initialCompile,
				  const function<void(const SourceDiff&, const string&)>& onChange);

// Only gcc 12.1.0 and later support -fuse-ld=mold, otherwise use -B
bool gccVersionGood();

SystemRequirements findSystemRequirements();

Config parseConfig(const string& path);

// Merge the options defined by config into the arguments before command processing
void mergeConfig(map<Flag, string>& args, const Config& config);

// Merge the options defined by config into the arguments before command processing
void mergeScriptConfig(map<Flag, string>& args, const Script& script);

#endif