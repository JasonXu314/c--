#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "Flag.h"
#include "FlagSet.h"

using namespace std;

const regex FLAG_REGEX("(--?[a-zA-Z\\-]+)=(.+)");

map<Flag, string> parseArgs(const vector<string>& argsList, const FlagSet& flags, size_t startIdx = 2);

string normalizeFileName(const string& fileName);

string stripExtension(const string& fileName);

string stripWhitespace(const string& str);

string stripDirectories(const string& fileName);

string getParentDirectory(const string& fileName);

string replace(const string& str, const string& from, const string& to);

string readFile(const string& path);

// Recursively reads the directory (subdirectories' files will have preceeding path prepended)
set<string> readDir(const string& path);

vector<string> split(const string& str, const string& delimiter);

string resolvePath(const string& path);

#endif