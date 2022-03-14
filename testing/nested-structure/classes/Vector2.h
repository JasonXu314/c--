#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>

using namespace std;

class Vector2 {
public:
	Vector2(const int& x, const int& y) : _x(x), _y(y) {}
	Vector2(const Vector2& v) : _x(v._x), _y(v._y) {}

	Vector2 operator+(const Vector2& v) const;

	friend ostream& operator<<(ostream& os, const Vector2& v);

private:
	int _x;
	int _y;
};

#endif