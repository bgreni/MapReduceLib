#pragma once

#include <list>
#include <vector>
#include <utility> 
#include <string>
#include <pthread.h>
#include <map>
using namespace std;

// struct cmp_str
// {
//    bool operator()(char const *a, char const *b) const
//    {
//       return strcmp(a, b) < 0;
//    }
// };


class mypartition {
    private:
        pthread_mutex_t mutex;
    public:
        map<string, list<string>> data;
        void insertPair(pair<string, string> pair);
        char* checkKey(char* key, bool popItem);
        int keyCount();
        pair<map<string, list<string>>::iterator, map<string, list<string>>::iterator> getIterators();
};


class mypartitions {
    private:
    public:
        mypartitions();
        vector<mypartition> partitionList;
        mypartitions(int numPartitions);
        void addToPartition(pair<string, string> pair, int partitionNum);
        mypartition* getPartition(int num);
};