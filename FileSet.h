#ifndef FILE_SET_H
#define FILE_SET_H

#include <iterator>
#include <set>
#include <unordered_set>
#include <string>

using namespace std;

template <class FileType>
class FileSet {
public:
	FileSet() {}

	bool contains(const FileType& file) const;
	bool contains(const string& fileName) const;

	void insert(const FileType& file);

	FileType& operator[](const string& fileName);

	typename set<FileType>::iterator begin() const;
	typename set<FileType>::iterator end() const;

private:
	set<FileType> _files;
};

#endif