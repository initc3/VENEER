
#ifndef __COMPLEX_H
#define __COMPLEX_H

#include "FP.h"

struct Complex {
	FP x, y;

	Complex() : x(), y() {}
	Complex(const FP &x) : x(x), y() {}
	Complex(const FP &x, const FP &y): x(x), y(y) {}
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

