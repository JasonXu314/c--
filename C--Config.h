#ifndef CMM_CONFIG_H
#define CMM_CONFIG_H

#include <map>
#include <string>

#include "C--Script.h"

using namespace std;

struct Config {
	map<Flag, string> defaultFlags;
	map<string, Script> scripts;
};

#endif