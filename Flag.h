#ifndef FLAG_H
#define FLAG_H

#include <set>
#include <string>

using namespace std;

enum FlagType { NORMAL, BOOLEAN, COLLATING };

struct Flag {
	string flag;
	set<string> aliases;
	FlagType flagType;

	string toString() {
		string out = flag + " (";

		for (auto it = aliases.begin(); it != aliases.end(); it++) {
			if (it == --aliases.end()) {
				out += *it + ")";
			} else {
				out += *it + ", ";
			}
		}

		return out;
	}

	bool operator==(const Flag& other) const { return flag == other.flag; }
	bool operator==(const string& other) const { return flag == other || aliases.count(other); }

	bool operator<(const Flag& other) const { return flag < other.flag; }
};

#endif