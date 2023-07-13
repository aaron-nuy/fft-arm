#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <complex>
#include <vector>
#include <sys/stat.h>

#include "fft.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const int W = 900;
const int H = 420;

// default function, x goes from 0 to 5
static float f(float x) {
    return 0.05f * x * x * x - 0.3f * x * x + 0.5f * x
        + 0.6f * sinf(2.0f * (float)M_PI * 500.0f * x)
        + 0.3f * sinf(2.0f * (float)M_PI * 1500.0f * x);
}

static void put(unsigned char* px, int x, int y) {
    if (x < 0 || x >= W || y < 0 || y >= H)
        return;
    int i = (y * W + x) * 3;
    px[i] = 40;
    px[i + 1] = 40;
    px[i + 2] = 160;
}

static void draw_line(unsigned char* px, int x0, int y0, int x1, int y1) {
    int steps = abs(x1 - x0);
    if (abs(y1 - y0) > steps)
        steps = abs(y1 - y0);
    if (steps < 1)
        steps = 1;
    for (int s = 0; s <= steps; s++)
        put(px, x0 + (x1 - x0) * s / steps, y0 + (y1 - y0) * s / steps);
}

// quick line plot, autoscaled
static void plot(const std::vector<float>& y, const char* path) {
    if (y.empty())
        return;

    float lo = y[0], hi = y[0];
    for (size_t i = 0; i < y.size(); i++) {
        if (y[i] < lo) lo = y[i];
        if (y[i] > hi) hi = y[i];
    }
    if (hi - lo < 1e-9f)
        hi = lo + 1.0f;

    unsigned char* px = new unsigned char[W * H * 3];
    memset(px, 255, W * H * 3);

    int m = y.size() > 1 ? (int)y.size() - 1 : 1;
    int prev_x = -1, prev_y = -1;
    for (size_t i = 0; i < y.size(); i++) {
        int x = (int)(i * (W - 1) / m);
        int yv = H - 1 - (int)((y[i] - lo) / (hi - lo) * (H - 20) + 10);
        if (prev_x >= 0)
            draw_line(px, prev_x, prev_y, x, yv);
        prev_x = x;
        prev_y = yv;
    }

    stbi_write_png(path, W, H, 3, px, W * 3);
    delete[] px;
    printf("wrote %s\n", path);
}

int main() {
    const int n = 65536;
    std::vector<std::complex<double>> a(n);
    for (int i = 0; i < n; i++) {
        float x = i * 5.0f / (n - 1);
        a[i] = f(x);
    }

    mkdir("out", 0755);

    std::vector<float> orig(n);
    for (int i = 0; i < n; i++)
        orig[i] = (float)a[i].real();
    plot(orig, "out/orig.png");

    fft(a, false);

    std::vector<float> mag;
    for (int i = 0; i < n / 2; i++)
        mag.push_back((float)std::abs(a[i]));
    plot(mag, "out/spectrum.png");

    fft(a, true);

    std::vector<float> back(n);
    for (int i = 0; i < n; i++)
        back[i] = (float)a[i].real();
    plot(back, "out/roundtrip.png");

    return 0;
}
