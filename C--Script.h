#ifndef CMM_SCRIPT_H
#define CMM_SCRIPT_H

#include <map>
#include <string>

#include "Flag.h"

using namespace std;

struct Script {
	string mainFile;
	string command;
	map<Flag, string> defaultFlags;
};

#endif