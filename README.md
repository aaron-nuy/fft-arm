simple 1d fft in c++ for aarch64, tested with qemu-user

    make build
    make run

run ./fft with a file of comma separated numbers, or with no arguments
to use the default function (x from 0 to 5, 50k samples, zero padded
to the next power of two)

writes 3 pngs to out/: the input, the roundtrip after fft + inverse
fft, and the frequency domain
