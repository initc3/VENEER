
CXX = g++
CFLAGS = -Wall -O3

all: main.o
	$(CXX) *.o -o main $(CFLAGS)

main.o: Layers.o sha256.o NDArray.o Model.o
	$(CXX) main.cpp -c $(CFLAGS)

sha256.o:
	$(CXX) sha256.cpp -c $(CFLAGS)

Complex.o:
	$(CXX) Complex.cpp -c $(CFLAGS)

NDArray.o:
	$(CXX) NDArray.cpp -c $(CFLAGS)

FFT.o:
	$(CXX) FFT.cpp -c $(CFLAGS)

Layers.o: Layer.o Conv.o Bias.o ReLU.o Pool.o Flatten.o MatMul.o
	$(CXX) Layers.cpp -c $(CFLAGS)

Layer.o: NDArray.o
	$(CXX) Layer.cpp -c $(CFLAGS)

Conv.o: Layer.o Complex.o FFT.o
	$(CXX) Conv.cpp -c $(CFLAGS)

Bias.o: Layer.o
	$(CXX) Bias.cpp -c $(CFLAGS)

ReLU.o: Layer.o
	$(CXX) ReLU.cpp -c $(CFLAGS)

Pool.o: Layer.o
	$(CXX) Pool.cpp -c $(CFLAGS)

Flatten.o: Layer.o
	$(CXX) Flatten.cpp -c $(CFLAGS)

MatMul.o: Layer.o
	$(CXX) MatMul.cpp -c $(CFLAGS)

Model.o:
	$(CXX) Model.cpp -c $(CFLAGS)

clean:
	rm -f *.o main

