
COMP = g++ -Wall -Werror -pthread -O3 -std=c++14 -g -o
OBJ = g++ -Wall -Werror -pthread -O3 -std=c++14 -g -c 
DEB = g++ -Wall -Werror -pthread -fsanitize=thread -O3 -std=c++14 -g -o

default: debug

wc: mapreduce.o threadpool.o distwc.o partitions.o Makefile
	$(COMP) wordcount mapreduce.o threadpool.o distwc.o partitions.o

mapreduce.o: mapreduce.cpp mapreduce.h
	$(OBJ) mapreduce.cpp

threadpool.o: threadpool.cpp threadpool.h
	$(OBJ) threadpool.cpp

partitions.o: partitions.cpp partitions.h
	$(OBJ) partitions.cpp

distwc.o: distwc.cpp
	$(OBJ) distwc.cpp

clean:
	-rm *.o $(objects)
	-rm wordcount

debug:
	$(DEB) wordcount mapreduce.o threadpool.o distwc.o partitions.o



	