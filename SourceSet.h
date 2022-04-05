#ifndef SOURCE_SET_H
#define SOURCE_SET_H

#include <set>
#include <string>

#include "FileSet.hpp"
#include "SourceFiles.h"

using namespace std;

struct SourceSet {
	Implementation main;
	FileSet<Header> headers;
	FileSet<Implementation> sources;
};

#endif