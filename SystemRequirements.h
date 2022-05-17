#ifndef SYSTEM_REQUIREMENTS_H
#define SYSTEM_REQUIREMENTS_H

#include <string>

using namespace std;

struct Component {
	bool present;
	string path;
};

struct SystemRequirements {
	Component gpp;
	Component gdb;
	Component valgrind;
	Component mold;
	Component lld;
	Component gcov;
};

#endif