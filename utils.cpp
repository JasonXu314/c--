#include "utils.h"

using namespace std;

map<Flag, string> parseArgs(const vector<string>& argsList, const FlagSet& flags, size_t startIdx) {
	map<Flag, string> out;
	bool inCollatingFlag = false;
	string collatingFlagValue;
	Flag lastFlag;

	for (size_t i = startIdx; i < argsList.size(); i++) {
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
				throw invalid_argument("Unknown flag: " + arg);
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
		} else if (arg.substr(0, 2) != "--" && arg.substr(0, 1) == "-") {
			for (size_t i = 1; i < arg.length(); i++) {
				if (!flags.contains("-" + arg.substr(i, 1))) {	// use substr because otherwise would be adding chars
					throw invalid_argument("Unknown flag: -" + arg.substr(i, 1) + " in concatenated flag " + arg);
				} else {
					Flag flag = flags.get("-" + arg.substr(i, 1));

					if (flag.flagType != FlagType::BOOLEAN) {
						throw invalid_argument("Only boolean flags can be concatenated together");
					} else if (out.count(flag)) {
						throw invalid_argument("Duplicate flag: " + flag.toString());
					} else {
						out.insert({flag, ""});
					}
				}
			}
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
	bool firstNonWSFound = false, lastNonWSFound = false;
	size_t firstNonWS = 0, lastNonWS = str.size() - 1;

	for (size_t i = 0; i < str.size(); i++) {
		if (!firstNonWSFound && str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
			firstNonWS = i;
			firstNonWSFound = true;
		}
		if (!lastNonWSFound && str[str.size() - i - 1] != ' ' && str[str.size() - i - 1] != '\t' && str[str.size() - i - 1] != '\n') {
			lastNonWS = str.size() - i - 1;
			lastNonWSFound = true;
		}
		if (firstNonWSFound && lastNonWSFound) {
			break;
		}
	}

	return out.substr(firstNonWS, lastNonWS - firstNonWS + 1);
}

string stripDirectories(const string& fileName) {
	string out = fileName;
	size_t lastSlash = out.find_last_of('/');

	if (lastSlash != string::npos) {
		out = out.substr(lastSlash + 1);
	}

	return out;
}

string getParentDirectory(const string& fileName) {
	string out = fileName;
	size_t lastSlash = out.find_last_of('/');

	if (lastSlash != string::npos) {
		out = out.substr(0, lastSlash);
	} else {
		out = ".";
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

set<string> readDir(const string& path) {
	set<string> out;
	DIR* dir;
	dirent* entry;

	if ((dir = opendir(path.c_str())) != nullptr) {
		while ((entry = readdir(dir)) != nullptr) {
			if (entry->d_type == DT_REG) {
				string fileName = entry->d_name;

				out.insert(fileName);
			} else if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
				string dirName = entry->d_name;
				set<string> dirContents = readDir(path + "/" + dirName);

				for (const string& nestedFile : dirContents) {
					out.insert(dirName + "/" + nestedFile);
				}
			}
		}
	}

	closedir(dir);

	return out;
}

vector<string> split(const string& str, const string& delimiter) {
	vector<string> out;
	string word;

	for (size_t i = 0; i < str.length(); i++) {
		if (str.substr(i, delimiter.length()) == delimiter) {
			out.push_back(word);
			word = "";
			i += delimiter.length() - 1;
		} else {
			word += str[i];
		}
	}

	out.push_back(word);

	return out;
}

string resolvePath(const string& path) {
	stack<string> pathStack;

	for (const string& dir : split(path, "/")) {
		if (dir == "..") {
			pathStack.pop();
		} else if (dir != "." && dir != "") {
			pathStack.push(dir);
		}
	}

	string out = "";

	while (!pathStack.empty()) {
		out = "/" + pathStack.top() + out;
		pathStack.pop();
	}

	return out.length() <= 1 ? "." : out.substr(1);
}