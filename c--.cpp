#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "c--_utils.h"

using namespace std;

int main(int argc, char** argv) {
	if (system("g++ --version > /dev/null") != 0) {
		cout << "g++ not found" << endl;
		return 1;
	}

	vector<string> argsList;

	for (int i = 0; i < argc; i++) {
		argsList.push_back(argv[i]);
	}

	try {
		map<Flag, string> args = parseArgs(argsList, CMM_FLAGS);

		if (argc < 2) {
			printHelpMessage(argsList);
		} else {
			string cmd = argsList[1];

			if (cmd == "help" || cmd == "h") {
				printHelpMessage(argsList);
			} else if (argc < 3) {
				printHelpMessage({argv[0], "help", cmd});
			} else {
				if (cmd == "compile" || cmd == "c") {
					compileFile(argsList[2], args);
				} else if (cmd == "run" || cmd == "r") {
					compileAndRun(argsList[2], args);
				} else if (cmd == "debug" || cmd == "d") {
					compileAndDebug(argsList[2], args);
				} else if (cmd == "valgrind" || cmd == "v") {
					compileAndValgrind(argsList[2], args);
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