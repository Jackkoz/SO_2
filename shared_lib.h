/*
 * Jacek
 * Koziński
 * 334678
 */

#include <pthread.h>

#ifndef _SHARED_LIB_H_
#define _SHARED_LIB_H_

// Server output queue id
#define OUT_KEY 123
// Server input queues id
#define REQUEST_KEY 321
#define RELEASE_KEY 213

// Structure used in the messaging process by clients and server
typedef struct message
{
    long PID;
    int resourceType;
    int resourceAmount;
} message;

// Structure used to pass arguments to threads
typedef struct thread_arguments
{
    int resourceType;
    int amount1, amount2;
    int PID1, PID2;
    int *resource;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} thread_arguments;

#endif