#include <iostream>
#include "nodeSocket.h"

using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27014"
#define DEFAULT_BUFLEN 512

int main(int argc, char* argv[])
{
    nodeSocket server = nodeSocket();
    
    if (server.createSocket(DEFAULT_PORT) == 1)
    {
        printf("Error creating socket\n");
        return 0;
    }

    // Listening Socket handling
    server.bindSocket();
    if (server.listenSocket() == 1)
    {
        printf("Error listening on socket\n");
        return 0;
    }
    
    return 0;
}
