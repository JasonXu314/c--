#include "FlagSet.h"

using namespace std;

FlagSet::FlagSet(const set<Flag>& flags) {
	for (const Flag& flag : flags) {
		_flags.insert(flag);
	}
}

FlagSet::FlagSet(const initializer_list<Flag>& flags) {
	for (const Flag& flag : flags) {
		_flags.insert(flag);
	}
}

bool FlagSet::contains(const Flag& flag) const { return _flags.count(flag); }

bool FlagSet::contains(const string& flagName) const {
	for (const Flag& flag : _flags) {
		if (flag == flagName) {
			return true;
		}
	}

	return false;
}

const Flag FlagSet::get(const string& flagName) const {
	for (const Flag& flag : _flags) {
		if (flag == flagName) {
			return flag;
		}
	}

	throw runtime_error("FlagSet::get: flag not found");
}