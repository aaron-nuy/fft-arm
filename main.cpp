#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <complex>
#include <string>
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

static int next_pow2(int n) {
    int p = 1;
    while (p < n)
        p <<= 1;
    return p;
}

// one big comma separated list of numbers
static std::vector<float> read_numbers(const char* path) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "cant open %s\n", path);
        exit(1);
    }

    std::vector<float> out;
    std::string tok;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == ',' || c == '\n' || c == '\r') {
            if (!tok.empty()) {
                out.push_back((float)atof(tok.c_str()));
                tok.clear();
            }
        } else {
            tok += (char)c;
        }
    }
    if (!tok.empty())
        out.push_back((float)atof(tok.c_str()));
    fclose(fp);

    if (out.empty()) {
        fprintf(stderr, "no numbers in %s\n", path);
        exit(1);
    }
    return out;
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

int main(int argc, char** argv) {
    std::vector<float> sig;
    int n_orig;

    if (argc > 1) {
        sig = read_numbers(argv[1]);
    } else {
        for (int i = 0; i < 50000; i++)
            sig.push_back(f(i * 5.0f / 49999.0f));
    }
    n_orig = (int)sig.size();

    // zero pad to the next power of two
    int n = next_pow2(n_orig);
    sig.resize(n, 0.0f);
    printf("samples %d, padded to %d\n", n_orig, n);

    mkdir("out", 0755);

    std::vector<float> orig(sig.begin(), sig.begin() + n_orig);
    plot(orig, "out/orig.png");

    std::vector<std::complex<double>> a(n);
    for (int i = 0; i < n; i++)
        a[i] = sig[i];
    fft(a, false);

    // magnitude of the first half
    std::vector<float> mag;
    for (int i = 0; i < n / 2; i++)
        mag.push_back((float)std::abs(a[i]));
    plot(mag, "out/spectrum.png");

    fft(a, true);

    std::vector<float> back(n_orig);
    for (int i = 0; i < n_orig; i++)
        back[i] = (float)a[i].real();

    float err = 0.0f;
    for (int i = 0; i < n_orig; i++) {
        float d = fabsf(orig[i] - back[i]);
        if (d > err)
            err = d;
    }
    printf("max diff after fft + inverse: %g\n", err);

    plot(back, "out/roundtrip.png");
    return 0;
}
