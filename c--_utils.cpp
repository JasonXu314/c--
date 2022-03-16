#include "c--_utils.h"

using namespace std;

void printHelpMessage(const vector<string>& argsList) {
	if (argsList.size() < 2) {
		cout << "Usage: " << argsList[0] << " <command>\nSee " << argsList[0] << " help for more info" << endl;
	} else if (argsList.size() == 2) {
		cout << "Commands:\n"
			 << "help (h)     - display this message\n"
			 << "compile (c)  - compile a given file and its dependencies\n"
			 << "run (r)      - compile the given file and run it\n"
			 << "debug (d)    - compile the given file and gdb it\n"
			 << "valgrind (v) - compile the given file and run valgrind" << endl;
	} else if (argsList.size() == 3) {
		string helpCmd = argsList[2];

		if (helpCmd == "help" || helpCmd == "h") {
			cout << "Displays help about the program" << endl;
		} else if (helpCmd == "compile" || helpCmd == "c") {
			cout << "Usage: " << argsList[0] << " [compile | c] [<file> | <file>.cpp] [options]\n"
				 << "Compiles the given file and its dependencies.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++"
				 << "\t--watch (-w)				- watch the source files for changes" << endl;
		} else if (helpCmd == "run" || helpCmd == "r") {
			cout << "Usage: " << argsList[0] << " [run | r] [<file> | <file>.cpp] [options]\n"
				 << "Compiles the given file and runs it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--args (-a) <arguments>  - arguments to pass to the executable\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++"
				 << "\t--watch (-w)				- watch the source files for changes" << endl;
		} else if (helpCmd == "debug" || helpCmd == "d") {
			cout << "Usage: " << argsList[0] << " [debug | d] [<file> | <file>.cpp] [options]\n"
				 << "Compiles the given file and gdb's it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--gdb-flags (-g) <flags> - raw flags to pass to gdb\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++" << endl;
		} else if (helpCmd == "valgrind" || helpCmd == "v") {
			cout << "Usage: " << argsList[0] << " [valgrind | v] [<file> | <file>.cpp] [options]\n"
				 << "Compiles the given file and runs valgrind on it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>          - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>        - folder to output to (default \"bin\")\n"
				 << "\t--args (-a) <arguments>       - arguments to pass to the executable\n"
				 << "\t--raw-flags (-r) <flags>      - raw flags to pass to g++\n"
				 << "\t--valgrind-flags (-v) <flags> - raw flags to pass to valgrind" << endl;
		} else {
			throw invalid_argument("Unknown command: " + helpCmd + "\nUsage: " + argsList[0] + " help <command>");
		}
	} else {
		throw invalid_argument("Usage: " + argsList[0] + " help [command]");
	}
}

void compileCommand(const string& file, const map<Flag, string>& args) {
	if (args.count(WATCH_FLAG)) {
		auto onChange = [&args](const SourceSet& sources) {
			try {
				string outputPath = compile(sources, args);

				cout << "Succesfully compiled to " << outputPath << endl;
			} catch (exception& e) {
				cout << "Compilation failed...\n" << e.what() << endl;
			}
		};

		runWatchLoop(file, onChange);
	} else {
		SourceSet sources = generateSources(normalizeFileName(file));
		compile(sources, args);
	}
}

string compile(const SourceSet& sources, const map<Flag, string>& args, bool debug) {
	string inputFile = sources.main, outputFile = stripExtension(inputFile), outputFolder = "bin", rawFlags, sourcesList;

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
			throw runtime_error("Error creating output folder: " + outputFolder);
		}
	}

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error("Error during compilation...\nCommand: " + cmd);
	}

	return "./" + outputFolder + "/" + outputFile;
}

void compileAndRun(const string& file, const map<Flag, string>& args) {
	if (args.count(WATCH_FLAG)) {
		auto onChange = [&args](const SourceSet& sources) {
			try {
				string executablePath = compile(sources, args, false), runCmd = executablePath;

				if (args.count(ARGS_FLAG)) {
					runCmd += " " + args.at(ARGS_FLAG);
				}

				system("clear");
				system(runCmd.c_str());
			} catch (exception& e) {
				cout << "Compilation failed...\n" << e.what() << endl;
			}
		};

		runWatchLoop(file, onChange);
	} else {
		SourceSet sources = generateSources(normalizeFileName(file));
		string executablePath = compile(sources, args, false), runCmd = executablePath;

		if (args.count(ARGS_FLAG)) {
			runCmd += " " + args.at(ARGS_FLAG);
		}

		system(runCmd.c_str());
	}
}

void compileAndDebug(const string& file, const map<Flag, string>& args) {
	if (system("gdb --version > /dev/null") != 0) {
		throw runtime_error("GDB not found");
	}

	SourceSet sources = generateSources(normalizeFileName(file));
	string executablePath = compile(sources, args, true), runCmd = "gdb " + executablePath;

	if (args.count(GDB_FLAGS_FLAG)) {
		runCmd += " " + args.at(GDB_FLAGS_FLAG);
	}

	system(runCmd.c_str());
}

void compileAndValgrind(const string& file, const map<Flag, string>& args) {
	if (system("valgrind --version > /dev/null") != 0) {
		throw runtime_error("Valgrind not found");
	}

	SourceSet sources = generateSources(normalizeFileName(file));
	string executablePath = compile(sources, args, true), runCmd = "valgrind --leak-check=full";

	if (args.count(VALGRIND_FLAGS_FLAG)) {
		runCmd += " " + args.at(VALGRIND_FLAGS_FLAG);
	}

	runCmd += " " + executablePath;

	if (args.count(ARGS_FLAG)) {
		runCmd += " " + args.at(ARGS_FLAG);
	}

	system(runCmd.c_str());
}

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

void reconcileSources(const int fileWatcher, const SourceSet& oldSources, const SourceSet& newSources, const hash<string>& hash,
					  map<int, string>& watchDescriptorToPath, map<string, int>& pathToWatchDescriptor, map<string, size_t>& lastContents) {
	for (const string& source : newSources.sources) {
		if (!oldSources.sources.count(source)) {
			int watchDescriptor = inotify_add_watch(fileWatcher, source.c_str(), IN_MODIFY);
			watchDescriptorToPath.insert({watchDescriptor, source});
			pathToWatchDescriptor.insert({source, watchDescriptor});
			lastContents[source] = hash(stripWhitespace(readFile(source)));
		}
	}

	for (const string& source : oldSources.sources) {
		if (!newSources.sources.count(source)) {
			inotify_rm_watch(fileWatcher, pathToWatchDescriptor[source]);
			watchDescriptorToPath.erase(pathToWatchDescriptor[source]);
			pathToWatchDescriptor.erase(source);
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
}

void runWatchLoop(const string& file, const function<void(const SourceSet&)>& onChange) {
	int fileWatcher = inotify_init();
	map<int, string> watchDescriptorToPath;
	map<string, int> pathToWatchDescriptor;
	map<string, size_t> lastContents;
	inotify_event event;
	hash<string> hash;
	SourceSet sources = generateSources(normalizeFileName(file));

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

	onChange(sources);

	while (true) {
		if (read(fileWatcher, &event, 16) && (event.mask & IN_MODIFY)) {
			string changedFile = watchDescriptorToPath[event.wd];
			size_t hashedContents = hash(stripWhitespace(readFile(changedFile)));

			if (lastContents[changedFile] != hashedContents) {
				system("clear");
				cout << "Change to " << changedFile << " detected, re-compiling..." << endl;

				SourceSet newSources = generateSources(normalizeFileName(file));
				reconcileSources(fileWatcher, sources, newSources, hash, watchDescriptorToPath, pathToWatchDescriptor, lastContents);
				sources = newSources;

				onChange(sources);

				lastContents[changedFile] = hashedContents;
			} else {
				cout << "No effective change to " << changedFile << ", not compiling" << endl;
			}
		}
	}
}