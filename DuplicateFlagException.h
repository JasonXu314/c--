#ifndef DUPLICATE_FLAG_EXCEPTION_H
#define DUPLICATE_FLAG_EXCEPTION_H

#include <exception>
#include <string>

#include "Flag.h"

using namespace std;

class DuplicateFlagException : public exception {
public:
	DuplicateFlagException(const Flag& flag);

	virtual const char* what() const noexcept override;

private:
	string msg;
};

#endif