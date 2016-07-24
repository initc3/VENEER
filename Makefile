
CXX = g++
CXXFLAGS = -Wall -O2

all: main

main:  Layers.o sha256.o NDArray.o Model.o main.cpp
	$(CXX) *.o main.cpp $(CXXFLAGS) -o main

%.o: %.cpp %.h
	$(CXX) $< $(CXXFLAGS) -c

FFT.o: Complex.o

Layers.o: Layer.o Conv.o Bias.o ReLU.o Pool.o Flatten.o MatMul.o

Layer.o: NDArray.o

Conv.o: Layer.o FFT.o

Bias.o: Layer.o

ReLU.o: Layer.o

Pool.o: Layer.o

Flatten.o: Layer.o

MatMul.o: Layer.o

Model.o: Layers.o

Complex.o: FP.o

NDArray.o: FP.o

clean:
	rm -f *.o main

