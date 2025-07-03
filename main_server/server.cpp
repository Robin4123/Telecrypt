#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include "build/serverSocket.h"

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int main (){
    
    serverSocket server = serverSocket();
    server.createSocket(DEFAULT_PORT);
    server.bindSocket();
    server.listenSocket();
    
    
    return 0;
}