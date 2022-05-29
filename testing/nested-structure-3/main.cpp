#include <iostream>

#include "nested/test.h"

using namespace std;

int main() {
	if (testAdd(1, 2)) {
		cout << "Test passed" << endl;
	} else {
		cout << "Test failed" << endl;
	}

	return 0;
}