
COMP = g++ -Wall -Werror -pthread -O3 -std=c++14 -g -o
OBJ = g++ -Wall -Werror -pthread -O3 -std=c++14 -g -c 
DEB = g++ -Wall -Werror -pthread -fsanitize=thread -std=c++11 -g -o

default: wc

wc: compile
	$(COMP) wordcount mapreduce.o threadpool.o distwc.o partitions.o

compile: mapreduce.cpp threadpool.cpp distwc.cpp partitions.cpp
	$(OBJ) mapreduce.cpp
	$(OBJ) threadpool.cpp
	$(OBJ) distwc.cpp
	$(OBJ) partitions.cpp

clean:
	-rm *.o $(objects)
	-rm wordcount

debug:
	$(DEB) wordcount mapreduce.o threadpool.o distwc.o partitions.o