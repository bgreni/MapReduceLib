CC:=g++
WARN:=-Wall 
LIB:=-pthread -O3
CCOPTS:=-std=c++14 -D_GNU_SOURCE
TARGET:=distwc

$(TARGET): ../threadpool.o ../mapreduce.o distwc.o ../partitions.o
	$(CC) $(WARN) $(CCOPTS) $^ -o $@ $(LIB)
	g++ -o validation validation.cc -std=c++14 -Wall -O3

%.o: %.c
	$(CC) $(WARN) $(CCOPTS) $< -c $(LIB)

clean-result: 
	rm -rf result-*.txt
clean-all:
	rm -rf *.o *.gch $(TARGET)
	rm -rf result-*.txt
	rm -rf validation

threadpool.o: ../threadpool.cpp ../threadpool.h
distwc.o: distwc.c ../mapreduce.h
mapreduce.o: ../mapreduce.cpp ../mapreduce.h ../threadpool.h
partitions.o: ../partitions.cpp ../partitions.h

