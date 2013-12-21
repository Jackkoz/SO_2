#include <stdio.h>      // printf, scanf
#include <pthread.h>    // threads
#include <sys/ipc.h>    // IPC_CREAT
#include <signal.h>     // Processing signals
#include <stdlib.h>     // exit()

#include "shared_lib.h"

void SIGINT_handler(int sig)
{
    // Deleting queues
    if (SERVER_REQUEST != -1)
        msgctl(SERVER_REQUEST, IPC_RMID, NULL);
    if (SERVER_RELEASE != -1)
        msgctl(SERVER_RELEASE, IPC_RMID, NULL);
    if (SERVER_OUT != -1)
        msgctl(SERVER_OUT, IPC_RMID, NULL);

    printf("Shutting down server\n");
    exit(0);
}

int main(int arguments_number, char* arguments[])
{
    // Setting up SIGINT processing
    if (signal (SIGINT, SIGINT_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);

    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber], loopCounter;

    // Declare input queue
    SERVER_OUT = msgget(OUT_KEY, IPC_CREAT);
    // Declare output queue
    SERVER_REQUEST = msgget(REQUEST_KEY, IPC_CREAT);
    SERVER_RELEASE = msgget(RELEASE_KEY, IPC_CREAT);

    // Initializing the array of resources
    for (loopCounter = 0; loopCounter < resourcesNumber; loopCounter++)
    {
        resources[loopCounter] = resourcesAmount;
    }

    while (1)
    {
        // *** DEBUG ***
            printf("Server running\n");
            sleep(1);
        // *** DEBUG ***

        // Main body

        // Read a single request message from queue

        // Process request message

        // Read release message from queue while the queue<type> is not empty

        // Process release message
    }

    return 0;
}