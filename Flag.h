#ifndef FLAG_H
#define FLAG_H

#include <set>
#include <string>

using namespace std;

struct Flag {
	string flag;
	set<string> aliases;

	bool operator==(const Flag& other) const { return flag == other.flag; }
	bool operator==(const string& other) const { return flag == other || aliases.count(other); }

	bool operator<(const Flag& other) const { return flag < other.flag; }
};

#endif