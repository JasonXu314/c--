#define CATCH_CONFIG_MAIN
#include <lib/catch.hpp>
#include <string>

#include "g--_utils.h"
#include "utils.h"

using namespace std;

TEST_CASE("File Processing", "[files]") {
	SECTION("normalizeFileName-1") {
		string file = "main.cpp";

		REQUIRE(normalizeFileName(file) == "main.cpp");
	}

	SECTION("normalizeFileName-2") {
		string file = "./main.cpp";

		REQUIRE(normalizeFileName(file) == "./main.cpp");
	}

	SECTION("normalizeFileName-3") {
		string file = "main";

		REQUIRE(normalizeFileName(file) == "main.cpp");
	}

	SECTION("normalizeFileName-4") {
		string file = "./main";

		REQUIRE(normalizeFileName(file) == "./main.cpp");
	}

	SECTION("stripExtension-1") {
		string file = "main.cpp";

		REQUIRE(stripExtension(file) == "main");
	}

	SECTION("stripExtension-2") {
		string file = "main";

		REQUIRE(stripExtension(file) == "main");
	}

	SECTION("stripExtension-3") {
		string file = "main.cpp.txt";

		REQUIRE(stripExtension(file) == "main.cpp");
	}

	SECTION("stripDirectories-1") {
		string file = "main.cpp";

		REQUIRE(stripDirectories(file) == "main.cpp");
	}

	SECTION("stripDirectories-2") {
		string file = "./main.cpp";

		REQUIRE(stripDirectories(file) == "main.cpp");
	}

	SECTION("stripDirectories-3") {
		string file = "my/sample/directory/main.cpp";

		REQUIRE(stripDirectories(file) == "main.cpp");
	}

	SECTION("readFile-1") {
		string file = readFile("mocks/files/file-1.txt");

		REQUIRE(file == "Hello World!");
	}

	SECTION("readFile-2") {
		string file = readFile("mocks/files/file-2.txt");

		REQUIRE(file == "\nHello World!\n");
	}

	SECTION("readFile-3") {
		string file = readFile("mocks/files/file-3.txt");

		REQUIRE(file == "\n\n\n\nHello World!\n\n\n\n");
	}

	SECTION("readDir-1") {
		set<string> files = readDir("mocks/readDir-1");

		REQUIRE(files.size() == 2);
		REQUIRE(files.count("1.txt"));
		REQUIRE(files.count("2.txt"));
	}

	SECTION("readDir-2") {
		set<string> files = readDir("mocks/readDir-2");

		REQUIRE(files.size() == 2);
		REQUIRE(files.count("1.txt"));
		REQUIRE(files.count("d1/1.txt"));
	}

	SECTION("resolvePath-1") {
		string path = "my/sample/directory";

		REQUIRE(resolvePath(path) == "my/sample/directory");
	}

	SECTION("resolvePath-2") {
		string path = "./my/sample/directory";

		REQUIRE(resolvePath(path) == "my/sample/directory");
	}

	SECTION("resolvePath-3") {
		string path = "my/sample/../directory";

		REQUIRE(resolvePath(path) == "my/directory");
	}

	SECTION("resolvePath-4") {
		string path = "my/sample/../../directory";

		REQUIRE(resolvePath(path) == "directory");
	}
}

TEST_CASE("Text Processing", "[text]") {
	SECTION("stripWhitespace-1") {
		string file1 = readFile("mocks/files/file-1.txt"), file2 = readFile("mocks/files/file-2.txt"), file3 = readFile("mocks/files/file-3.txt");

		REQUIRE(stripWhitespace(file1) == "Hello World!");
		REQUIRE(stripWhitespace(file2) == "Hello World!");
		REQUIRE(stripWhitespace(file3) == "Hello World!");
	}

	SECTION("stripWhitespace-2") {
		string text = " Hello World! ";

		REQUIRE(stripWhitespace(text) == "Hello World!");
	}

	SECTION("stripWhitespace-3") {
		string text = "\tHello World!\t";

		REQUIRE(stripWhitespace(text) == "Hello World!");
	}

	SECTION("stripWhitespace-4") {
		string text = " \t\n Hello World!\n \t \n";

		REQUIRE(stripWhitespace(text) == "Hello World!");
	}

	SECTION("split-1") {
		string text = "my/sample/directory";
		vector<string> parts = split(text, "/");

		REQUIRE(parts.size() == 3);
		REQUIRE(parts[0] == "my");
		REQUIRE(parts[1] == "sample");
		REQUIRE(parts[2] == "directory");
	}

	SECTION("split-2") {
		string text = "my,text,with,commas";
		vector<string> parts = split(text, ",");

		REQUIRE(parts.size() == 4);
		REQUIRE(parts[0] == "my");
		REQUIRE(parts[1] == "text");
		REQUIRE(parts[2] == "with");
		REQUIRE(parts[3] == "commas");
	}

	SECTION("split-3") {
		string text = "my--text--with--skewer";
		vector<string> parts = split(text, "--");

		REQUIRE(parts.size() == 4);
		REQUIRE(parts[0] == "my");
		REQUIRE(parts[1] == "text");
		REQUIRE(parts[2] == "with");
		REQUIRE(parts[3] == "skewer");
	}
}