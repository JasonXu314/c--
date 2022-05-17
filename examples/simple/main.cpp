#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	for (int i = 1; i <= 100; i++) {
		if (i % 15 == 0) {
			cout << (argc > 1 ? argv[1] : "Fizz") << (argc > 2 ? argv[2] : "Buzz") << endl;
		} else if (i % 3 == 0) {
			cout << (argc > 1 ? argv[1] : "Fizz") << endl;
		} else if (i % 5 == 0) {
			cout << (argc > 2 ? argv[2] : "Buzz") << endl;
		} else {
			cout << i << endl;
		}
	}

	return 0;
}