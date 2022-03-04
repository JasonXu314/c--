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
			 << "memcheck (m) - compile the given file and run valgrind" << endl;
	} else if (argsList.size() == 3) {
		string helpCmd = argsList[2];

		if (helpCmd == "help" || helpCmd == "h") {
			cout << "Displays help about the program" << endl;
		} else if (helpCmd == "compile" || helpCmd == "c") {
			cout << "Usage: " << argsList[0] << " [compile | c] [<file> | <file>.cpp | all] [options]\n"
				 << "Compiles the given file and its dependencies.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++" << endl;
		} else if (helpCmd == "run" || helpCmd == "r") {
			cout << "Usage: " << argsList[0] << " [run | r] [<file> | <file>.cpp | all] [options]\n"
				 << "Compiles the given file and runs it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--args (-a) <arguments>  - arguments to pass to the executable\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++" << endl;
		} else if (helpCmd == "debug" || helpCmd == "d") {
			cout << "Usage: " << argsList[0] << " [debug | d] [<file> | <file>.cpp | all] [options]\n"
				 << "Compiles the given file and gdb's it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>     - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>   - folder to output to (default \"bin\")\n"
				 << "\t--gdb-flags (-g) <flags> - raw flags to pass to gdb\n"
				 << "\t--raw-flags (-r) <flags> - raw flags to pass to g++" << endl;
		} else if (helpCmd == "memcheck" || helpCmd == "m") {
			cout << "Usage: " << argsList[0] << " [memcheck | m] [<file> | <file>.cpp | all] [options]\n"
				 << "Compiles the given file and runs valgrind on it.\n"
				 << "If the supplied file is \"all\", the output file will be called \"main\"\n\n"
				 << "Options:\n"
				 << "\t--output (-o) <name>          - output file name (default same name as source)\n"
				 << "\t--folder (-f) <folder>        - folder to output to (default \"bin\")\n"
				 << "\t--args (-a) <arguments>       - arguments to pass to the executable\n"
				 << "\t--raw-flags (-r) <flags>      - raw flags to pass to g++\n"
				 << "\t--valgrind-flags (-v) <flags> - raw flags to pass to valgrind" << endl;
		} else {
			cout << "Unknown command: " << helpCmd << "\nUsage: " << argsList[0] << " help <command>" << endl;
		}
	} else {
		cout << "Usage: " << argsList[0] << " help [command]" << endl;
	}
}

string compileFile(const string& file, const map<Flag, string>& args, bool debug) {
	string inputFile = file == "all" ? "all" : normalizeFileName(file), outputFile = inputFile == "all" ? "main" : stripExtension(inputFile),
		   outputFolder = "bin", rawFlags;

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

	if (inputFile == "all") {
		cmd += " *.cpp";
	} else {
		set<string> sources = generateSources(inputFile);
		string sourcesList;

		for (const string& source : sources) {
			sourcesList += source + " ";
		}

		cmd += " " + stripWhitespace(sourcesList);
	}

	cmd += " -o ./" + outputFolder + "/" + outputFile;

	struct stat dirInfo;
	if (stat(outputFolder.c_str(), &dirInfo) != 0) {
		string mkdirCmd = "mkdir " + outputFolder;
		system(mkdirCmd.c_str());
	}

	system(cmd.c_str());
	return "./" + outputFolder + "/" + outputFile;
}

void compileAndRun(const string& file, const map<Flag, string>& args) {
	string executablePath = compileFile(file, args), runCmd = executablePath;

	if (args.count(ARGS_FLAG)) {
		runCmd += " " + args.at(ARGS_FLAG);
	}

	system(runCmd.c_str());
}

void compileAndDebug(const string& file, const map<Flag, string>& args) {
	if (system("gdb --version > /dev/null") != 0) {
		cout << "GDB not found" << endl;
		exit(1);
	}

	string executablePath = compileFile(file, args, true), runCmd = "gdb " + executablePath;

	if (args.count(GDB_FLAGS_FLAG)) {
		runCmd += " " + args.at(GDB_FLAGS_FLAG);
	}

	system(runCmd.c_str());
}

void compileAndMemcheck(const string& file, const map<Flag, string>& args) {
	if (system("valgrind --version > /dev/null") != 0) {
		cout << "Valgrind not found" << endl;
		exit(1);
	}

	string executablePath = compileFile(file, args, true), runCmd = "valgrind --leak-check=full";

	if (args.count(VALGRIND_FLAGS_FLAG)) {
		runCmd += " " + args.at(VALGRIND_FLAGS_FLAG);
	}

	runCmd += " " + executablePath;

	if (args.count(ARGS_FLAG)) {
		runCmd += " " + args.at(ARGS_FLAG);
	}

	system(runCmd.c_str());
}

set<string> generateSources(const string& mainFile) {
	fstream in(mainFile);
	set<string> sources, headers;
	string line;
	map<string, set<string>> dependencyMap = generateDependencyMap();

	sources.insert(mainFile);

	for (const string& header : dependencyMap[mainFile]) {
		if (!headers.count(header)) {
			findHeaders(header, headers);
		}
	}

	for (const string& headerFile : headers) {
		set<string> dependents = findDependents(headerFile, sources, dependencyMap);

		for (const string& dependent : dependents) {
			sources.insert(dependent);
		}
	}

	in.close();

	return sources;
}

map<string, set<string>> generateDependencyMap() {
	map<string, set<string>> out;
	vector<string> files = readDir(".");

	for (const string& file : files) {
		if (regex_match(file, regex("^.*\\.cpp$"))) {
			if (!out.count(file)) {
				fstream in(file);
				string line;

				while (getline(in, line)) {
					regex includeRegex("^\\s*#include \"(.*)\"");

					if (regex_match(line, includeRegex)) {
						string includeFile = getIncludedFile(line);

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
			regex mainRegex(
				"int\\s+main\\s*\\(\\s*(int\\s+[a-zA-Z_][a-zA-Z_0-9]*|int\\s+[a-zA-Z_][a-zA-Z_0-9]*,\\s*(char\\*\\*\\s+[a-zA-Z_][a-zA-Z_0-9]*|char\\*"
				"\\s+[a-zA-Z_][a-zA-Z_0-9]*\\[\\d*\\]))?\\s*\\)");

			if (!regex_search(fileContents, mainRegex)) {
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

	while (getline(in, line)) {
		regex includeRegex("^\\s*#include \"(.*)\"");

		if (regex_match(line, includeRegex)) {
			string includeFile = getIncludedFile(line);

			if (!headersVisited.count(includeFile)) {
				findHeaders(includeFile, headersVisited);
			}
		}
	}

	in.close();
}

bool isCollatingFlag(const string& flag) {
	return flag == "--args" || flag == "-a" || flag == "--raw-flags" || flag == "-r" || flag == "--valgrind-flags" || flag == "-v";
}

string getIncludedFile(const string& line) {
	string fileName;
	bool inFileName = false;

	for (size_t i = 0; i < line.size(); i++) {
		if (line[i] == '"') {
			if (inFileName) {
				break;
			} else {
				inFileName = true;
			}
		} else if (inFileName) {
			fileName += line[i];
		}
	}

	return fileName;
}