#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "commands.h"

using namespace std;

int main(int argc, char** argv) {
	SystemRequirements sys = findSystemRequirements();

	if (!sys.gpp.present) {
		cout << BRED "g++ not found" reset << endl;
		return 1;
	}

	siginterrupt(SIGINT, true);

	vector<string> argsList;

	for (int i = 0; i < argc; i++) {
		argsList.push_back(argv[i]);
	}

	try {
		if (argc < 2) {
			helpCommand(argsList);
		} else {
			string cmd = argsList[1], mainFile;
			bool hasConfig = false, hasScript = false;
			Config config;
			Script script;

			struct stat _;
			if (stat(".c--rc.yml", &_) == 0) {
				cout << BGRN "Using config file" BWHT " .c--rc.yml" reset << endl;
				config = parseConfig(".c--rc.yml");
				hasConfig = true;
			} else if (stat(".c--rc", &_) == 0) {
				cout << BGRN "Using config file" BWHT " .c--rc" reset << endl;
				config = parseConfig(".c--rc");
				hasConfig = true;
			} else if (stat(".cmmrc.yml", &_) == 0) {
				cout << BGRN "Using config file" BWHT " .cmmrc.yml" reset << endl;
				config = parseConfig(".cmmrc.yml");
				hasConfig = true;
			} else if (stat(".cmmrc", &_) == 0) {
				cout << BGRN "Using config file" BWHT " .cmmrc" reset << endl;
				config = parseConfig(".cmmrc");
				hasConfig = true;
			}

			if (hasConfig && config.scripts.count(cmd)) {
				cout << BGRN "Using script " BWHT << cmd << reset << endl;
				script = config.scripts[cmd];

				cmd = script.command;
				mainFile = script.mainFile;
				hasScript = true;
			}

			map<Flag, string> args = parseArgs(argsList, CMM_FLAGS, hasScript ? 2 : 3);

			if (hasConfig) {
				mergeConfig(args, config);
			}
			if (hasScript) {
				mergeScriptConfig(args, script);
			}

			if (sys.mold.present && !args.count(IGNORE_MOLD_FLAG)) {
				cout << BGRN "Using mold..." reset << endl;
			} else if (sys.lld.present && !args.count(IGNORE_LLD_FLAG)) {
				cout << BGRN "Using lld..." reset << endl;
			} else {
				cout << BCYN "Using default " BWHT "ld..." reset << endl;
			}

			if (cmd == "help" || cmd == "h") {
				helpCommand(argsList);
			} else if (argc < 3) {
				helpCommand({argv[0], "help", cmd});
			} else {
				if (mainFile == "") {  // if it wasn't set by a script (might change this check later)
					mainFile = argsList[2];
				}

				if (cmd == "compile" || cmd == "c") {
					compileCommand(mainFile, args, sys);
				} else if (cmd == "run" || cmd == "r") {
					runCommand(mainFile, args, sys);
				} else if (cmd == "debug" || cmd == "d") {
					debugCommand(mainFile, args, sys);
				} else if (cmd == "valgrind" || cmd == "v") {
					valgrindCommand(mainFile, args, sys);
				} else if (cmd == "gcov" || cmd == "g") {
					gcovCommand(mainFile, args, sys);
				} else {
					cout << "Unknown command: " << cmd << endl;
				}
			}
		}
	} catch (exception& err) {
		cerr << err.what() << endl;
		return 1;
	}

	return 0;
}