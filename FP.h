#ifndef __FP_H
#define __FP_H

#include <iostream>
#include <climits>

const int NUM_Q = 40;
const long long TWO_TO_Q = 1ULL << 40;

struct FP {
	long long n; // a.n = a * (2 ^ NUM_Q)

	FP(): n(0) {
	}

	static inline FP from(float x) {
		return FP((long long)(x * (1ULL << NUM_Q)));
	}

	static inline FP internal_constructor(long long x) {
		return FP(x);
	}

protected:
	FP(long long n): n(n) {
	}

};


inline FP operator + (const FP &a, const FP &b) {
	return FP :: internal_constructor(a.n + b.n);
}

inline FP operator - (const FP &a, const FP &b) {
	return FP :: internal_constructor(a.n - b.n);
}

inline FP operator * (const FP &a, const FP &b) { // (a * b).n = a * b * (2 ^ NUM_Q) = a.n * b.n / 2 ^ Q
	__int128_t result = a.n; // TODO: may not compile in some compiler / platform
	result *= b.n;
	result /= TWO_TO_Q;
	if (result > LONG_LONG_MAX)
		result %= LONG_LONG_MAX; // should not happen
	return FP :: internal_constructor((long long)result);
}

inline FP operator / (const FP &a, long long d) {
	return FP :: internal_constructor(a.n / d);
}

inline FP operator += (FP &a, const FP &b) {
	a.n += b.n;
	return a;
}

inline FP operator -= (FP &a, const FP &b) {
	a.n -= b.n;
	return a;
}

inline FP operator - (const FP &a) {
	return FP :: internal_constructor(-a.n);
}

inline FP operator *= (FP &a, const FP &b) { // (a * b).n = a * b * (2 ^ NUM_Q) = a.n * b.n / 2 ^ Q
	__int128_t result = a.n; // TODO: may not compile in some compiler / platform
	result *= b.n;
	result /= TWO_TO_Q;
	if (result > LONG_LONG_MAX)
		result %= LONG_LONG_MAX; // should not happen
	a.n = result;
	return a;
}

inline FP operator /= (FP &a, long long d) {
	a.n /= d;
	return a;
}

inline std :: ostream& operator << (std :: ostream &os, const FP &a) {
	long double n = a.n;
	n /= (long double) (TWO_TO_Q);
	os << n;
	return os;
}

inline std :: istream& operator >> (std :: istream &is, FP &a) {
	float x;
	is >> x;
	a = FP :: from(x);
	return is;
}

inline bool operator < (const FP &a, const FP &b) {
	return a.n < b.n;
}

inline bool operator > (const FP &a, const FP &b) {
	return a.n > b.n;
}

inline bool operator <= (const FP &a, const FP &b) {
	return a.n <= b.n;
}

inline bool operator >= (const FP &a, const FP &b) {
	return a.n >= b.n;
}

inline bool operator == (const FP &a, const FP &b) {
	return a.n == b.n;
}

inline bool operator != (const FP &a, const FP &b) {
	return a.n != b.n;
}

#endif

