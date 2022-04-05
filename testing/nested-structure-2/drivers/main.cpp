#include "../headers/utils.h"
#include "../impl/template.hpp"

#include <iostream>

using namespace std;

int main() {
	Container<int> c(1);

	cout << c.get() << endl;
	
	return 0;
}