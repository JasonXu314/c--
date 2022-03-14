#include "Vector2.h"

Vector2 Vector2::operator+(const Vector2& v) const { return Vector2(_x + v._x, _y + v._y); }

ostream& operator<<(ostream& os, const Vector2& v) { return os << "<" << v._x << ", " << v._y << ">"; }