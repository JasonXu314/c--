#include "c--_utils.h"

using namespace std;

void printHelpMessage(const vector<string>& argsList) {
	if (argsList.size() < 2) {
		cout << BWHT "Usage: " GRN << argsList[0] << CYN " <command>" reset "\n"
			 << "See " BWHT << argsList[0] << " help" reset " for more info" << endl;
	} else if (argsList.size() == 2) {
		cout << BWHT "Commands:" reset "\n"
			 << BCYN "help " BCYN "(h)" reset "     - display this message\n"
			 << BCYN "compile " BCYN "(c)" reset "  - compile a given file and its dependencies\n"
			 << BCYN "run " BCYN "(r)" reset "      - compile the given file and run it\n"
			 << BCYN "debug " BCYN "(d)" reset "    - compile the given file and gdb it\n"
			 << BCYN "valgrind " BCYN "(v)" reset " - compile the given file and run valgrind" << endl;
	} else if (argsList.size() == 3) {
		string helpCmd = argsList[2];

		if (helpCmd == "help" || helpCmd == "h") {
			cout << "Displays help about the program" << endl;
		} else if (helpCmd == "compile" || helpCmd == "c") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [compile | c] " BWHT "[<file> | <file>.cpp] [options]" reset "\n"
				 << "Compiles the given file and its dependencies.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "     - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "   - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset " - raw flags to pass to g++\n"
				 << "\t" CYN "--watch " BCYN "(-w)" reset "		 - watch the source files for changes" << endl;
		} else if (helpCmd == "run" || helpCmd == "r") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [run | r] " BWHT "[<file> | <file>.cpp] [options]\n" reset
				 << "Compiles the given file and runs it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "     - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "   - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--args " BCYN "(-a) " BWHT "<arguments>" reset "  - arguments to pass to the executable\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset " - raw flags to pass to g++\n"
				 << "\t" CYN "--watch " BCYN "(-w)" reset "		- watch the source files for changes" << endl;
		} else if (helpCmd == "debug" || helpCmd == "d") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [debug | d] " BWHT "[<file> | <file>.cpp] [options]" reset "\n"
				 << "Compiles the given file and " BWHT "gdb" reset "'s it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "     - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "   - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--gdb-flags " BCYN "(-g) " BWHT "<flags>" reset " - raw flags to pass to gdb\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset " - raw flags to pass to g++" << endl;
		} else if (helpCmd == "valgrind" || helpCmd == "v") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [valgrind | v] " BWHT "[<file> | <file>.cpp] [options]" reset "\n"
				 << "Compiles the given file and runs " BWHT "valgrind" reset " on it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "          - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "        - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--args " BCYN "(-a) " BWHT "<arguments>" reset "       - arguments to pass to the executable\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset "      - raw flags to pass to g++\n"
				 << "\t" CYN "--valgrind-flags " BCYN "(-v) " BWHT "<flags>" reset " - raw flags to pass to valgrind" << endl;
		} else {
			throw invalid_argument("Unknown command: " + helpCmd + "\nUsage: " + argsList[0] + " help <command>");
		}
	} else {
		throw invalid_argument("Usage: " + argsList[0] + " help [command]");
	}
}

void compileCommand(const string& file, const map<Flag, string>& args) {
	string mainFile = normalizeFileName(file), outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) : "bin";

	if (args.count(WATCH_FLAG)) {
		string objectsFolder = outputFolder + "/.objects";

		struct stat dirInfo;
		if (stat(outputFolder.c_str(), &dirInfo) != 0) {
			string mkdirCmd = "mkdir " + outputFolder;
			bool statusCode = system(mkdirCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error creating output folder: " BWHT + outputFolder + reset);
			}
		}
		if (stat(objectsFolder.c_str(), &dirInfo) != 0) {
			string mkdirCmd = "mkdir " + objectsFolder;
			bool statusCode = system(mkdirCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error creating objects folder: " BWHT + objectsFolder + reset);
			}
		} else {
			string rmCmd = "rm " + objectsFolder + "/*";
			bool statusCode = system(rmCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error removing objects in folder: " BWHT + objectsFolder + reset);
			}
		}

		auto initialCompile = [&args, &mainFile](const SourceSet& sources) {
			try {
				for (const string& source : sources.sources) {
					compileToObject(source, args);
				}

				string outputPath = compileObjects(mainFile, args);

				cout << GRN "Succesfully compiled to " BWHT << outputPath << reset << endl;
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};
		auto onChange = [&args, &mainFile, &outputFolder](const SourceDiff& diff, const string& changedFile) {
			try {
				compileToObject(changedFile, args);

				for (const string& source : diff.removed) {
					string objectPath = outputFolder + "/.objects/" + source + ".o";
					remove(objectPath.c_str());
				}

				for (const string& source : diff.added) {
					compileToObject(source, args);
				}

				string outputPath = compileObjects(mainFile, args);

				cout << GRN "Succesfully compiled to " BWHT << outputPath << reset << endl;
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};

		runWatchLoop(mainFile, initialCompile, onChange);
	} else {
		SourceSet sources = generateSources(mainFile);
		compile(sources, args);
	}
}

void compileAndRun(const string& file, const map<Flag, string>& args) {
	string mainFile = normalizeFileName(file), outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) : "bin";

	if (args.count(WATCH_FLAG)) {
		string objectsFolder = outputFolder + "/.objects";

		struct stat dirInfo;
		if (stat(outputFolder.c_str(), &dirInfo) != 0) {
			string mkdirCmd = "mkdir " + outputFolder;
			bool statusCode = system(mkdirCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error creating output folder: " BWHT + outputFolder + reset);
			}
		}
		if (stat(objectsFolder.c_str(), &dirInfo) != 0) {
			string mkdirCmd = "mkdir " + objectsFolder;
			bool statusCode = system(mkdirCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error creating objects folder: " BWHT + objectsFolder + reset);
			}
		} else {
			string rmCmd = "rm " + objectsFolder + "/*";
			bool statusCode = system(rmCmd.c_str());

			if (statusCode != 0) {
				throw runtime_error(BHRED "Error removing objects in folder: " BWHT + objectsFolder + reset);
			}
		}

		auto initialCompile = [&args, &mainFile](const SourceSet& sources) {
			try {
				for (const string& source : sources.sources) {
					compileToObject(source, args);
				}

				string executablePath = compileObjects(mainFile, args), runCmd = executablePath;

				if (args.count(ARGS_FLAG)) {
					runCmd += " " + args.at(ARGS_FLAG);
				}

				system("clear");
				system(runCmd.c_str());
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};
		auto onChange = [&args, &mainFile, &outputFolder](const SourceDiff& diff, const string& changedFile) {
			try {
				compileToObject(changedFile, args);

				for (const string& source : diff.removed) {
					string objectPath = outputFolder + "/.objects/" + source + ".o";
					remove(objectPath.c_str());
				}

				for (const string& source : diff.added) {
					compileToObject(source, args);
				}

				string executablePath = compileObjects(mainFile, args), runCmd = executablePath;

				if (args.count(ARGS_FLAG)) {
					runCmd += " " + args.at(ARGS_FLAG);
				}

				system("clear");
				system(runCmd.c_str());
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};

		runWatchLoop(mainFile, initialCompile, onChange);
	} else {
		SourceSet sources = generateSources(mainFile);
		string executablePath = compile(sources, args, false), runCmd = executablePath;

		if (args.count(ARGS_FLAG)) {
			runCmd += " " + args.at(ARGS_FLAG);
		}

		system(runCmd.c_str());
	}
}

void compileAndDebug(const string& file, const map<Flag, string>& args) {
	if (system("gdb --version > /dev/null") != 0) {
		throw runtime_error(BHRED "GDB not found" reset);
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
		throw runtime_error(BHRED "Valgrind not found" reset);
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
			throw runtime_error(BHRED "Error creating output folder: " BWHT + outputFolder + reset);
		}
	}

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}

	return "./" + outputFolder + "/" + outputFile;
}

void compileToObject(const string& file, const map<Flag, string>& args) {
	string outputFile = stripExtension(replace(file, "/", "_")) + ".o", outputFolder = "bin/.objects", rawFlags;

	if (args.count(FOLDER_FLAG)) {
		outputFolder = args.at(FOLDER_FLAG);
	}

	if (args.count(RAW_FLAGS_FLAG)) {
		rawFlags = args.at(RAW_FLAGS_FLAG);
	}

	string cmd = "g++ -Wall -W -pedantic-errors -c";

	if (rawFlags.length() != 0) {
		cmd += " " + rawFlags;
	}

	cmd += " " + file + " -o " + outputFolder + "/" + outputFile;

	bool statusCode = system(cmd.c_str());

	if (statusCode != 0) {
		throw runtime_error(BHRED "Error during compilation...\n" BWHT "Command: " reset + cmd);
	}
}

string compileObjects(const string& mainFile, const map<Flag, string>& args) {
	string inputFile = mainFile, outputFile = stripExtension(inputFile), outputFolder = "bin", rawFlags;

	if (args.count(OUTPUT_FLAG)) {
		outputFile = args.at(OUTPUT_FLAG);
	}

	if (args.count(FOLDER_FLAG)) {
		outputFolder = args.at(FOLDER_FLAG);
	}

	if (args.count(RAW_FLAGS_FLAG)) {
		rawFlags = args.at(RAW_FLAGS_FLAG);
	}

	string cmd = "g++ -Wall -W -pedantic-errors -s";

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
	bool deletePhase = true;

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

	while (true) {
		if (read(fileWatcher, &event, sizeof(inotify_event) + NAME_MAX + 1) && (event.mask & IN_MODIFY)) {
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
		}
	}
}