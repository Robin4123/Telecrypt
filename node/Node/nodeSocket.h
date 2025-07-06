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

// Main server
#define DEFAULT_PORT "27015"
#define SERVER_IP "10.100.102.29"

using namespace std;

#pragma comment(lib, "WS2_32.lib")

class nodeSocket
{
public:
    nodeSocket();

    int createSocket(const char* port);

	// Listening socket
    int listenSocket();
    int bindSocket();

	// Sending socket
	SOCKET connectSocket(const char* port, const char* ip);
	int sendData(string data, SOCKET user);

	SOCKET getListeningSocket();
	SOCKET getSendingSocket();
private:
    void acceptConnection();
	void handleClient(SOCKET user);
	void getMessagesAndForward(SOCKET recv_sock, SOCKET send_sock);

    
    std::vector<std::thread> users;
    SOCKET ListenSocket; // Will be connected to client/ the node we recieve information from
    struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
};
