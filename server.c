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

void SIGINT_handler(int sig)
{
    // Ignoring further SIGINT interrupts
    signal(SIGINT, SIG_IGN);

    // Deleting resource_mutexes and conditions
    // int loop = 0;
    // for (loop; loop < )

    // Deleting conditions

    // Deleting queues
    if (SERVER_REQUEST != -1)
        msgctl(SERVER_REQUEST, IPC_RMID, NULL);
    if (SERVER_RELEASE != -1)
        msgctl(SERVER_RELEASE, IPC_RMID, NULL);
    if (SERVER_OUT != -1)
        msgctl(SERVER_OUT, IPC_RMID, NULL);

    printf("\nSERVER SHUTDOWN\n");

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

    // Check if you can work
    pthread_mutex_lock(args.mutex);
    {
        if (*args.awaiting == 1)
            pthread_cond_wait(args.condition1, args.mutex);

        // Declare awaiting for resources
        *args.awaiting = 1;
    }
    pthread_mutex_unlock(args.mutex);

    // Check for resources
    pthread_mutex_lock(args.mutex);
    {
        while (*args.resource < (args.amount1 + args.amount2))
            pthread_cond_wait(args.condition0, args.mutex);

        // Lock resources
        *args.resource -= (args.amount1 + args.amount2);

        // Notify
        unsigned long my_tid = (unsigned long) pthread_self();
        printf("Wątek %lu przydziela %d zasobów %d klientom %ld %ld, pozostało %d zasobów\n",
            my_tid, args.amount1 + args.amount2, args.resourceType, args.PID1, args.PID2, *args.resource);

        // Free waiting spot, signal someone sleeping on queue
        *args.awaiting = 0;
        pthread_cond_signal(args.condition1);
    }
    pthread_mutex_unlock(args.mutex);

    // Notify clients
    message notification, *n_ptr = &notification;

    notification.PID = args.PID1;
    notification.resourceType = args.PID2;
    if (msgsnd(SERVER_OUT, n_ptr, sizeof(notification) - sizeof(long), 0) == -1)
        serverShutdown();

    notification.PID = args.PID2;
    notification.resourceType = args.PID1;
    if (msgsnd(SERVER_OUT, n_ptr, sizeof(notification) - sizeof(long), 0) == -1)
        serverShutdown();

    // Await the responses, we don't need to actually read them
    if (msgrcv(SERVER_RELEASE, n_ptr, sizeof(notification) - sizeof(long), args.PID1, 0) == -1)
        serverShutdown();
    if (msgrcv(SERVER_RELEASE, n_ptr, sizeof(notification) - sizeof(long), args.PID2, 0) == -1)
        serverShutdown();

    // Release resources and wake thread waiting on premium spot
    pthread_mutex_lock(args.mutex);
    {
        *args.resource += args.amount1 + args.amount2;
        pthread_cond_signal(args.condition0);
    }
    pthread_mutex_unlock(args.mutex);

}

int main(int arguments_number, char* arguments[])
{
    // Setting up SIGINT capturing and processing
    if (signal(SIGINT, SIGINT_handler) == SIG_IGN)

    // Creating communication queues
    SERVER_OUT = msgget(OUT_KEY,  0666 | IPC_CREAT);
    SERVER_REQUEST = msgget(REQUEST_KEY,  0666 | IPC_CREAT);
    SERVER_RELEASE = msgget(RELEASE_KEY,  0666 | IPC_CREAT);

    if (SERVER_OUT == -1 || SERVER_RELEASE == -1 || SERVER_REQUEST == -1)
        serverShutdown();

    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber + 1];
    int loopCounter;

    message request, *rq_ptr = &request;
    int requestingPID, requestedType, requestedAmount;

    thread_arguments thread_input[resourcesNumber + 1], *th_in;

    pthread_cond_t queue_cond[resourcesNumber + 1][2];
    pthread_mutex_t queue_mutex[resourcesNumber + 1];
    int numberOfAwaiting[resourcesNumber + 1][2];

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
        if (pthread_cond_init (&queue_cond[loopCounter][0], NULL) != 0)
            serverShutdown();
        if (pthread_cond_init (&queue_cond[loopCounter][1], NULL) != 0)
            serverShutdown();
    }

    while (1)
    {
        // Read a single request message of any type from queue
        if (msgrcv(SERVER_REQUEST, rq_ptr, sizeof(request) - sizeof(long), 0, 0) == -1)
            serverShutdown();

        requestingPID = request.PID;
        requestedType = request.resourceType;
        requestedAmount = request.resourceAmount;

        if (numberOfAwaiting[requestedType][1] == 0)
        {
            thread_input[requestedType].resourceType = requestedType;
            thread_input[requestedType].amount1 = requestedAmount;
            thread_input[requestedType].PID1 = requestingPID;
            thread_input[requestedType].resource = &resources[requestedType];
            thread_input[requestedType].awaiting = &numberOfAwaiting[requestedType][0];
            thread_input[requestedType].mutex = &queue_mutex[requestedType];
            thread_input[requestedType].condition0 = &queue_cond[requestedType][0];
            thread_input[requestedType].condition1 = &queue_cond[requestedType][1];
            numberOfAwaiting[requestedType][1] = 1;
        }
        else
        {
            thread_input[requestedType].amount2 = requestedAmount;
            thread_input[requestedType].PID2 = requestingPID;
            numberOfAwaiting[requestedType][1] = 0;

            th_in = &thread_input[requestedType];

            // Create the thread for the pair
            if (pthread_create(id_ptr, &attr, thread, (void *)th_in) != 0)
                serverShutdown();
        }
    }

    return 0;
}