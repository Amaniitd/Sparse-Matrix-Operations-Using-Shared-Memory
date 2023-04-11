CC=g++
CFLAGS=-std=c++11 -O3 -fopenmp

sources=attempt2.cpp util.cpp main.cpp 
objects=$(sources:.cpp=.o)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

target:$(objects)
	$(CC) $^ $(CFLAGS) -L. -l:library.so -Wl,-rpath='./' -o exec

run: target
	OMP_NUM_THREADS=8 ./exec bin/test_case_13 tmp
# perf stat ./exec input output_tmp

clean:
	rm *.o exec