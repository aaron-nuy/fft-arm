#include <cstdio>
#include <cmath>
#include <complex>
#include <vector>

#include "fft.h"

// default function, x goes from 0 to 5
static float f(float x) {
    return 0.05f * x * x * x - 0.3f * x * x + 0.5f * x
        + 0.6f * sinf(2.0f * (float)M_PI * 500.0f * x)
        + 0.3f * sinf(2.0f * (float)M_PI * 1500.0f * x);
}

int main() {
    const int n = 65536;
    std::vector<std::complex<double>> a(n);
    for (int i = 0; i < n; i++) {
        float x = i * 5.0f / (n - 1);
        a[i] = f(x);
    }

    fft(a, false);

    // peek at the first bins
    for (int i = 0; i < 20; i++)
        printf("%d %f\n", i, std::abs(a[i]));

    return 0;
}
