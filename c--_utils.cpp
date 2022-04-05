#include "c--_utils.h"
#include "SourceFiles.h"
#include "utils.h"

SourceSet generateSources(const string& mainPath) {
	SourceSet out = {{stripDirectories(mainPath), mainPath}, {}, {}};
	fstream in(mainPath);
	string line;
	map<string, set<string>> dependencyMap = generateDependencyMap();

	out.sources.insert(out.main);

	for (const string& header : dependencyMap[mainPath]) {
		if (!out.headers.contains(header)) {
			findHeaders(header, out.headers);
		}
	}

	for (const Header& headerFile : out.headers) {
		FileSet<Implementation> dependents = findDependents(headerFile, out.sources, dependencyMap);

		for (const Implementation& dependent : dependents) {
			out.sources.insert(dependent);
		}
	}

	in.close();

	return out;
}

map<string, set<string>> generateDependencyMap() {
	map<string, set<string>> out;
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

						out[file].insert(includeFile);
					}
				}

				in.close();
			}
		}
	}

	return out;
}

FileSet<Implementation> findDependents(const Header& headerFile, const FileSet<Implementation>& ignore, const map<string, set<string>>& dependencyMap) {
	FileSet<Implementation> out;

	for (const auto& entry : dependencyMap) {
		string cppFile = entry.first;
		set<string> headers = entry.second;

		if (ignore.contains(cppFile)) {
			continue;
		} else if (headers.count(headerFile.path)) {
			string fileContents = readFile(cppFile);

			if (!regex_search(fileContents, MAIN_REGEX)) {
				out.insert({stripDirectories(cppFile), cppFile});
			}
		}
	}

	return out;
}

void findHeaders(const string& filePath, FileSet<Header>& headersVisited) {
	if (headersVisited.contains(filePath)) {
		return;
	} else {
		headersVisited.insert({stripDirectories(filePath), filePath});
	}

	fstream in(filePath);
	string line;
	smatch match;

	while (getline(in, line)) {
		if (regex_match(line, match, INCLUDE_REGEX)) {
			string includeFile = match[1];

			if (!headersVisited.contains(includeFile)) {
				findHeaders(includeFile, headersVisited);
			}
		}
	}

	in.close();
}

string directCompile(const SourceSet& sources, const map<Flag, string>& args, const SystemRequirements& sys, bool debug) {
	string inputFile = sources.main.path, outputFile = stripExtension(stripDirectories(inputFile)), outputFolder = "bin", rawFlags, sourcesList;

	if (debug) {
		outputFile += "_debug";
	}

	if (args.count(OUTPUT_FLAG)) {
		outputFile = args.at(OUTPUT_FLAG);
	}

	if (args.count(FOLDER_FLAG)) {
		outputFolder = args.at(FOLDER_FLAG);
	}

	if (args.count(RAW_FLAGS_FLAG)) {
		rawFlags = args.at(RAW_FLAGS_FLAG);
	}

	string cmd = "g++ -Wall -W -pedantic-errors " + string(debug ? "-g" : "-s");

	if (rawFlags.length() != 0) {
		cmd += " " + rawFlags;
	}

	if (sys.mold.present) {
		if (gccVersionGood()) {
			cmd += " -fuse-ld=mold";
		} else {
			cmd += " -B/usr/local/libexec/mold";
		}
	}

	for (const Implementation& source : sources.sources) {
		sourcesList += source.path + " ";
	}

	cmd += " " + stripWhitespace(sourcesList) + " -o " + outputFolder + "/" + outputFile;

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

void compileToObject(const string& file, const map<Flag, string>& args, const SystemRequirements& sys, bool debug) {
	string outputFile = stripExtension(replace(file, "/", "_")) + ".o", outputFolder = "bin/.objects", rawFlags;

	if (args.count(FOLDER_FLAG)) {
		outputFolder = args.at(FOLDER_FLAG);
	}

	if (args.count(RAW_FLAGS_FLAG)) {
		rawFlags = args.at(RAW_FLAGS_FLAG);
	}

	string cmd = "g++ -Wall -W -pedantic-errors -c " + string(debug ? "-g" : "-s");

	if (rawFlags.length() != 0) {
		cmd += " " + rawFlags;
	}

	if (sys.mold.present) {
		if (gccVersionGood()) {
			cmd += " -fuse-ld=mold";
		} else {
			cmd += " -B/usr/local/libexec/mold";
		}
	}

	cmd += " " + file + " -o " + outputFolder + "/" + outputFile;

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}
}

string compileObjects(const string& mainFile, const map<Flag, string>& args, const SystemRequirements& sys, bool debug) {
	string inputFile = stripDirectories(mainFile), outputFile = stripExtension(inputFile), outputFolder = "bin", rawFlags;

	if (debug) {
		outputFile += "_debug";
	}

	if (args.count(OUTPUT_FLAG)) {
		outputFile = args.at(OUTPUT_FLAG);
	}

	if (args.count(FOLDER_FLAG)) {
		outputFolder = args.at(FOLDER_FLAG);
	}

	if (args.count(RAW_FLAGS_FLAG)) {
		rawFlags = args.at(RAW_FLAGS_FLAG);
	}

	string cmd = "g++ -Wall -W -pedantic-errors " + string(debug ? "-g" : "-s");

	if (rawFlags.length() != 0) {
		cmd += " " + rawFlags;
	}

	if (sys.mold.present) {
		if (gccVersionGood()) {
			cmd += " -fuse-ld=mold";
		} else {
			cmd += " -B/usr/local/libexec/mold";
		}
	}

	cmd += " " + outputFolder + "/.objects/*.o -o " + outputFolder + "/" + outputFile;

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

	return out;
}