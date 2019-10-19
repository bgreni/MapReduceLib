#pragma once

#include <list>
#include <vector>
#include <utility> 
#include <string>
#include <pthread.h>
using namespace std;

class mypartition {
    private:
        pthread_mutex_t mutex;
    public:
        list<pair<string, string>> data;
        void insertPair(pair<string, string> pair);
        void popNext();
        const char* peekNext();
        bool partitionDone();
};


class mypartitions {
    private:
    public:
        vector<mypartition> partitionList;
        mypartitions(int numPartitions);
        void addToPartition(pair<string, string> pair, int partitionNum);
        mypartition* getPartition(int num);
};