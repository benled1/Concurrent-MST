CC = g++
CFLAGS = -std=c++14 -O3 -pthread
INCLUDES = -I./utils

all: mst_mpi mst_thread mst_serial

mst_mpi: mst_mpi.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

mst_thread: mst_thread.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

mst_serial: mst_serial.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f mst_mpi mst_thread mst_serial
