#ifndef SOURCE_FILES_H
#define SOURCE_FILES_H

#include <initializer_list>
#include <set>
#include <string>

using namespace std;

enum SourceType { HEADER, IMPLEMENTATION };

struct SourceFile {
	string name;
	string path;
	SourceType type;
};

struct Header : public SourceFile {
	Header(const string& name, const string& path) : SourceFile{name, path, HEADER} {}

	bool operator==(const Header& other) const { return path == other.path; }
	bool operator==(const string& other) const { return path == other; }

	bool operator<(const Header& other) const { return path < other.path; }
};

struct Implementation : public SourceFile {
	Implementation(const string& name, const string& path) : SourceFile{name, path, IMPLEMENTATION} {}

	bool operator==(const Implementation& other) const { return path == other.path; }
	bool operator==(const string& other) const { return path == other; }

	bool operator<(const Implementation& other) const { return path < other.path; }
};

#endif