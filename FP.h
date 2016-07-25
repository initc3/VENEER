#ifndef __FP_H
#define __FP_H

#include <iostream>
#include <climits>

struct FP {

	typedef int DATA_TYPE;
	static const int NUM_Q = 16;
	static const DATA_TYPE TWO_TO_Q = (DATA_TYPE)1 << NUM_Q;

#ifdef MEASURE_COMPUTATION
	static int cnt_add;
	static int cnt_sub;
	static int cnt_mul;
	static int cnt_div;
	static int cnt_cmp;
#endif

	DATA_TYPE n; // a.n = a * (2 ^ NUM_Q)

	FP(): n(0) {
	}

	static inline FP from(float x) {
		return FP((DATA_TYPE)(x * TWO_TO_Q));
	}

	static inline FP internal_constructor(DATA_TYPE x) {
		return FP(x);
	}

protected:
	FP(DATA_TYPE n): n(n) {
	}

};


inline FP operator + (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_add;
#endif
	return FP :: internal_constructor(a.n + b.n);
}

inline FP operator - (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_sub;
#endif
	return FP :: internal_constructor(a.n - b.n);
}

inline FP operator * (const FP &a, const FP &b) { // (a * b).n = a * b * (2 ^ NUM_Q) = a.n * b.n / 2 ^ Q
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_mul;
#endif
	long long result = a.n; // TODO: may not compile in some compiler / platform
	result *= b.n;
	result >>= FP :: NUM_Q;
	return FP :: internal_constructor( (long long) result );
}

inline FP operator / (const FP &a, FP :: DATA_TYPE d) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_div;
#endif
	return FP :: internal_constructor(a.n / d);
}

inline FP operator += (FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_add;
#endif
	a.n += b.n;
	return a;
}

inline FP operator -= (FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_sub;
#endif
	a.n -= b.n;
	return a;
}

inline FP operator - (const FP &a) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_sub;
#endif
	return FP :: internal_constructor(-a.n);
}

inline FP operator /= (FP &a, FP :: DATA_TYPE d) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_div;
#endif
	a.n /= d;
	return a;
}

inline std :: ostream& operator << (std :: ostream &os, const FP &a) {
	long double n = a.n;
	n /= (long double) (FP :: TWO_TO_Q);
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
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n < b.n;
}

inline bool operator > (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n > b.n;
}

inline bool operator <= (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n <= b.n;
}

inline bool operator >= (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n >= b.n;
}

inline bool operator == (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n == b.n;
}

inline bool operator != (const FP &a, const FP &b) {
#ifdef MEASURE_COMPUTATION
	++ FP :: cnt_cmp;
#endif
	return a.n != b.n;
}

#endif

