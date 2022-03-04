#include "DuplicateFlagException.h"

DuplicateFlagException::DuplicateFlagException(const Flag& flag) : _flag(flag) {}

const char* DuplicateFlagException::what() const noexcept {
	string out = "Duplicate Flags: " + _flag.flag + " (";

	for (auto it = _flag.aliases.begin(); it != _flag.aliases.end(); it++) {
		if (it == --_flag.aliases.end()) {
			out += *it + ")";
		} else {
			out += *it + ", ";
		}
	}

	return out.c_str();
}