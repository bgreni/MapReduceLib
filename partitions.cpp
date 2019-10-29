#include "partitions.h"
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief compares the key of two pairs alphabetically
 * @param pair1 - the first pair to be compared
 * @param pair2 - the second pair to be compared
 * @return bool - whether key1 < key2
*/
bool pairComp(pair<string, string> pair1, pair<string, string> pair2) {
    return pair1.first < pair2.first;
}

////////////////////////////////////////////////////
// mypartition implementation
///////////////////////////////////////////////////

/**
 * @brief inserts a new key,value pair into the partition, then sorts it into increasing order
 * @param pair - the new pair to be inserted into the partition
*/
void mypartition::insertPair(pair<string, string> pair) {
    pthread_mutex_lock(&mutex);
    // cout << "------------------------------------" << endl;
    // cout  << "TID: " << pthread_self() << endl;
    // cout << "PUTTING PAIR INTO PARTITION" << endl;
    // cout << pair.first << " : " << pair.second << endl;
    // cout << "PARTITION BEFORE" << endl;
    // keyCount();
    if (data.find(pair.first) != data.end()) {
        data[pair.first].push_back(pair.second);
    } else {
        data[pair.first] = list<string>();
        data[pair.first].push_back(pair.second);
    }
    // cout << "PARITION AFTER" << endl;
    // keyCount();
    // cout << "------------------------------------" << endl;
    pthread_mutex_unlock(&mutex);
}


/**
 * @brief gives the number of unique keys in the partition
 * @return int - number of unique keys
*/
int mypartition::keyCount() {
    for (auto it : data) {
        cout << it.first << " : " << it.second.size() << endl;
    }
    return 1;
}

pair<map<string, list<string>>::iterator, map<string, list<string>>::iterator> mypartition::getIterators() {
    return pair<map<string, list<string>>::iterator, map<string, list<string>>::iterator>(data.begin(), data.end());
} 

/**
 * @brief returns the top item in the parition, but does not remove it
 * @return const char* - the top item in the parition, or NULL if the partition is empty
*/
char* mypartition::checkKey(char* key, bool popItem) {
    string s = string(key);
    if (data[s].empty()) {
        return NULL;
    }
    if (popItem) {
        data[s].pop_back();
    }
    // convert c++ string to c-style char* string
    return key;
}


////////////////////////////////////////////////////
// mypartitions implementation
///////////////////////////////////////////////////

mypartitions::mypartitions() {}

/**
 * @brief constructor for partitions object
 * @param numPartitions - the number of paritions contained in this object
*/
mypartitions::mypartitions(int numPartitions) {
    for (int i = 0; i < numPartitions; i++) {
        partitionList.push_back(mypartition());
    }
}

/**
 * @brief add a new pair to a particular partition
 * @param pair - the new pair
 * @param paritionNum - the parition the pair will be inserted into
*/

pthread_mutex_t t = PTHREAD_MUTEX_INITIALIZER;

void mypartitions::addToPartition(pair<string, string> pair, int partitionNum) {
    // pthread_mutex_lock(&t);
    partitionList.at(partitionNum).insertPair(pair);
    // pthread_mutex_unlock(&t);
}

/**
 * @brief gives a pointer to a paritcular parition
 * @param num - the number of the parition requested
 * @return mypartition* - a pointer to parition number num
*/
mypartition* mypartitions::getPartition(int num) {
    // get an iterator to first partition and increment it to partition num
    auto it = partitionList.begin();
    it += num;
    // dereference the iterator and return the address of the partition it was pointing to
    // or whatever cause iterators are weird
    return &*it;
}