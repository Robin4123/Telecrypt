#include "nodeSocket.h"

#define DEFAULT_BUFLEN 512

// Default constructor
// We intialize one listening socket to incoming connections (Via other nodes or clients)
nodeSocket::nodeSocket()
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

// TODO: Since we create sockets in 2 different scenarios (Listening socket and Sending socket), maybe create a function that create socket by IP and port to shorten code

// Creation of the listening socket
int nodeSocket::createSocket(const char* port)
{
    // Creation of the listening socket
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
    
    return 0;
}

int nodeSocket::bindSocket()
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

int nodeSocket::listenSocket()
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
void nodeSocket::acceptConnection()
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
        users.emplace_back(std::thread(&nodeSocket::handleClient, this, ClientSocket));
    }
    
}

// Connecting to main server or the next node in the messages route by the given port and IP
SOCKET nodeSocket::connectSocket(const char* port, const char* ip)
{
    int iResult = 0;
    SOCKET SendSocket = INVALID_SOCKET;

    // Create socket
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
    }

    SendSocket = INVALID_SOCKET;

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    SendSocket = socket(ptr->ai_family, ptr->ai_socktype, 
        ptr->ai_protocol);

    if (SendSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
    }
    
    // Connect to server.
    iResult = connect(SendSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(SendSocket);
        SendSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (SendSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return INVALID_SOCKET;
    }
    return SendSocket;
}


// Simple function to the send the given data to the given socket
int nodeSocket::sendData(string data, SOCKET user)
{
    int iSendResult = send(user, data.c_str(), (int) data.length(), 0);
    printf("Bytes sent: %d\n", iSendResult);
    return iSendResult;
}

// Function handles client (Can be node or user)
/*
 * Source sock is the sock we recieve data from
 * Dest sock is the sock we will send data do, it can be client, another node or the main server
 *
 * We first determine the next dest socket
 * We then create a thread for the recieving messages from the dest sock and forwarding to the source sock
 * And we also in the function get the messages from the source sock and forwarding it to the dest sock
 * Thus creating a two way forwarding system of messages
 */
void nodeSocket::handleClient(SOCKET source_sock)
{
    // Establishing connection to the next destination (Node/Client/Main server)
    SOCKET dest_sock = INVALID_SOCKET;
    dest_sock = connectSocket(DEFAULT_PORT, SERVER_IP);

    thread recv_thread = thread(&nodeSocket::getMessagesAndForward, this, dest_sock, source_sock);
    char recvbuf[DEFAULT_BUFLEN];
    int iResult = 1;
    int recvbuflen = DEFAULT_BUFLEN;

    // Receive until the user disconnects and the recv_thread is active
    while (iResult > 0 && recv_thread.joinable())
    {
        printf("Receiving messages from %s...\n", std::to_string(source_sock).c_str());
        iResult = recv(source_sock, recvbuf, recvbuflen, 0);
        // TODO: Since this code is copied to the getMessagesAndForward consider creating a function to handle the checking of messages
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d From user: %s\n", iResult, std::to_string(source_sock).c_str());
            printf("%s\n", recvbuf);

            printf("Forwarding messages to %s...\n", std::to_string(dest_sock).c_str());
            sendData(recvbuf, dest_sock);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(source_sock);
            iResult = 0;
        }
    }
    printf("Connection closed by user: %s...\n", std::to_string(source_sock).c_str());
    closesocket(source_sock);
    recv_thread.join();
}

// Pretty straight forward, we get messages from the recv_sock and forwarding to the send_sock until we get an invalid message from the recv_sock
// Function being used by handleClient as explained there
void nodeSocket::getMessagesAndForward(SOCKET recv_sock, SOCKET send_sock)
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult = 1;
    while (iResult > 0)
    {
        printf("Receiving messages from %s...\n", std::to_string(recv_sock).c_str());
        iResult = recv(recv_sock, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d From user: %s\n", iResult, std::to_string(recv_sock).c_str());
            printf("%s\n", recvbuf);

            printf("Forwarding messages to %s...\n", std::to_string(send_sock).c_str());
            sendData(recvbuf, send_sock);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(recv_sock);
            iResult = 0;}
    }
    printf("Connection closed by user: %s...\n", std::to_string(recv_sock).c_str());
    closesocket(recv_sock);
}
