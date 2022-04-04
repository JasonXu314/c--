#include "c--_utils.h"

SourceSet generateSources(const string& mainFile) {
	SourceSet out;
	fstream in(mainFile);
	string line;
	map<string, set<string>> dependencyMap = generateDependencyMap();

	out.main = mainFile;
	out.sources.insert(mainFile);

	for (const string& header : dependencyMap[mainFile]) {
		if (!out.headers.count(header)) {
			findHeaders(header, out.headers);
		}
	}

	for (const string& headerFile : out.headers) {
		set<string> dependents = findDependents(headerFile, out.sources, dependencyMap);

		for (const string& dependent : dependents) {
			out.sources.insert(dependent);
		}
	}

	in.close();

	return out;
}

map<string, set<string>> generateDependencyMap() {
	map<string, set<string>> out;
	vector<string> files = readDir(".");

	for (const string& file : files) {
		smatch foldersMatch;

		if (regex_match(file, foldersMatch, regex("^(.+/)?.*\\.cpp$"))) {
			if (!out.count(file)) {
				fstream in(file);
				string line;
				smatch includeMatch;

				while (getline(in, line)) {
					if (regex_match(line, includeMatch, INCLUDE_REGEX)) {
						string includeFile = foldersMatch[1].str() + includeMatch[1].str();

						out[file].insert(includeFile);
					}
				}

				in.close();
			}
		}
	}

	return out;
}

set<string> findDependents(const string& headerFile, const set<string>& ignore, const map<string, set<string>>& dependencyMap) {
	set<string> out;

	for (const auto& entry : dependencyMap) {
		string cppFile = entry.first;
		set<string> headers = entry.second;

		if (ignore.count(cppFile)) {
			continue;
		} else if (headers.count(headerFile)) {
			string fileContents = readFile(cppFile);

			if (!regex_search(fileContents, MAIN_REGEX)) {
				out.insert(cppFile);
			}
		}
	}

	return out;
}

void findHeaders(const string& fileName, set<string>& headersVisited) {
	if (headersVisited.count(fileName)) {
		return;
	} else {
		headersVisited.insert(fileName);
	}

	fstream in(fileName);
	string line;
	smatch match;

	while (getline(in, line)) {
		if (regex_match(line, match, INCLUDE_REGEX)) {
			string includeFile = match[1];

			if (!headersVisited.count(includeFile)) {
				findHeaders(includeFile, headersVisited);
			}
		}
	}

	in.close();
}

string directCompile(const SourceSet& sources, const map<Flag, string>& args, bool debug) {
	string inputFile = sources.main, outputFile = stripExtension(stripDirectories(inputFile)), outputFolder = "bin", rawFlags, sourcesList;

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

	for (const string& source : sources.sources) {
		sourcesList += source + " ";
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

void compileToObject(const string& file, const map<Flag, string>& args, bool debug) {
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

	cmd += " " + file + " -o " + outputFolder + "/" + outputFile;

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}
}

string compileObjects(const string& mainFile, const map<Flag, string>& args, bool debug) {
	string inputFile = mainFile, outputFile = stripExtension(inputFile), outputFolder = "bin", rawFlags;

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

	for (const string& source : newSources.sources) {
		if (!oldSources.sources.count(source)) {
			int watchDescriptor = inotify_add_watch(fileWatcher, source.c_str(), IN_MODIFY);
			watchDescriptorToPath.insert({watchDescriptor, source});
			pathToWatchDescriptor.insert({source, watchDescriptor});
			lastContents[source] = hash(stripWhitespace(readFile(source)));
			out.added.insert(source);
		}
	}

	for (const string& source : oldSources.sources) {
		if (!newSources.sources.count(source)) {
			inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source]);
			watchDescriptorToPath.erase(pathToWatchDescriptor[source]);
			pathToWatchDescriptor.erase(source);
			out.removed.insert(source);
		}
	}

	for (const string& header : newSources.headers) {
		if (!oldSources.headers.count(header)) {
			int watchDescriptor = inotify_add_watch(fileWatcher, header.c_str(), IN_MODIFY);
			watchDescriptorToPath.insert({watchDescriptor, header});
			pathToWatchDescriptor.insert({header, watchDescriptor});
			lastContents[header] = hash(stripWhitespace(readFile(header)));
		}
	}

	for (const string& header : oldSources.headers) {
		if (!newSources.headers.count(header)) {
			inotify_rm_watch(fileWatcher, pathToWatchDescriptor[header]);
			watchDescriptorToPath.erase(pathToWatchDescriptor[header]);
			pathToWatchDescriptor.erase(header);
		}
	}

	return out;
}

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& initialCompile,
				  const function<void(const SourceDiff&, const string&)>& onChange) {
	int fileWatcher = inotify_init();
	map<int, string> watchDescriptorToPath;
	map<string, int> pathToWatchDescriptor;
	map<string, size_t> lastContents;
	inotify_event event;
	hash<string> hash;
	SourceSet sources = generateSources(file);
	SourceDiff diff;
	bool deletePhase = true, continueWatch = true;

	thread quitThread(
		[](const pthread_t& parentThread) {
			while (true) {
				string input;

				cin >> input;

				if (input == "quit" || input == "q") {
					pthread_kill(parentThread, SIGINT);
					break;
				}
			}
		},
		pthread_self());

	for (const string& source : sources.sources) {
		int watchDescriptor = inotify_add_watch(fileWatcher, source.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, source});
		pathToWatchDescriptor.insert({source, watchDescriptor});
		lastContents.insert({source, hash(stripWhitespace(readFile(source)))});
	}

	for (const string& header : sources.headers) {
		int watchDescriptor = inotify_add_watch(fileWatcher, header.c_str(), IN_MODIFY);
		watchDescriptorToPath.insert({watchDescriptor, header});
		pathToWatchDescriptor.insert({header, watchDescriptor});
		lastContents.insert({header, hash(stripWhitespace(readFile(header)))});
	}

	initialCompile(sources);

	auto noop = [](int) {};

	signal(SIGINT, noop);
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

	quitThread.join();

	cout << BWHT "Interrupt signal received, cleaning up..." reset << endl;

	for (const string& source : sources.sources) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source]);
	}

	for (const string& header : sources.headers) {
		inotify_rm_watch(fileWatcher, pathToWatchDescriptor[header]);
	}
}