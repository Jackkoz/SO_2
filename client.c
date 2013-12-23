/*
 * Jacek
 * Koziński
 * 334678
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "shared_lib.h"

int main(int arguments_number, char* arguments[])
{
    // Ids of queues used to communicate with server
    int SERVER_OUT, SERVER_REQUEST, SERVER_RELEASE;
    SERVER_OUT = msgget(OUT_KEY, 0666);
    SERVER_REQUEST = msgget(REQUEST_KEY, 0666);
    SERVER_RELEASE = msgget(RELEASE_KEY, 0666);

    int resourceType = atoi(arguments[1]);
    int resourceAmount = atoi(arguments[2]);
    int workTime = atoi(arguments[3]);

    message receivedMessage, requestMessage;
    message *rc_ptr = &receivedMessage, *rq_ptr = &requestMessage;
    long partnerPID, myPID = getpid();

    requestMessage.PID = myPID;
    requestMessage.resourceType = resourceType;
    requestMessage.resourceAmount = resourceAmount;

    while (1)
    {
        // Send request
        if (msgsnd(SERVER_REQUEST, rq_ptr, sizeof(requestMessage) - sizeof(long), 0) != 0)
            exit(0);

        printf("Sent request\n");
        printf("%d %ld\n", SERVER_OUT, myPID);

        // Receive co-worker PID; server has locked resources
        // If error then terminate yourself - server has deleted queues
        if (msgrcv(SERVER_OUT, rc_ptr, myPID, sizeof(receivedMessage) - sizeof(long), 0) == -1)
            exit(0);

        printf("asd\n");

        // Obtain PID of partner
        partnerPID = receivedMessage.resourceType;

        // Communicate start of work
        printf("%d %d %ld %ld\n", resourceType, resourceAmount, myPID, partnerPID);

        // Actual work
        sleep(workTime);

        // Notify the server about the end of work
        if (msgsnd(SERVER_REQUEST, rq_ptr, sizeof(requestMessage) - sizeof(long), 0) != 0)
            exit(0);

        // Communicate end of work
        printf("%d KONIEC\n", getpid());
    }

    return 0;
}