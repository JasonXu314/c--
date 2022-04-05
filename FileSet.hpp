#include "FileSet.h"

template <class FileType>
bool FileSet<FileType>::contains(const FileType& file) const {
	return _files.count(file);
}

template <class FileType>
bool FileSet<FileType>::contains(const string& fileName) const {
	for (const FileType& file : _files) {
		if (file.path == fileName) {
			return true;
		}
	}
	return false;
}

template <class FileType>
void FileSet<FileType>::insert(const FileType& file) {
	_files.insert(file);
}

template <class FileType>
FileType& FileSet<FileType>::operator[](const string& fileName) {
	for (FileType& file : _files) {
		if (file.path == fileName) {
			return file;
		}
	}
	throw runtime_error("File not found: " + fileName);
}

template <class FileType>
typename set<FileType>::iterator FileSet<FileType>::begin() const {
	return _files.begin();
}

template <class FileType>
typename set<FileType>::iterator FileSet<FileType>::end() const {
	return _files.end();
}