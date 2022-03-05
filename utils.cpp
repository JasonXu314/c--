#include "utils.h"

using namespace std;

map<Flag, string> parseArgs(const vector<string>& argsList, const FlagSet& flags) {
	map<Flag, string> out;
	bool inCollatingFlag = false;
	string collatingFlagValue;
	Flag lastFlag;

	for (size_t i = 3; i < argsList.size(); i++) {
		string arg = argsList[i], flagValue;
		bool flagValuePair = false;
		smatch match;

		if (regex_match(argsList[i], match, FLAG_REGEX)) {
			flagValuePair = true;
			arg = match[1];
			flagValue = match[2];
		}

		if (flags.contains(arg)) {
			if (inCollatingFlag) {
				inCollatingFlag = false;
				out.insert({lastFlag, stripWhitespace(collatingFlagValue)});
				collatingFlagValue = "";
			}

			Flag flag = flags.get(arg);

			if (flag.isCollatingFlag && !flagValuePair) {
				inCollatingFlag = true;
				collatingFlagValue = "";
			} else {
				if (out.count(flag)) {
					throw DuplicateFlagException(flag);
				} else {
					if (flagValuePair) {
						out.insert({flag, flagValue});
					} else {
						out.insert({flag, argsList[i + 1]});
						i++;
					}
				}
			}

			lastFlag = flag;
		} else if (inCollatingFlag) {
			collatingFlagValue += arg + " ";
		} else {
			throw runtime_error("Unknown flag: " + arg);
		}
	}

	if (inCollatingFlag) {
		out.insert({lastFlag, stripWhitespace(collatingFlagValue)});
	}

	return out;
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