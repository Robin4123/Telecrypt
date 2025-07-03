#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "WS2_32.lib")

#define DEFAULT_BUFLEN 512

class clientSocket{
    public:
        clientSocket();

        void createSocket(const char* port, const char* ip);

        int connectSocket();
        SOCKET getSocket();
        int sendData(std::string data);

    private:
    struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

    SOCKET sock;

};