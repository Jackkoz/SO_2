#ifndef _SHARED_LIB_H_
#define _SHARED_LIB_H_

// Declare server input queue id
#define OUT_KEY 123
// Declare server output queue id
#define REQUEST_KEY 321
#define RELEASE_KEY 213

// Ids of queues used to communicate with server
extern int SERVER_OUT, SERVER_REQUEST, SERVER_RELEASE;

// Structure used in the messaging process by clients and server
typedef struct message
{
    int type;
    // int text[10];
    char* text;
} message;

#endif