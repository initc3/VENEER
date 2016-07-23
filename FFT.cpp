#include <algorithm>
#include <cmath>
#include "Complex.h"
#include "FFT.h"
using namespace std;

const double PI = 3.14159265358979323846264338327950288;

void DFT(Complex P[], int n, int oper) {
	for (int i = 1, j = 0; i < n - 1; i++) {
		for (int s = n; j ^= s >>= 1, ~j & s; );
		if (i < j) swap(P[i], P[j]);
	}
	for (int d = 0; (1 << d) < n; d++) {
		int m = 1 << d, m2 = m * 2;
		float p0 = PI / m * oper;
		Complex unit_p0(cos(p0), sin(p0));
		for (int i = 0; i < n; i += m2) {
			Complex unit(1.0, 0.0);
			for (int j = 0; j < m; j++) {
				Complex &P1 = P[i + j + m], &P2 = P[i + j];
				Complex t = mul(unit, P1);
				P1 = Complex(P2.x - t.x, P2.y - t.y);
				P2 = Complex(P2.x + t.x, P2.y + t.y);
				unit = mul(unit, unit_p0);
			}
		}
		
	}
}


