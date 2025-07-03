#include "serverSocket.h"

#define DEFAULT_BUFLEN 512

serverSocket::serverSocket()
{
    WSADATA wsaData;
    ListenSocket = INVALID_SOCKET;
    
    int iResult;
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        printf("WSAStartup failed: %d/n", iResult);
    }
    else {
        printf("WSAStartup succeeded\n");
    }    
}

int serverSocket::createSocket(const char* port)
{
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
}

int serverSocket::bindSocket()
{
    // Setup the TCP listening socket
    int iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
}

int serverSocket::listenSocket()
{
    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    
    acceptConnection();
}

void serverSocket::acceptConnection()
{
    SOCKET ClientSocket;
    while (true)
    {
        ClientSocket = INVALID_SOCKET;
    
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            continue;
        }

        printf("New client connected, creating thread..\n");
        users.emplace_back(std::thread(&serverSocket::handleClient, this, ClientSocket));
    }
    
}

void serverSocket::handleClient(SOCKET user)
{
    char recvbuf[DEFAULT_BUFLEN];
    int iResult = 1;
    int recvbuflen = DEFAULT_BUFLEN;

    // Receive until the user disconnects
    while (iResult > 0)
    {
        iResult = recv(user, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d From user: %s\n", iResult, std::to_string(user).c_str());
            printf("%s\n", recvbuf);

            sendData("Custom data", user);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(user);
            iResult = 0;
        }
    }
    printf("Connection closed by user: %s...\n", std::to_string(user).c_str());
    closesocket(user);
}

int serverSocket::sendData(string data, SOCKET user)
{
    int iSendResult = send(user, data.c_str(), (int) data.length(), 0);
    printf("Bytes sent: %d\n", iSendResult);
    return iSendResult;
}
