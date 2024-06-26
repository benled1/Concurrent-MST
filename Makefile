CC = g++
MPICXX = mpic++
CFLAGS = -std=c++11 -Wall -pthread
CXXFLAGS = -std=c++14 -O3 -pthread
INCLUDES = -I./utils

all: mst_mpi mst_thread mst_serial

mst_mpi: mst_mpi.cpp 
	$(MPICXX) $(CFLAGS) $(INCLUDES) $^ -o $@
	
mst_serial: mst_serial.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

mst_thread: mst_thread.cpp 
	$(CC) $(CXXFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f mst_mpi mst_thread mst_serial