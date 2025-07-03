#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>

#include "clientSocket.h"

#define DEFAULT_PORT "27015"
#define SERVER_IP "10.100.102.29"

using namespace std;


void receiveData(SOCKET server)
{
    int recvbuflen = DEFAULT_BUFLEN;

    char recvbuf[DEFAULT_BUFLEN];

    int iResult = 1;
    
    while (iResult > 0) {
        iResult = recv(server, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0'; // Getting rid of empty space
            printf("Bytes received: %d\n", iResult); // Len of the message
            printf("%s\n", recvbuf); // Message itself
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    }
}

void handleConnection(clientSocket Socket)
{
    thread serverThread = thread(&receiveData, Socket.getSocket());
    string data;
    // Run while the thread is alive, aka there is still a connection to the server
    while (serverThread.joinable())
    {
        // Send data to server here.m
        std::cout << "Enter text (press Enter to submit): ";
        std::getline(std::cin, data); // Reads input until Enter is pressed
        Socket.sendData(data);
    }
}

int main (){
    clientSocket Socket = clientSocket();

    Socket.createSocket(DEFAULT_PORT, SERVER_IP);

    if(Socket.connectSocket() == 1)
    {
        printf("Unable to connect, try again");
        return 0;
    }
    handleConnection(Socket);
    return 0;
}


