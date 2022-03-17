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

			if (out.count(flag)) {
				string msg = "Duplicate flag: " + flag.flag + " (";

				for (auto it = flag.aliases.begin(); it != flag.aliases.end(); it++) {
					if (it == --flag.aliases.end()) {
						msg += *it + ")";
					} else {
						msg += *it + ", ";
					}
				}

				throw invalid_argument(msg);
			} else {
				if (flag.flagType == FlagType::COLLATING && !flagValuePair) {
					inCollatingFlag = true;
					collatingFlagValue = "";
				} else if (flag.flagType == FlagType::BOOLEAN) {
					out.insert({flag, ""});
				} else if (flagValuePair) {
					out.insert({flag, flagValue});
				} else {
					out.insert({flag, argsList[i + 1]});
					i++;
				}
			}

			lastFlag = flag;
		} else if (inCollatingFlag) {
			collatingFlagValue += arg + " ";
		} else {
			throw invalid_argument("Unknown flag: " + arg);
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
		if (!firstNonWhitespaceFound && str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
			firstNonWhitespace = i;
			firstNonWhitespaceFound = true;
		}
		if (!lastNonWhitespaceFound && str[str.size() - i - 1] != ' ' && str[str.size() - i - 1] != '\t' && str[str.size() - i - 1] != '\n') {
			lastNonWhitespace = str.size() - i - 1;
			lastNonWhitespaceFound = true;
		}
		if (firstNonWhitespaceFound && lastNonWhitespaceFound) {
			break;
		}
	}

	return out.substr(firstNonWhitespace, lastNonWhitespace - firstNonWhitespace + 1);
}

string stripDirectories(const string& fileName) {
	string out = fileName;
	size_t lastSlash = out.find_last_of('/');

	if (lastSlash != string::npos) {
		out = out.substr(lastSlash + 1);
	}

	return out;
}

string replace(const string& str, const string& from, const string& to) {
	string out = str;
	size_t pos = 0;

	while ((pos = out.find(from, pos)) != string::npos) {
		out.replace(pos, from.length(), to);
		pos += to.length();
	}

	return out;
}

string readFile(const string& path) {
	ifstream file(path);
	string result, line;

	while (getline(file, line)) {
		result += line + "\n";
	}

	file.close();

	return result.substr(0, line == "" ? result.length() : result.length() - 1);
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
			} else if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
				string dirName = entry->d_name;
				vector<string> dirContents = readDir(path + "/" + dirName);

				for (size_t i = 0; i < dirContents.size(); i++) {
					out.push_back(dirName + "/" + dirContents[i]);
				}
			}
		}
	}

	closedir(dir);

	return out;
}