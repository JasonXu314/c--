#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <set>
#include <string>
#include <vector>

using namespace std;

string findValueOfFlag(const string& flag, const vector<string>& argsList);

string collectValuesOfFlag(const string& flag, const vector<string>& argsList, const set<string>& reservedFlags);

string normalizeFileName(const string& fileName);

string stripExtension(const string& fileName);

string stripWhitespace(const string& str);

string readFile(const string& path);

vector<string> readDir(const string& path);

#endif