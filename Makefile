CXX = aarch64-linux-gnu-g++
CXXFLAGS = -O2 -std=c++17 -static -Wall

build: main.o fft.o
	$(CXX) $(CXXFLAGS) -o fft main.o fft.o

main.o: main.cpp fft.h stb_image_write.h
	$(CXX) $(CXXFLAGS) -c main.cpp

fft.o: fft.cpp fft.h
	$(CXX) $(CXXFLAGS) -c fft.cpp

run: build
	qemu-aarch64 ./fft

clean:
	rm -f fft *.o
