#include "threadpool.h"
#include <iostream>
#include <unistd.h>
using namespace std;
bool checkKP(ThreadPool_t *tp);
void stopKP(ThreadPool_t *tp);
bool emptyQueue(ThreadPool_t *tp);
void incTD(ThreadPool_t *tp);
bool checkTD(ThreadPool_t *tp);

////////////////////////////////////////////////////
// ThreadPool_work_queue_t implementation
///////////////////////////////////////////////////

bool ThreadPool_work_queue_t::isEmpty() {
    pthread_mutex_lock(&datamutex);
    bool empty = workVector.empty();
    pthread_mutex_unlock(&datamutex);
    return empty;
}

ThreadPool_work_t ThreadPool_work_queue_t::getJob() {
    pthread_mutex_lock(&datamutex);
    ThreadPool_work_t nextJob = workVector.front();
    workVector.pop();
    pthread_mutex_unlock(&datamutex);
    return nextJob;
}

void ThreadPool_work_queue_t::addToQueue(ThreadPool_work_t newWork) {
    pthread_mutex_lock(&datamutex);
    workVector.push(newWork);
    pthread_mutex_unlock(&datamutex);
}


////////////////////////////////////////////////////
// ThreadPool_t implementation
///////////////////////////////////////////////////


/**
 * @brief creates a new thread pool of num threads
 * @param num - the number of threads that will be created in the threadpool
 * @return ThreadPool_t* - a pointer to the new threadpool
*/
ThreadPool_t* ThreadPool_create(int num) {
    // init instance values of a threadpool
    pthread_mutex_t jobmutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t kpmutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t tdmutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    ThreadPool_work_queue_t workQueue;

    ThreadPool_t *newPool =  new ThreadPool_t {
        num,
        0,
        true,
        cond,
        workQueue,
        // TODO TRY USING A VECTOR OF PTHREADS INSTEAD
        new pthread_t[num],
        jobmutex,
        kpmutex,
        tdmutex
    };

    // create threads
    for (int i = 0; i < num; i++) {
        pthread_create(&newPool->threadPool[i], NULL, (void*(*)(void *))&Thread_run, (void*)newPool);
    }

    return newPool;
}

/**
 * @brief waits for work to complete and release all resources of a threadpool
 * @param tp - the threadpool to be destroyed
*/
void ThreadPool_destroy(ThreadPool_t *tp) {
    // wait till all work has been  completed
    // TODO MAKE THIS PART LESS STUPID
    // cout << "ON THE INSIDE 1" << endl;
    // pthread_mutex_lock(&tp->jobmutex);
    stopKP(tp);
    // pthread_mutex_unlock(&tp->jobmutex);
    pthread_cond_broadcast(&tp->cond);
    // cout << "ON THE INSIDE 2" << endl;
    // while (!checkTD(tp));
    for (int i = 0; i < tp->threadCount; i++) {
        pthread_join(tp->threadPool[i], NULL);
    }
    // cout << "ON THE INSIDE 3" << endl;
    // release resources
    pthread_mutex_destroy(&tp->kpmutex);
    pthread_mutex_destroy(&tp->jobmutex);
    pthread_cond_destroy(&tp->cond);
    // cout << "ON THE INSIDE 4" << endl;
    delete[] tp->threadPool;
    delete tp;
    // cout << "ON THE INSIDE 5" << endl;
}

/**
 * @brief add work to workqueue for a threadpool
 * @param tp - the threadpool to add work to
 * @param func - the function to be executed in the job
 * @param arg - the arguments for the function in the job
 * @return bool - whether adding the job was successful
*/
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg) {
    ThreadPool_work_t newWork = {
        func,
        arg,
    };
    // pthread_mutex_lock(&tp->jobmutex);
    tp->workQueue.addToQueue(newWork);
    // pthread_mutex_unlock(&tp->jobmutex);
    pthread_cond_signal(&tp->cond);
    // signal waiting threads that a new job is available
    return true;
}

/**
 * @brief get the next job in the queue
 * @param tp - the threadpool to get the work from
 * @return ThreadPool_work_t* - pointer to a job 
*/
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp) {
    ThreadPool_work_t *work = new ThreadPool_work_t;
    *work = tp->workQueue.getJob();
    return work;
}  

/**
 * @brief runs thread in a loop until all jobs are finished
 * @param tp - threadpool this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp) {
    while (1) {
        // get job waiting mutex
        pthread_mutex_lock(&tp->jobmutex);
        while(emptyQueue(tp) && checkKP(tp)) {
            pthread_cond_wait(&tp->cond, &tp->jobmutex);
        }
        // check that job processing is still ongoing
        if (!checkKP(tp) && emptyQueue(tp)) {
            break;
        }
        ThreadPool_work_t *work = ThreadPool_get_work(tp);
        // cout << "WHAT THE THREAD SEES: " << (char*)work->arg << endl;
        pthread_mutex_unlock(&tp->jobmutex);
        work->func(work->arg);
        delete work;
    }
    pthread_mutex_unlock(&tp->jobmutex);
    incTD(tp);
    pthread_exit(NULL);
    return nullptr;

}

/**
 * @brief checks that the job processing is still happening
 * @param tp - the threadpool we are checking
 * @return bool - whether the condition is true
*/
bool checkKP(ThreadPool_t *tp) {
    pthread_mutex_lock(&tp->kpmutex);
    bool kp = tp->keepGoing;
    pthread_mutex_unlock(&tp->kpmutex);
    return kp;
}

/**
 * @brief sets keepGoing to false
 * @param tp - the threadpool we want to begin exiting
*/
void stopKP(ThreadPool_t *tp) {
    pthread_mutex_lock(&tp->kpmutex);
    tp->keepGoing = false;
    pthread_mutex_unlock(&tp->kpmutex);
}

/**
 * @brief checks if the work queue is empty
 * @param tp - the threadpool to check
 * @return - bool returns true if the work queue is empty
*/
bool emptyQueue(ThreadPool_t *tp) {
    return tp->workQueue.isEmpty();
}

void incTD(ThreadPool_t *tp) {
    pthread_mutex_lock(&tp->tdmutext);
    tp->threadsDone++;
    pthread_mutex_unlock(&tp->tdmutext);
}

bool checkTD(ThreadPool_t *tp) {
    pthread_mutex_lock(&tp->tdmutext);
    int  n = tp->threadsDone;
    pthread_mutex_unlock(&tp->tdmutext);
    return (tp->threadCount == n);
}


