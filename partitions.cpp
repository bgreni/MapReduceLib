#include "partitions.h"
#include <algorithm>
#include <iostream>
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
    data.push_back(pair);
    data.sort();
    pthread_mutex_unlock(&mutex);
}

/**
 * @brief removes the top item from the parition
*/
void mypartition::popNext() {
    data.pop_front();
}

/**
 * @brief checks if all items have been been removed from the partition
 * @return bool - if the parition is empty
*/
bool mypartition::partitionDone() {
    return data.empty();
}

/**
 * @brief returns the top item in the parition, but does not remove it
 * @return const char* - the top item in the parition, or NULL if the partition is empty
*/
const char* mypartition::peekNext() {
    if (data.empty()) {
        return NULL;
    }
    // convert c++ string to c-style char* string
    return data.front().first.c_str();
}


////////////////////////////////////////////////////
// mypartitions implementation
///////////////////////////////////////////////////

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
void mypartitions::addToPartition(pair<string, string> pair, int partitionNum) {
    partitionList.at(partitionNum).insertPair(pair);
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