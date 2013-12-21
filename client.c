#include <stdio.h>
#include <pthread.h>
#include "shared_lib.h"

int main(int arguments_number, char* arguments[])
{
    int resourceType = atoi(arguments[1]);
    int resourceAmount = atoi(arguments[2]);
    // n – żądana liczba sztuk zasobu (1 <= n <= podłoga(N/2))
    int workTime = atoi(arguments[3]);
    int partnerPID;

    while (1)
    {
        // Communicate the need for resource

        // Sleep on condition until woken up to work
        // Or declare enough types to sleep on reading from queue

        // Receive co-worker PID, server has locked resources
        // partnerPID = recieve();

        // Communicate start of work
        printf("%d %d %d %d\n", resourceType, resourceAmount, getpid(), partnerPID);

        // Actual work
        sleep(workTime);

        // Coummunicate the release of resources

        // Communicate end of work
        printf("%d KONIEC\n", getpid());
    }

    return 0;
}