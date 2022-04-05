#ifndef TEMPLATE_H
#define TEMPLATE_H

template <class T>
class Container {
public:
	Container(const T& t) : _t(t) {}

	T get() const;
	
private:
	T _t;
};

#endif