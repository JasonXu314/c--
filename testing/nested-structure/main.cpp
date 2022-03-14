#include <iostream>

#include "classes/Vector2.h"
#include "structs/Point.h"

using namespace std;

int main() {
	Vector2 v1(1, 2);
	Vector2 v2(3, 4);
	Vector2 v3 = v1 + v2;

	cout << "v1: " << v1 << endl;
	cout << "v2: " << v2 << endl;
	cout << "v3: " << v3 << endl;

	return 0;
}