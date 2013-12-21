#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "shared_lib.h"

int main(int arguments_number, char* arguments[])
{
    int resourceType = atoi(arguments[1]);
    int resourceAmount = atoi(arguments[2]);
    int workTime = atoi(arguments[3]);
    int partnerPID;
    message receivedMessage, request;
    message *in_ptr = &receivedMessage, *out_ptr = &request;

    request.type = resourceType;
    sprintf(request.text, "%d", resourceAmount);

    while (1)
    {
        // Communicate the need for resource
        msgsnd(SERVER_REQUEST, out_ptr, sizeof(request.text), 0);

        // Receive co-worker PID; server has locked resources
        // If error then terminate yourself - server has deleted queues
        if (msgrcv(SERVER_OUT, in_ptr, 10, resourceType, 0) == -1)
        {
            exit(0);
        }

        // Obtain PID of partner
        partnerPID = atoi(receivedMessage.text);

        // Communicate start of work
        printf("%d %d %d %d\n", resourceType, resourceAmount, getpid(), partnerPID);

        // Actual work
        sleep(workTime);

        // Coummunicate the release of resources
        msgsnd(SERVER_RELEASE, out_ptr, sizeof(request.text), 0);

        // Communicate end of work
        printf("%d KONIEC\n", getpid());
    }

    return 0;
}