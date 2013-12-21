#include <stdio.h>
#include <pthread.h>
#include "shared_lib.h"

int main(int arguments_number, char* arguments[])
{
    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber], loopCounter;

    // Declare input queue

    // Declare output queue

    // Initializing the array of resources
    for (loopCounter = 0; loopCounter < resourcesNumber; loopCounter++)
    {
        resources[loopCounter] = resourcesAmount;
    }

    while (1)
    {
        // Main body

        // Read a single request message from queue

        // Process request message

        // Read release message from queue while the queue<type> is not empty

        // Process release message
    }

    return 0;
}