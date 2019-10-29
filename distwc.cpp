#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "mapreduce.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <chrono>
using namespace std;

void Map(char *file_name) {
    // cout << file_name << endl;
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);
    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL)
            MR_Emit(token, (char*)"1");
    }
    free(line);
    fclose(fp);
}

void Reduce(char *key, int partition_number) {
    // cout << "In Reduce: " << key << endl;
    int count = 0;
    char *value, name[100];
    while ((value = MR_GetNext(key, partition_number)) != NULL)
        count++;
    sprintf(name, "result-%d.txt", partition_number);
    FILE *fp = fopen(name, "a");
    printf("%s: %d\n", key, count);
    fprintf(fp, "%s: %d\n", key, count);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    auto start = chrono::high_resolution_clock::now(); 
    MR_Run(argc - 3, &(argv[3]), Map, atoi(argv[1]), Reduce, atoi(argv[2]));
    // MR_Run(argc - 1, &(argv[1]), Map, 10, Reduce, 10);
    auto stop = chrono::high_resolution_clock::now(); 
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); 
    cout << duration.count() << endl;
}
