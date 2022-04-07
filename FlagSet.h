#ifndef FLAG_SET_H
#define FLAG_SET_H

#include <set>
#include <stdexcept>

#include "Flag.h"

using namespace std;

class FlagSet {
public:
	FlagSet(const set<Flag>& flags);
	FlagSet(const initializer_list<Flag>& flags);

	bool contains(const Flag& flag) const;
	bool contains(const string& flagName) const;

	const Flag& get(const string& flagName) const;

private:
	set<Flag> _flags;
};

#endif