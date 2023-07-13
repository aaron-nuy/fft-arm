#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>

// in place radix 2 cooley tukey, size has to be a power of two
void fft(std::vector<std::complex<double>>& a, bool invert);

#endif
