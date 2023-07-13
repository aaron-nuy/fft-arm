#include "fft.h"
#include <cmath>

static void reverse_bits(std::vector<std::complex<double>>& a) {
    int n = (int)a.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
            std::swap(a[i], a[j]);
    }
}

void fft(std::vector<std::complex<double>>& a, bool invert) {
    int n = (int)a.size();
    reverse_bits(a);

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * M_PI / len * (invert ? 1.0 : -1.0);
        std::complex<double> wlen(cos(ang), sin(ang));
        for (int i = 0; i < n; i += len) {
            std::complex<double> w(1.0, 0.0);
            for (int j = 0; j < len / 2; j++) {
                std::complex<double> u = a[i + j];
                std::complex<double> v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}
