#ifndef SOURCE_DIFF_H
#define SOURCE_DIFF_H

#include <set>
#include <string>

using namespace std;

struct SourceDiff {
	set<string> added;
	set<string> removed;
};

#endif