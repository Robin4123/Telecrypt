#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include "build/serverSocket.h"

#define DEFAULT_PORT "27015"

int main (){
    
    serverSocket server = serverSocket();

    if (server.createSocket(DEFAULT_PORT) == 1)
    {
        printf("Failed to create socket\n");
        return 1;
    }
    if (server.bindSocket() == 1)
    {
        printf("Failed to bind socket\n");
        return 1;
    }
    if (server.listenSocket() == 1)
    {
        printf("Failed to listen socket\n");
        return 1;
    }
    
    return 0;
}