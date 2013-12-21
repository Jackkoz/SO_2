#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "shared_lib.h"

int main(int arguments_number, char* arguments[])
{
    // Ids of queues used to communicate with server
    int SERVER_OUT, SERVER_REQUEST, SERVER_RELEASE;
    SERVER_OUT = msgget(OUT_KEY, 0);
    SERVER_REQUEST = msgget(REQUEST_KEY, 0);
    SERVER_RELEASE = msgget(RELEASE_KEY, 0);

    int resourceType = atoi(arguments[1]);
    int resourceAmount = atoi(arguments[2]);
    int workTime = atoi(arguments[3]);
    int partnerPID;
    message receivedMessage, requestMessage;
    message *rc_ptr = &receivedMessage, *rq_ptr = &requestMessage;

    requestMessage.type = resourceType;
    sprintf(requestMessage.text, "%d", resourceAmount);

    while (1)
    {
        // Communicate the need for resource
        msgsnd(SERVER_REQUEST, rq_ptr, strlen(requestMessage.text) + 1, 0);

        // Receive co-worker PID; server has locked resources
        // If error then terminate yourself - server has deleted queues
        if (msgrcv(SERVER_OUT, rc_ptr, getpid(), resourceType, 0) == -1)
            exit(0);

        // Obtain PID of partner
        partnerPID = atoi(receivedMessage.text);

        // Communicate start of work
        printf("%d %d %d %d\n", resourceType, resourceAmount, getpid(), partnerPID);

        // Actual work
        sleep(workTime);

        // Coummunicate the release of resources
        msgsnd(SERVER_RELEASE, rq_ptr, strlen(requestMessage.text) + 1, 0);

        // Communicate end of work
        printf("%d KONIEC\n", getpid());
    }

    return 0;
}