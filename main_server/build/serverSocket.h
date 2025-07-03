#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <thread>
#include <string>
#include <vector>
using namespace std;

#pragma comment(lib, "WS2_32.lib")

class serverSocket
{
public:
    serverSocket();

    int createSocket(const char* port);
    int listenSocket();
    int sendData(string data, SOCKET user);
    int bindSocket();
private:
    void acceptConnection();
    void handleClient(SOCKET user);
    
    std::vector<std::thread> users;
    SOCKET ListenSocket;
    struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
};
