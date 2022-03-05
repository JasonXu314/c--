#include "DuplicateFlagException.h"

using namespace std;

DuplicateFlagException::DuplicateFlagException(const Flag& flag) {
	msg = "Duplicate flag: " + flag.flag + " (";

	for (auto it = flag.aliases.begin(); it != flag.aliases.end(); it++) {
		if (it == --flag.aliases.end()) {
			msg += *it + ")";
		} else {
			msg += *it + ", ";
		}
	}
}

const char* DuplicateFlagException::what() const noexcept { return msg.c_str(); }