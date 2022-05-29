#include "g--_utils.h"

#include "SourceFiles.h"
#include "utils.h"

SourceSet generateSources(const string& mainPath) {
	SourceSet out = {{stripDirectories(mainPath), mainPath}, {}, {}};
	map<string, FileSet<Header>> dependencyMap = generateDependencyMap();

	out.sources.insert(out.main);

	for (const Header& header : dependencyMap[mainPath]) {
		if (!out.headers.contains(header)) {
			findHeaders(header.path, out.headers, ".");
		}
	}

	for (const Header& headerFile : out.headers) {
		FileSet<Implementation> dependents = findDependents(headerFile, out.sources, dependencyMap);

		for (const Implementation& dependent : dependents) {
			out.sources.insert(dependent);
		}
	}

	return out;
}

map<string, FileSet<Header>> generateDependencyMap() {
	map<string, FileSet<Header>> out;
	set<string> files = readDir(".");

	for (const string& file : files) {
		smatch foldersMatch;

		if (regex_match(file, foldersMatch, regex("^(.+/)?.*\\.cpp$"))) {
			if (!out.count(file)) {
				fstream in(file);
				string line;
				smatch includeMatch;

				while (getline(in, line)) {
					if (regex_match(line, includeMatch, INCLUDE_REGEX)) {
						string includeFile = resolvePath(foldersMatch[1].str() + includeMatch[1].str());
						FileSet<Header> dependencies;

						findHeaders(includeFile, dependencies, ".");

						out.insert({file, dependencies});
					}
				}

				in.close();
			}
		}
	}

	return out;
}

FileSet<Implementation> findDependents(const Header& headerFile, const FileSet<Implementation>& ignore, const map<string, FileSet<Header>>& dependencyMap) {
	FileSet<Implementation> out;

	for (const auto& entry : dependencyMap) {
		string cppFile = entry.first;
		FileSet<Header> headers = entry.second;

		if (ignore.contains(cppFile)) {
			continue;
		} else if (headers.contains(headerFile.path)) {
			string fileContents = readFile(cppFile);

			if (!regex_search(fileContents, MAIN_REGEX) && !regex_search(fileContents, CATCH_REGEX)) {
				out.insert({stripDirectories(cppFile), cppFile});
			}
		}
	}

	return out;
}

void findHeaders(const string& filePath, FileSet<Header>& headersVisited, const string& prevPath) {
	if (headersVisited.contains(resolvePath(prevPath + "/" + filePath))) {
		return;
	} else {
		headersVisited.insert({stripDirectories(filePath), resolvePath(prevPath + "/" + filePath)});
	}

	fstream in(filePath);
	string line;
	smatch match;

	while (getline(in, line)) {
		if (regex_match(line, match, INCLUDE_REGEX)) {
			string includeFile = resolvePath(match[1]);

			if (!headersVisited.contains(includeFile)) {
				findHeaders(includeFile, headersVisited, getParentDirectory(filePath));
			}
		}
	}

	in.close();
}

string buildCommand(const string& files, const string& outputFolder, const string& outputFile, const string& rawFlags, CompileModes mode, bool mold, bool lld,
					bool debug, bool gcov) {
	string cmd = "g++ -Wall -W -pedantic-errors ";

	if (mode == CompileModes::TO_OBJECT) {
		cmd += "-c ";
	}

	if (!gcov) {
		if (debug) {
			cmd += "-g ";
		} else {
			cmd += "-s ";
		}
	} else {
		cmd += "-fprofile-arcs -ftest-coverage ";
	}

	if (rawFlags.length() > 0) {
		cmd += rawFlags + " ";
	}

	if (mold) {
		if (gccVersionGood()) {
			cmd += "-fuse-ld=mold ";
		} else {
			cmd += "-B/usr/local/libexec/mold ";
		}
	} else if (lld) {
		cmd += "-fuse-ld=lld ";
	}

	cmd += files + " -o " + outputFolder + "/" + outputFile;

	return cmd;
}

string directCompile(const SourceSet& sources, const map<Flag, string>& args, const SystemRequirements& sys, bool debug, bool gcov) {
	string outputFile = args.count(OUTPUT_FLAG) ? args.at(OUTPUT_FLAG) : (stripExtension(stripDirectories(sources.main.path)) + (debug ? "_debug" : "")),
		   outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) : "bin", rawFlags = args.count(RAW_FLAGS_FLAG) ? args.at(RAW_FLAGS_FLAG) : "",
		   sourcesList;

	for (const Implementation& source : sources.sources) {
		sourcesList += source.path + " ";
	}

	string cmd = buildCommand(sourcesList, outputFolder, outputFile, rawFlags, CompileModes::TO_EXECUTABLE, sys.mold.present && !args.count(IGNORE_MOLD_FLAG),
							  sys.lld.present && !args.count(IGNORE_LLD_FLAG), debug, gcov);

	struct stat dirInfo;
	if (stat(outputFolder.c_str(), &dirInfo) != 0) {
		string mkdirCmd = "mkdir " + outputFolder;
		bool statusCode = system(mkdirCmd.c_str());

		if (statusCode != 0) {
			throw runtime_error(BHRED "Error creating output folder: " BWHT + outputFolder + reset);
		}
	}

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}

	return "./" + outputFolder + "/" + outputFile;
}

void compileToObject(const string& file, const map<Flag, string>& args, const SystemRequirements& sys, bool debug, bool gcov) {
	string outputFile = stripExtension(replace(file, "/", "_")) + ".o",
		   outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) + "/.objects" : "bin/.objects",
		   rawFlags = args.count(RAW_FLAGS_FLAG) ? args.at(RAW_FLAGS_FLAG) : "";

	string cmd = buildCommand(file, outputFolder, outputFile, rawFlags, CompileModes::TO_OBJECT, sys.mold.present && !args.count(IGNORE_MOLD_FLAG),
							  sys.lld.present && !args.count(IGNORE_LLD_FLAG), debug, gcov);

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}
}

string compileObjects(const string& mainFile, const map<Flag, string>& args, const SystemRequirements& sys, bool debug, bool gcov) {
	string outputFile = args.count(OUTPUT_FLAG) ? args.at(OUTPUT_FLAG) : (stripExtension(stripDirectories(mainFile)) + (debug ? "_debug" : "")),
		   outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) : "bin", rawFlags = args.count(RAW_FLAGS_FLAG) ? args.at(RAW_FLAGS_FLAG) : "";

	string cmd = buildCommand(outputFolder + "/.objects/*.o", outputFolder, outputFile, rawFlags, CompileModes::TO_EXECUTABLE,
							  sys.mold.present && !args.count(IGNORE_MOLD_FLAG), sys.lld.present && !args.count(IGNORE_LLD_FLAG), debug, gcov);

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}

	return "./" + outputFolder + "/" + outputFile;
}

SourceDiff reconcileSources(const int fileWatcher, const SourceSet& oldSources, const SourceSet& newSources, const hash<string>& hash,
							map<int, string>& watchDescriptorToPath, map<string, int>& pathToWatchDescriptor, map<string, size_t>& lastContents) {
	SourceDiff out;

	for (const Implementation& source : newSources.sources) {
		if (!oldSources.sources.contains(source)) {
			int watchDescriptor = inotify_add_watch(fileWatcher, source.path.c_str(), IN_MODIFY);
			watchDescriptorToPath.insert({watchDescriptor, source.path});
			pathToWatchDescriptor.insert({source.path, watchDescriptor});
			lastContents[source.path] = hash(stripWhitespace(readFile(source.path)));
			out.added.insert(source.path);
		}
	}

	for (const Implementation& source : oldSources.sources) {
		if (!newSources.sources.contains(source.path)) {
			inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source.path]);
			watchDescriptorToPath.erase(pathToWatchDescriptor[source.path]);
			pathToWatchDescriptor.erase(source.path);
			out.removed.insert(source.path);
		}
	}

	for (const Header& header : newSources.headers) {
		if (!oldSources.headers.contains(header)) {
			int watchDescriptor = inotify_add_watch(fileWatcher, header.path.c_str(), IN_MODIFY);
			watchDescriptorToPath.insert({watchDescriptor, header.path});
			pathToWatchDescriptor.insert({header.path, watchDescriptor});
			lastContents[header.path] = hash(stripWhitespace(readFile(header.path)));
		}
	}

	for (const Header& header : oldSources.headers) {
		if (!newSources.headers.contains(header)) {
			inotify_rm_watch(fileWatcher, pathToWatchDescriptor[header.path]);
			watchDescriptorToPath.erase(pathToWatchDescriptor[header.path]);
			pathToWatchDescriptor.erase(header.path);
		}
	}

	return out;
}

/** If you're reading this, PLEASE NEVER EVER ACTUALLY DO THIS UNLESS YOU REALLY REALLY KNOW WHAT YOU'RE DOING */
namespace GLOBAL_SIGINT_FN {
function<void()> fn = []() {};
}

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& initialCompile,
				  const function<void(const SourceDiff&, const string&, const SourceSet&)>& onChange) {
	int fileWatcher = inotify_init();
	map<int, string> watchDescriptorToPath;
	map<string, int> pathToWatchDescriptor;
	map<string, size_t> lastContents;
	hash<string> hash;
	SourceSet sources = generateSources(file);
	bool continueWatch = true;

	for (const Implementation& source : sources.sources) {
		int watchDescriptor = inotify_add_watch(fileWatcher, source.path.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, source.path});
		pathToWatchDescriptor.insert({source.path, watchDescriptor});
		lastContents.insert({source.path, hash(stripWhitespace(readFile(source.path)))});
	}

	for (const Header& header : sources.headers) {
		int watchDescriptor = inotify_add_watch(fileWatcher, header.path.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, header.path});
		pathToWatchDescriptor.insert({header.path, watchDescriptor});
		lastContents.insert({header.path, hash(stripWhitespace(readFile(header.path)))});
	}

	initialCompile(sources);

	thread watchThread([&continueWatch, &sources, &fileWatcher, &watchDescriptorToPath, &pathToWatchDescriptor, &hash, &lastContents, &onChange, &file]() {
		inotify_event event;
		SourceDiff diff;
		bool deletePhase = true;

		while (continueWatch) {
			int bytesRead = read(fileWatcher, &event, sizeof(inotify_event) + NAME_MAX + 1);

			if (bytesRead != -1 && (event.mask & IN_MODIFY)) {
				if (!deletePhase) {
					string changedFile = watchDescriptorToPath[event.wd], fileContents = stripWhitespace(readFile(changedFile));
					size_t hashedContents = hash(fileContents);

					if (lastContents[changedFile] != hashedContents) {
						system("clear");
						cout << BWHT "Change to " GRN << changedFile << BWHT " detected" reset ", re-compiling..." << endl;

						SourceSet newSources = generateSources(file);
						diff = reconcileSources(fileWatcher, sources, newSources, hash, watchDescriptorToPath, pathToWatchDescriptor, lastContents);
						sources = newSources;

						onChange(diff, changedFile, sources);

						lastContents[changedFile] = hashedContents;
					} else {
						cout << RED "No effective change" reset " to " BWHT << changedFile << reset ", not compiling" << endl;
					}
				}

				deletePhase = !deletePhase;
			} else if (bytesRead == -1) {
				continueWatch = false;
			}
		}
	});

	GLOBAL_SIGINT_FN::fn = [&continueWatch]() { continueWatch = false; };

	signal(SIGINT, [](int) { GLOBAL_SIGINT_FN::fn(); });

	while (continueWatch) {
		string input;

		cin >> input;

		if (input == "quit" || input == "q") {
			continueWatch = false;
		}
	}

	pthread_kill(watchThread.native_handle(), SIGINT);
	watchThread.join();

	cout << BWHT "Interrupt signal received, cleaning up..." reset << endl;

	for (const Implementation& source : sources.sources) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source.path]);
	}

	for (const Header& header : sources.headers) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[header.path]);
	}
}

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& initialCompile,
				  const function<void(const SourceDiff&, const string&)>& onChange) {
	int fileWatcher = inotify_init();
	map<int, string> watchDescriptorToPath;
	map<string, int> pathToWatchDescriptor;
	map<string, size_t> lastContents;
	hash<string> hash;
	SourceSet sources = generateSources(file);
	bool continueWatch = true;

	for (const Implementation& source : sources.sources) {
		int watchDescriptor = inotify_add_watch(fileWatcher, source.path.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, source.path});
		pathToWatchDescriptor.insert({source.path, watchDescriptor});
		lastContents.insert({source.path, hash(stripWhitespace(readFile(source.path)))});
	}

	for (const Header& header : sources.headers) {
		int watchDescriptor = inotify_add_watch(fileWatcher, header.path.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, header.path});
		pathToWatchDescriptor.insert({header.path, watchDescriptor});
		lastContents.insert({header.path, hash(stripWhitespace(readFile(header.path)))});
	}

	initialCompile(sources);

	thread watchThread([&continueWatch, &sources, &fileWatcher, &watchDescriptorToPath, &pathToWatchDescriptor, &hash, &lastContents, &onChange, &file]() {
		inotify_event event;
		SourceDiff diff;
		bool deletePhase = true;

		while (continueWatch) {
			int bytesRead = read(fileWatcher, &event, sizeof(inotify_event) + NAME_MAX + 1);

			if (bytesRead != -1 && (event.mask & IN_MODIFY)) {
				if (!deletePhase) {
					string changedFile = watchDescriptorToPath[event.wd], fileContents = stripWhitespace(readFile(changedFile));
					size_t hashedContents = hash(fileContents);

					if (lastContents[changedFile] != hashedContents) {
						system("clear");
						cout << BWHT "Change to " GRN << changedFile << BWHT " detected" reset ", re-compiling..." << endl;

						SourceSet newSources = generateSources(file);
						diff = reconcileSources(fileWatcher, sources, newSources, hash, watchDescriptorToPath, pathToWatchDescriptor, lastContents);
						sources = newSources;

						onChange(diff, changedFile);

						lastContents[changedFile] = hashedContents;
					} else {
						cout << RED "No effective change" reset " to " BWHT << changedFile << reset ", not compiling" << endl;
					}
				}

				deletePhase = !deletePhase;
			} else if (bytesRead == -1) {
				continueWatch = false;
			}
		}
	});

	GLOBAL_SIGINT_FN::fn = [&continueWatch]() { continueWatch = false; };

	signal(SIGINT, [](int) { GLOBAL_SIGINT_FN::fn(); });

	while (continueWatch) {
		string input;

		cin >> input;

		if (input == "quit" || input == "q") {
			continueWatch = false;
		}
	}

	pthread_kill(watchThread.native_handle(), SIGINT);
	watchThread.join();

	cout << BWHT "Interrupt signal received, cleaning up..." reset << endl;

	for (const Implementation& source : sources.sources) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source.path]);
	}

	for (const Header& header : sources.headers) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[header.path]);
	}
}

bool gccVersionGood() {
	FILE* fp;
	char str[32];

	fp = popen("g++ --version | grep -o -P '(?<= )[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(?=$)'", "r");
	fgets(str, sizeof(str), fp);

	pclose(fp);

	string versionString(str);

	int majorVersion = stoi(versionString.substr(0, versionString.find(".")));

	return majorVersion >= 12;
}

SystemRequirements findSystemRequirements() {
	SystemRequirements out;

	FILE* fp;
	char str[256];
	int statusCode;

	fp = popen("command -v g++", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.gpp = {false, ""};
	} else {
		string path(str);
		out.gpp = {true, path};
	}

	fp = popen("command -v gdb", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.gdb = {false, ""};
	} else {
		string path(str);
		out.gdb = {true, path};
	}

	fp = popen("command -v valgrind", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.valgrind = {false, ""};
	} else {
		string path(str);
		out.valgrind = {true, path};
	}

	fp = popen("command -v mold", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.mold = {false, ""};
	} else {
		string path(str);
		out.mold = {true, path};
	}

	fp = popen("command -v ld.lld", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.lld = {false, ""};
	} else {
		string path(str);
		out.lld = {true, path};
	}

	fp = popen("command -v gcov", "r");
	fgets(str, sizeof(str), fp);
	statusCode = pclose(fp);

	if (statusCode != 0) {
		out.gcov = {false, ""};
	} else {
		string path(str);
		out.gcov = {true, path};
	}

	return out;
}

Config parseConfig(const string& path) {
	Config out;
	vector<string> lines = split(readFile(path), "\n");

	for (size_t i = 0; i < lines.size(); i++) {
		smatch configLineMatch;

		if (regex_match(lines[i], configLineMatch, CONFIG_LINE_REGEX)) {
			string key = configLineMatch[1].str();
			string value = configLineMatch[2].str();

			if (CMM_FLAGS.contains("--" + key)) {
				out.defaultFlags.insert({CMM_FLAGS.get("--" + key), value});
			} else if (CMM_FLAGS.contains("-" + key)) {
				out.defaultFlags.insert({CMM_FLAGS.get("-" + key), value});
			} else {
				throw runtime_error("Unknown config key: " + key);
			}
		} else if (regex_match(lines[i], configLineMatch, CONFIG_SCRIPT_REGEX)) {
			string scriptName = configLineMatch[1].str();
			Script script;

			for (size_t j = i + 1; j < lines.size(); i = ++j) {
				smatch scriptLineMatch;

				if (regex_match(lines[j], scriptLineMatch, CONFIG_SCRIPT_LINE_REGEX)) {
					string key = scriptLineMatch[1].str();
					string value = scriptLineMatch[2].str();

					if (CMM_FLAGS.contains("--" + key)) {
						script.defaultFlags.insert({CMM_FLAGS.get("--" + key), value});
					} else if (CMM_FLAGS.contains("-" + key)) {
						script.defaultFlags.insert({CMM_FLAGS.get("-" + key), value});
					} else if (key == "main") {	 // put the main file check here to avoid duplicate checks because it's only 1 line
						script.mainFile = value;
					} else if (key == "command") {
						script.command = value;
					} else {
						throw runtime_error("Unknown config key: " + key);
					}
				} else if (lines[j].empty()) {
					break;
				} else {
					throw runtime_error("Invalid script line: " + lines[j] + " (line " + to_string(j + 1) + ")");
				}
			}

			if (script.mainFile == "") {
				throw runtime_error("Script " + scriptName + " has no main file");
			} else {
				out.scripts[scriptName] = script;
			}
		}
	}

	return out;
}

void mergeConfig(map<Flag, string>& args, const Config& config) {
	for (const auto& entry : config.defaultFlags) {
		Flag flag = entry.first;
		string value = entry.second;

		if (flag.flagType == FlagType::NORMAL) {
			if (args.count(flag)) {
				cout << BYEL "Warning: " reset "config flag " BWHT << flag.toString() << BRED " overridden " reset "by command line" << endl;
			} else {
				args[flag] = value;
			}
		} else if (flag.flagType == FlagType::BOOLEAN && value == "true") {
			if (!args.count(flag)) {
				args[flag] = "";
			}
		} else {
			args[flag] += " " + stripWhitespace(value);
		}
	}
}

void mergeScriptConfig(map<Flag, string>& args, const Script& script) {
	for (const auto& entry : script.defaultFlags) {
		Flag flag = entry.first;
		string value = entry.second;

		if (flag.flagType == FlagType::NORMAL) {
			if (args.count(flag)) {
				cout << BYEL "Warning: " reset "script flag " BWHT << flag.toString() << BRED " overridden " reset "by command line" << endl;
			} else {
				args[flag] = value;
			}
		} else if (flag.flagType == FlagType::BOOLEAN && value == "true") {
			if (!args.count(flag)) {
				args[flag] = "";
			}
		} else {
			args[flag] += " " + stripWhitespace(value);
		}
	}
}