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
    // Deleting queues
    if (SERVER_REQUEST != -1)
        msgctl(SERVER_REQUEST, IPC_RMID, NULL);
    if (SERVER_RELEASE != -1)
        msgctl(SERVER_RELEASE, IPC_RMID, NULL);
    if (SERVER_OUT != -1)
        msgctl(SERVER_OUT, IPC_RMID, NULL);

    printf("\n***\nServer shutdown\n***\n");
    exit(0);
}

void thread(int resourceType, int amount1, int amount2, int PID1, int PID2, int resources[])
{
    // Lock resources on mutex
    pthread_mutex_lock(&mutex);
    {
        resources[resourceType] -= (amount1 + amount2);

        // Notfiy
        printf("Wątek %d przydziela %d zasobów %d klientom %d %d, pozostało %d zasobów\n",
            pthread_self(), amount1 + amount2, resourceType, PID1, PID2, resources[resourceType]);
    }
    pthread_mutex_unlock(&mutex);

    // Notify clients
    message notification, *n_ptr;
    notification.type = PID1;
    sprintf(notification.text, "%d", PID2);
    msgsnd(SERVER_OUT, n_ptr, strlen(notification.text) + 1, 0);
    notification.type = PID2;
    sprintf(notification.text, "%d", PID1);
    msgsnd(SERVER_OUT, n_ptr, strlen(notification.text) + 1, 0);

    // Await the responses, we don't actually need to read them
    msgrcv(SERVER_RELEASE, n_ptr, PID1, resourceType, 0);
    msgrcv(SERVER_RELEASE, n_ptr, PID2, resourceType, 0);

    // Release resources
    pthread_mutex_lock(&mutex);
    {
        resources[resourceType] += amount1 + amount2;
    }
    pthread_mutex_unlock(&mutex);

    exit(0);
}

int main(int arguments_number, char* arguments[])
{
    // Setting up SIGINT capturing and processing
    if (signal (SIGINT, SIGINT_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);

    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber + 1], loopCounter;
    message request, *rq_ptr = &request;

    // Creating communication queues
    SERVER_OUT = msgget(OUT_KEY,  0666 | IPC_CREAT);
    SERVER_REQUEST = msgget(REQUEST_KEY,  0666 | IPC_CREAT);
    SERVER_RELEASE = msgget(RELEASE_KEY,  0666 | IPC_CREAT);

    // Initializing the array of resources
    for (loopCounter = 0; loopCounter < resourcesNumber; loopCounter++)
    {
        resources[loopCounter] = resourcesAmount;
    }

    while (1)
    {
        // *** DEBUG ***
            // printf("Server running\n");
        // *** DEBUG ***

        // Read a single request message of any type from queue
        msgrcv(SERVER_REQUEST, rq_ptr, 10, 0, 0);

        // Process request message
        // Create a thread only for a pair ready to work
        // Else manage awaiting threads

        // Read release message from queue while the queue<type> is not empty
        // Done by thread
    }

    return 0;
}