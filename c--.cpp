#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "cutil_utils.h"
#include "utils.h"

using namespace std;

int main(int argc, char** argv) {
	if (system("g++ --version > /dev/null") != 0) {
		cout << "g++ not found" << endl;
		return 1;
	}

	vector<string> argsList;
	set<string> flags;
	string prevFlag;

	for (int i = 0; i < argc; i++) {
		argsList.push_back(argv[i]);

		if (argv[i][0] == '-') {
			if (CUTIL_FLAGS.count(argv[i])) {
				flags.insert(argv[i]);
				prevFlag = argv[i];
			} else if (!isCollatingFlag(prevFlag)) {
				cout << "Unknown flag: " << argv[i] << endl;
				return 1;
			}
		}
	}

	if (argc < 2) {
		printHelpMessage(argsList);
	} else {
		string cmd = argsList[1];

		if (cmd == "help" || cmd == "h") {
			printHelpMessage(argsList);
		} else if (cmd == "compile" || cmd == "c") {
			compileFile(argsList, flags);
		} else if (cmd == "run" || cmd == "r") {
			compileAndRun(argsList, flags);
		} else if (cmd == "debug" || cmd == "d") {
			compileAndDebug(argsList, flags);
		} else if (cmd == "memcheck" || cmd == "m") {
			compileAndMemcheck(argsList, flags);
		} else {
			cout << "Unknown command: " << cmd << endl;
		}
	}

	return 0;
}