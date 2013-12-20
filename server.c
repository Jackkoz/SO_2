#include <stdio.h>
#include <pthread.h>

int main(int arguments_number, char* arguments[])
{
    // Initializing input data
    int resourcesNumber = atoi(arguments[1]);
    int resourcesAmount = atoi(arguments[2]);
    int resources[resourcesNumber], loopCounter;

    // Initializing the array of resources
    for (loopCounter = 0; loopCounter < resourcesNumber; loopCounter++)
    {
        resources[loopCounter] = resourcesAmount;
    }

    while (1)
    {
        // Main body
    }

    return 0;
}