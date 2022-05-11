#define CATCH_CONFIG_MAIN

#include <lib/catch.hpp>

#include "header.h"
#include "nested/header.h"

TEST_CASE("Main Header", "[main]") {
	SECTION("Basic") {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				REQUIRE(add(i, j) == i + j);
			}
		}
	}
}

TEST_CASE("Nested Header", "[nested]") {
	SECTION("Basic") {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				REQUIRE(subtract(i, j) == i - j);
			}
		}
	}
}