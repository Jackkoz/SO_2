/*
 * Jacek
 * Koziński
 * 334678
 */

#include <stdio.h>      // printf()
#include <pthread.h>    // threads
#include <sys/ipc.h>    // IPC_CREAT
#include <signal.h>     // Processing signals
#include <stdlib.h>     // exit()
#include <string.h>     // strlen()

#include "shared_lib.h"

// Ids of queues used to communicate with server
int SERVER_OUT, SERVER_REQUEST, SERVER_RELEASE;

// Global mutex
pthread_mutex_t mutex;

void SIGINT_handler(int sig)
{
    // Ignoring further SIGINT interrupts
    signal(SIGINT, SIG_IGN);

    // Acquiring and deleting global mutex
    pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);

    // Deleting resource_mutexes

    // Deleting conditions

    // Deleting queues
    if (SERVER_REQUEST != -1)
        msgctl(SERVER_REQUEST, IPC_RMID, NULL);
    if (SERVER_RELEASE != -1)
        msgctl(SERVER_RELEASE, IPC_RMID, NULL);
    if (SERVER_OUT != -1)
        msgctl(SERVER_OUT, IPC_RMID, NULL);

    exit(0);
}

void serverShutdown()
{
    SIGINT_handler(0);
}

void *thread(void *arg)
{
    thread_arguments *arg_ptr = (thread_arguments*) arg;
    thread_arguments args = *arg_ptr;

    // Lock resources on mutex
    pthread_mutex_lock(&mutex);
    {
        *args.resource -= (args.amount1 + args.amount2);

        // Notify
        printf("Wątek %d przydziela %d zasobów %d klientom %d %d, pozostało %d zasobów\n",
            pthread_self(), args.amount1 + args.amount2, args.resourceType, args.PID1, args.PID2, *args.resource);
    }
    pthread_mutex_unlock(&mutex);

    // Notify clients
    message notification, *n_ptr;

    notification.PID = args.PID1;
    notification.resourceType = args.PID2;
    msgsnd(SERVER_OUT, n_ptr, sizeof(notification) - sizeof(long), 0);

    notification.PID = args.PID2;
    notification.resourceType = args.PID1;
    msgsnd(SERVER_OUT, n_ptr, sizeof(notification) - sizeof(long), 0);

    // Await the responses, we don't need to actually read them
    msgrcv(SERVER_RELEASE, n_ptr, args.PID1, sizeof(notification) - sizeof(long), 0);
    msgrcv(SERVER_RELEASE, n_ptr, args.PID2, sizeof(notification) - sizeof(long), 0);

    // Release resources
    pthread_mutex_lock(&mutex);
    {
        *args.resource += args.amount1 + args.amount2;
    }
    pthread_mutex_unlock(&mutex);
}

int main(int arguments_number, char* arguments[])
{
    // Initializing global mutex
    if (pthread_mutex_init(&mutex, NULL) != 0)
        serverShutdown();

    // Setting up SIGINT capturing and processing
    signal(SIGINT, SIGINT_handler);

    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber + 1];
    int loopCounter;

    message request, *rq_ptr = &request;
    int requestingPID, requestedType, requestedAmount;

    thread_arguments thread_input, *th_in = &thread_input;

    pthread_cond_t queue[resourcesNumber + 1][2];
    pthread_mutex_t queue_mutex[resourcesNumber + 1];
    int numberOfAwaiting[resourcesNumber + 1][2];

    // Creating communication queues
    SERVER_OUT = msgget(OUT_KEY,  0666 | IPC_CREAT);
    SERVER_REQUEST = msgget(REQUEST_KEY,  0666 | IPC_CREAT);
    SERVER_RELEASE = msgget(RELEASE_KEY,  0666 | IPC_CREAT);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t thread_id, *id_ptr = &thread_id;


    // Initializing the array of resources and awaiting processes
    for (loopCounter = 0; loopCounter <= resourcesNumber; loopCounter++)
    {
        resources[loopCounter] = resourcesAmount;
        numberOfAwaiting[loopCounter][0] = 0;
        numberOfAwaiting[loopCounter][1] = 0;
        if (pthread_mutex_init(&queue_mutex[loopCounter], NULL) != 0)
            serverShutdown();
        if (pthread_cond_init (&queue[loopCounter][0], NULL) != 0)
            serverShutdown();
        if (pthread_cond_init (&queue[loopCounter][1], NULL) != 0)
            serverShutdown();
    }

    while (1)
    {
        // Read a single request message of any type from queue
        msgrcv(SERVER_REQUEST, rq_ptr, sizeof(request) - sizeof(long), 0, 0);
        requestingPID = rq_ptr->PID;
        requestedType = rq_ptr->resourceType;
        requestedAmount = rq_ptr->resourceAmount;

        if (numberOfAwaiting[requestedType][1] == 0)
        {
            thread_input.resourceType = requestedType;
            thread_input.amount1 = requestedAmount;
            thread_input.PID1 = requestingPID;
            *thread_input.resource = resources[requestedType];
            numberOfAwaiting[requestedType][1] = 1;
        }
        else
        {
            thread_input.amount2 = requestedAmount;
            thread_input.PID2 = requestingPID;
            numberOfAwaiting[requestedType][1] = 0;
            //create thread flag detached
            if (pthread_create(id_ptr, &attr, thread, (void *)th_in) != 0)
            // pthread_create(id_ptr, &attr, thread, (void *)th_in);
                serverShutdown();
        }
    }

    return 0;
}