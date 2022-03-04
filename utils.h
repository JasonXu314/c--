#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "DuplicateFlagException.h"
#include "Flag.h"
#include "FlagSet.h"

using namespace std;

map<Flag, string> parseArgs(const vector<string>& argsList, const FlagSet& flags, const FlagSet& collatingFlags);

string normalizeFileName(const string& fileName);

string stripExtension(const string& fileName);

string stripWhitespace(const string& str);

string readFile(const string& path);

vector<string> readDir(const string& path);

#endif