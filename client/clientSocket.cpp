#include "clientSocket.h"


clientSocket::clientSocket() {
    WSADATA wsaData;
    sock = INVALID_SOCKET;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
    }
}

void clientSocket::createSocket(const char* port, const char* ip) {
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
    }

    sock = INVALID_SOCKET;

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    sock = socket(ptr->ai_family, ptr->ai_socktype, 
        ptr->ai_protocol);

    if (sock == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
    }
}

int clientSocket::connectSocket() {
    // Connect to server.
    int iResult = 0;
    iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }
    return true;
}

SOCKET clientSocket::getSocket() { return sock;}

int clientSocket::sendData(std::string data) {
    
    const char* dataAsChar = data.c_str();
    int iResult = 0;
    
    // Send data
    iResult = send(sock, dataAsChar, (int) strlen(dataAsChar), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);
    
    return 0;
}