#include "utils.h"

using namespace std;

string findValueOfFlag(const string& flag, const vector<string>& argsList) {
	for (size_t i = 0; i < argsList.size(); i++) {
		if (argsList[i] == flag) {
			return argsList[i + 1];
		}
	}

	return "";
}

string collectValuesOfFlag(const string& flag, const vector<string>& argsList, const set<string>& reservedFlags) {
	string result;
	bool foundFlag = false;

	for (size_t i = 0; i < argsList.size(); i++) {
		if (argsList[i] == flag) {
			foundFlag = true;
			continue;
		}

		if (foundFlag) {
			if (reservedFlags.count(argsList[i])) {
				foundFlag = false;
				break;
			}

			result += argsList[i] + " ";
		}
	}

	return result;
}

string normalizeFileName(const string& fileName) {
	if (fileName.length() >= 4 && fileName.substr(fileName.size() - 4) == ".cpp") {
		return fileName;
	} else {
		return fileName + ".cpp";
	}
}

string stripExtension(const string& fileName) {
	int dotPos = fileName.find_last_of('.');
	return fileName.substr(0, dotPos);
}

string stripWhitespace(const string& str) {
	string out = str;
	bool firstNonWhitespaceFound = false, lastNonWhitespaceFound = false;
	size_t firstNonWhitespace = 0, lastNonWhitespace = str.size() - 1;

	for (size_t i = 0; i < str.size(); i++) {
		if (!firstNonWhitespaceFound && str[i] != ' ' && str[i] != '\t') {
			firstNonWhitespace = i;
			firstNonWhitespaceFound = true;
		}
		if (!lastNonWhitespaceFound && str[str.size() - i - 1] != ' ' && str[str.size() - i - 1] != '\t') {
			lastNonWhitespace = str.size() - i - 1;
			lastNonWhitespaceFound = true;
		}
		if (firstNonWhitespaceFound && lastNonWhitespaceFound) {
			break;
		}
	}

	return out.substr(firstNonWhitespace, lastNonWhitespace - firstNonWhitespace + 1);
}

string readFile(const string& path) {
	ifstream file(path);
	string result, line;

	while (getline(file, line)) {
		result += line + "\n";
	}

	file.close();

	return result;
}

vector<string> readDir(const string& path) {
	vector<string> out;
	DIR* dir;
	dirent* entry;

	if ((dir = opendir(path.c_str())) != nullptr) {
		while ((entry = readdir(dir)) != nullptr) {
			if (entry->d_type == DT_REG) {
				string fileName = entry->d_name;

				out.push_back(fileName);
			}
		}
	}

	closedir(dir);

	return out;
}