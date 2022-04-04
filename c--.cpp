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

	if (sys.mold.present) {
		cout << BGRN "Using mold..." reset << endl;
	} else {
		cout << BCYN "Using default " BWHT "ld..." reset << endl;
	}

	siginterrupt(SIGINT, true);

	vector<string> argsList;

	for (int i = 0; i < argc; i++) {
		argsList.push_back(argv[i]);
	}

	try {
		map<Flag, string> args = parseArgs(argsList, CMM_FLAGS);

		if (argc < 2) {
			helpCommand(argsList);
		} else {
			string cmd = argsList[1];

			if (cmd == "help" || cmd == "h") {
				helpCommand(argsList);
			} else if (argc < 3) {
				helpCommand({argv[0], "help", cmd});
			} else {
				if (cmd == "compile" || cmd == "c") {
					compileCommand(argsList[2], args, sys);
				} else if (cmd == "run" || cmd == "r") {
					runCommand(argsList[2], args, sys);
				} else if (cmd == "debug" || cmd == "d") {
					debugCommand(argsList[2], args, sys);
				} else if (cmd == "valgrind" || cmd == "v") {
					valgrindCommand(argsList[2], args, sys);
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