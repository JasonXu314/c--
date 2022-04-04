#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Flag.h"
#include "SystemRequirements.h"
#include "c--_utils.h"

using namespace std;

void helpCommand(const vector<string>& argsList);

void compileCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys);

void runCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys);

void debugCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys);

void valgrindCommand(const string& file, const map<Flag, string>& args, const SystemRequirements& sys);

#endif