#ifndef _SHARED_LIB_H_
#define _SHARED_LIB_H_

// Declare server input queue id
#define OUT_KEY 123
// Declare server output queue id
#define REQUEST_KEY 321
#define RELEASE_KEY 213

// Structure used in the messaging process by clients and server
typedef struct message
{
    int type;
    char text[20];
} message;

#endif