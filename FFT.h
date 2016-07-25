#ifndef __FFT_H
#define __FFT_H

#include "Complex.h"

#ifdef MEASURE_COMPUTATION
extern int cnt_FFT;
#endif
void DFT(Complex P[], int n, int oper);

#endif

