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
    return 0;
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

// We accept incoming connection and create a thread to handle that connection
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
        userSockets.push_back(ClientSocket);
        threadedUsers.emplace_back(std::thread(&serverSocket::handleClient, this, ClientSocket));
    }
}

// We let the user choose a user from the connected users to talk with
SOCKET serverSocket::chooseUser(SOCKET user)
{
    SOCKET otherUser = INVALID_SOCKET;
    int iResult = 1;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];
    int numUser = userSockets.size() - 1;

    while (numUser == 0)
    {
        // Wait until more users are online
        numUser = userSockets.size() - 1;
    }
    sendData("Choose a number between 0 to " + std::to_string(numUser), user);
    iResult = recv(user, recvbuf, recvbuflen, 0);

    recvbuf[iResult] = '\0';
    printf("Bytes received: %d From user: %s\n", iResult, std::to_string(user).c_str());
    printf("%s\n", recvbuf);
    int num = int(recvbuf[0]) - '0'; // Minus the ASCII value of 0
    otherUser = userSockets.at(num);
    return otherUser;
}
void serverSocket::handleClient(SOCKET user)
{
    char recvbuf[DEFAULT_BUFLEN];
    int iResult = 1;
    int recvbuflen = DEFAULT_BUFLEN;

    SOCKET otherUser = chooseUser(user);
    // Receive until the user disconnects or the otherUser has closed it's connection
    while (iResult > 0 && otherUser != INVALID_SOCKET)
    {
        iResult = recv(user, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d From user: %s\n", iResult, std::to_string(user).c_str());
            printf("%s\n", recvbuf);


            // Get data from other user and send it to user
            sendData(recvbuf, otherUser);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(user);
            iResult = 0;
        }
    }
    printf("Connection closed by user: %s...\n", std::to_string(user).c_str());
    chooseUser(otherUser); // Let the other user choose a new user to talk to
    userSockets.erase(std::remove(userSockets.begin(), userSockets.end(), user), userSockets.end()); // Removing the disconnected user from the list of connected users
    closesocket(user);
}

int serverSocket::sendData(string data, SOCKET user)
{
    int iSendResult = send(user, data.c_str(), (int) data.length(), 0);
    printf("Bytes sent: %d\n", iSendResult);
    return iSendResult;
}
