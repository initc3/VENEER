
#ifndef __COMPLEX_H
#define __COMPLEX_H

struct Complex {
	float x, y;

	Complex() : x(0.0), y(0.0) {}
	Complex(float x) : x(x), y(0.0) {}
	Complex(float x, float y): x(x), y(y) {}
};

inline Complex mul(const Complex &a, const Complex &b) {
	return Complex(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

inline Complex add(const Complex &a, const Complex &b) {
	return Complex(a.x + b.x, a.y + b.y);
}

inline Complex sub(const Complex &a, const Complex &b) {
	return Complex(a.x - b.x, a.y - b.y);
}

#endif

