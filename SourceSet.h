#ifndef SOURCE_SET_H
#define SOURCE_SET_H

#include <set>
#include <string>

using namespace std;

struct SourceSet {
	string main;
	set<string> headers;
	set<string> sources;
};

#endif