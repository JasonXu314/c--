#include "commands.h"

#include "SourceFiles.h"

void helpCommand(const vector<string>& argsList) {
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
				 << "\t" CYN "--watch " BCYN "(-w)" reset "		 - watch the source files for changes (" BWHT "quit" reset " or " BWHT "q" reset " to stop)\n"
				 << "\t" CYN "--ignore-mold " BCYN "(-m)" reset "	 - don't use mold as the linker\n"
				 << "\t" CYN "--ignore-lld " BCYN "(-l)" reset "	 - don't use ld.lld (llvm linker) as the linker\n"
				 << "\t" CYN "--debug " BCYN "(-d)" reset "		 - compile with debug symbols (" BHWHT "_debug" reset
					" will be appended to file name unless explicitly set)"
				 << endl;
		} else if (helpCmd == "run" || helpCmd == "r") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [run | r] " BWHT "[<file> | <file>.cpp] [options]\n" reset
				 << "Compiles the given file and runs it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "     - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "   - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--args " BCYN "(-a) " BWHT "<arguments>" reset "  - arguments to pass to the executable\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset " - raw flags to pass to g++\n"
				 << "\t" CYN "--watch " BCYN "(-w)" reset "		- watch the source files for changes\n"
				 << "\t" CYN "--ignore-mold " BCYN "(-m)" reset "	 - don't use mold as the linker\n"
				 << "\t" CYN "--ignore-lld " BCYN "(-l)" reset "	 - don't use ld.lld (llvm linker) as the linker" << endl;
		} else if (helpCmd == "debug" || helpCmd == "d") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [debug | d] " BWHT "[<file> | <file>.cpp] [options]" reset "\n"
				 << "Compiles the given file and " BWHT "gdb" reset "'s it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "     - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "   - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--gdb-flags " BCYN "(-g) " BWHT "<flags>" reset " - raw flags to pass to gdb\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset " - raw flags to pass to g++\n"
				 << "\t" CYN "--ignore-mold " BCYN "(-m)" reset "	 - don't use mold as the linker\n"
				 << "\t" CYN "--ignore-lld " BCYN "(-l)" reset "	 - don't use ld.lld (llvm linker) as the linker" << endl;
		} else if (helpCmd == "valgrind" || helpCmd == "v") {
			cout << BWHT "Usage: " GRN << argsList[0] << CYN " [valgrind | v] " BWHT "[<file> | <file>.cpp] [options]" reset "\n"
				 << "Compiles the given file and runs " BWHT "valgrind" reset " on it.\n\n"
				 << BWHT "Options:" reset "\n"
				 << "\t" CYN "--output " BCYN "(-o) " BWHT "<name>" reset "          - output file name (default same name as source)\n"
				 << "\t" CYN "--folder " BCYN "(-f) " BWHT "<folder>" reset "        - folder to output to (default \"bin\")\n"
				 << "\t" CYN "--args " BCYN "(-a) " BWHT "<arguments>" reset "       - arguments to pass to the executable\n"
				 << "\t" CYN "--raw-flags " BCYN "(-r) " BWHT "<flags>" reset "      - raw flags to pass to g++\n"
				 << "\t" CYN "--valgrind-flags " BCYN "(-v) " BWHT "<flags>" reset " - raw flags to pass to valgrind\n"
				 << "\t" CYN "--ignore-mold " BCYN "(-m)" reset "	 - don't use mold as the linker\n"
				 << "\t" CYN "--ignore-lld " BCYN "(-l)" reset "	 - don't use ld.lld (llvm linker) as the linker" << endl;
		} else {
			throw invalid_argument("Unknown command: " + helpCmd + "\nUsage: " + argsList[0] + " help <command>");
		}
	} else {
		throw invalid_argument("Usage: " + argsList[0] + " help [command]");
	}
}

void compileCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys) {
	string mainFile = normalizeFileName(file), outputFolder = args.count(FOLDER_FLAG) ? args.at(FOLDER_FLAG) : "bin";
	bool debug = false;

	if (args.count(DEBUG_FLAG)) {
		debug = true;
	}

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

		auto initialCompile = [&args, &mainFile, &debug, &sys](const SourceSet& sources) {
			try {
				for (const Implementation& source : sources.sources) {
					compileToObject(source.path, args, sys, debug);
				}

				string outputPath = compileObjects(mainFile, args, sys, debug);

				cout << GRN "Succesfully compiled to " BWHT << outputPath << reset << endl;
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};
		auto onChange = [&args, &mainFile, &outputFolder, &debug, &sys](const SourceDiff& diff, const string& changedFile) {
			try {
				compileToObject(changedFile, args, sys, debug);

				for (const string& source : diff.removed) {
					string objectPath = outputFolder + "/.objects/" + source + ".o";
					remove(objectPath.c_str());
				}

				for (const string& source : diff.added) {
					compileToObject(source, args, sys, debug);
				}

				string outputPath = compileObjects(mainFile, args, sys, debug);

				cout << GRN "Succesfully compiled to " BWHT << outputPath << reset << endl;
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};

		runWatchLoop(mainFile, initialCompile, onChange);
	} else {
		SourceSet sources = generateSources(mainFile);

		cout << BGRN "Compiling..." reset << endl;

		string outputPath = directCompile(sources, args, sys, debug);

		cout << GRN "Succesfully compiled to " BWHT << outputPath << reset << endl;
	}
}

void runCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys) {
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

		auto initialCompile = [&args, &mainFile, &sys](const SourceSet& sources) {
			try {
				for (const Implementation& source : sources.sources) {
					compileToObject(source.path, args, sys);
				}

				string executablePath = compileObjects(mainFile, args, sys), runCmd = executablePath;

				if (args.count(ARGS_FLAG)) {
					runCmd += " " + args.at(ARGS_FLAG);
				}

				system("clear");
				system(runCmd.c_str());
			} catch (exception& e) {
				cout << "Compilation " BHRED "failed..." reset "\n" << e.what() << endl;
			}
		};
		auto onChange = [&args, &mainFile, &outputFolder, &sys](const SourceDiff& diff, const string& changedFile) {
			try {
				compileToObject(changedFile, args, sys);

				for (const string& source : diff.removed) {
					string objectPath = outputFolder + "/.objects/" + source + ".o";
					remove(objectPath.c_str());
				}

				for (const string& source : diff.added) {
					compileToObject(source, args, sys);
				}

				string executablePath = compileObjects(mainFile, args, sys), runCmd = executablePath;

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

		cout << BGRN "Compiling..." reset << endl;

		string executablePath = directCompile(sources, args, sys, false), runCmd = executablePath;

		if (args.count(ARGS_FLAG)) {
			runCmd += " " + args.at(ARGS_FLAG);
		}

		system(runCmd.c_str());
	}
}

void debugCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys) {
	if (!sys.gdb.present) {
		throw runtime_error(BHRED "GDB not found" reset);
	}

	SourceSet sources = generateSources(normalizeFileName(file));
	string executablePath = directCompile(sources, args, sys, true), runCmd = "gdb " + executablePath;

	if (args.count(GDB_FLAGS_FLAG)) {
		runCmd += " " + args.at(GDB_FLAGS_FLAG);
	}

	system(runCmd.c_str());
}

void valgrindCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys) {
	if (!sys.valgrind.present) {
		throw runtime_error(BHRED "Valgrind not found" reset);
	}

	SourceSet sources = generateSources(normalizeFileName(file));
	string executablePath = directCompile(sources, args, sys, true), runCmd = "valgrind --leak-check=full";

	if (args.count(VALGRIND_FLAGS_FLAG)) {
		runCmd += " " + args.at(VALGRIND_FLAGS_FLAG);
	}

	runCmd += " " + executablePath;

	if (args.count(ARGS_FLAG)) {
		runCmd += " " + args.at(ARGS_FLAG);
	}

	system(runCmd.c_str());
}