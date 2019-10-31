#include "mapreduce.h"
#include "threadpool.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring> 
#include "partitions.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cctype>
#include <map>
using namespace std;

// Global variable declaration
int R;
mypartitions *P;
Reducer reduce;

/** 
 * @brief compares file size of two files
 * @param file1 - name of the first file
 * @param file2 - name of the second file
 * @return bool - whether (size of file1 < size of file2
 * */ 
bool fileCmp(string file1, string file2) {
    struct stat file1stat;
    struct stat file2stat;
    // get file stats
    stat(file1.c_str(), &file1stat);
    stat(file2.c_str(), &file2stat);
    return file1stat.st_size < file2stat.st_size;
}

void printP() {
    cout << "printing partitions" << endl;
    for (size_t i = 0; i < P->partitionList.size(); i++) {
        mypartition p = P->partitionList.at(i);
        cout << "partition number " << i << endl;
        p.keyCount();
    }
}

/**
 * @brief runs the map reduce library with the given functions and inputs
 * @param num_files - the number of input files
 * @param filenames - an array of input file names
 * @param mapper - the user defined function that will perform the mapping 
 * @param num_mappers - the number of mapper threads that will be used
 * @param concate - the user defined function that will perform the reduce
 * @param num_reducers - number of reducer threads that will be used
*/
void MR_Run(int num_files, char *filenames[],
            Mapper mapper, int num_mappers,
            Reducer concate, int num_reducers) {
    
    reduce = concate;
    R = num_reducers;
    // R = 10;
    P = new mypartitions(R);
    // sort the filenames in decreasing order so biggest jobs will go into queue first
    vector<string> files;
    for (int i = 0; i < num_files; i++) {
        // FILE *fp = fopen(filenames[i],"r");
        // cout << (fp == NULL) << endl;
        // fclose(fp);
        files.push_back(string(filenames[i]));
    }

    sort(files.begin(), files.end(), fileCmp);
    
    // for (auto f : files) {
    //     cout << f << endl;
    // }
    // create mapper threads
    ThreadPool_t *mappers = ThreadPool_create(num_mappers);
    // add jobs for all files
    for (size_t i = 0; i < files.size(); i++) {
        ThreadPool_add_work(mappers, (thread_func_t)mapper, (void*)files.at(i).c_str());
        // files.pop_back();
    }
    // cout << "HERE 1" << endl;

    ThreadPool_destroy(mappers);
    // printP();
    // cout << "HERE 2" << endl;

    pthread_t reduceThreads[R];
    for (long int i = 0; i < R; i++) {
        pthread_create(&reduceThreads[i], NULL, (void*(*)(void *))&MR_ProcessPartition, (void*)i);
    }
    // cout << "HERE 3" << endl;

    for (int i = 0; i < R; i++) {
        pthread_join(reduceThreads[i], nullptr);
    }
    // cout << "HERE 4" << endl;
    // ThreadPool_t *reducers = ThreadPool_create(R);
    // for (long int i = 0; i < R; i++) {
    //     ThreadPool_add_work(reducers, (thread_func_t)MR_ProcessPartition, (void*)i);
    // }
    // ThreadPool_destroy(reducers);
    
    delete P;
    // cout << "HERE 5" << endl;
}

/**
 * @brief adds a key value pair to the appropriate partition[haskey]
 * @param key - the key to be hashed
 * @param value - the value of the pair
*/
void MR_Emit(char *key, char *value) {
    unsigned long hashKey = MR_Partition(key, R);
    P->addToPartition(pair<string, string>(string(key), string(value)), hashKey);
}

/**
 * @brief hash function for mapping keys to partitions
 * @param key - the key to be hashed
 * @param nun_partitions - the range of keys that can be produced
*/
unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

/**
 * @brief processes a particular partition, runs the reduce function once per unique key
 * @param partition_number - the partition to be processed
*/
void MR_ProcessPartition(int partition_number) {
    mypartition *currPart = P->getPartition(partition_number);
    // run until the partition is empty
    auto iters = currPart->getIterators();
    for (auto i = iters.first; i != iters.second; i++) {
        char* next = currPart->checkKey(const_cast<char*>(i->first.c_str()), false);
        // cout << "---------------------" << endl;
        // cout << "Before Reduce: " << next << endl;
        if (next != NULL) {
            reduce(next, partition_number);
        }
    }
}

/**
 * @brief gets the next key in the partition
 * @param key - the key being reduced
 * @param partition_number - the partition being processed
 * @return char* - the value of the next key, or NULL if partition is empty of next is different than key
*/
char *MR_GetNext(char *key, int partition_number) {
    mypartition *currPart = P->getPartition(partition_number);
    char* next = currPart->checkKey(key, true);
    // cout << "In Reduce: " << key << " : " << next << endl;
    if (next != NULL && strcmp(next, key) == 0) {
        return next;
    }
    return NULL;
}
