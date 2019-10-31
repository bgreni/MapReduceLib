
COMP = g++ -Wall -Werror -pthread -O3 -std=c++14 -D_GNU_SOURCE  -o
OBJ = g++ -Wall -Werror -pthread -O3 -std=c++14 -D_GNU_SOURCE  -c 
DEB = g++ -Wall -Werror -pthread -O3 -std=c++14 -g -o
SAN =  g++ -Wall -Werror -pthread -fsanitize=thread -O3 -std=c++14 -D_GNU_SOURCE  -o

default: wc

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

clean-result:
	-rm result*

debug: mapreduce.o threadpool.o distwc.o partitions.o Makefile
	$(DEB) wordcount mapreduce.o threadpool.o distwc.o partitions.o

sani: mapreduce.o threadpool.o distwc.o partitions.o Makefile
	$(SAN) wordcount mapreduce.o threadpool.o distwc.o partitions.o



	