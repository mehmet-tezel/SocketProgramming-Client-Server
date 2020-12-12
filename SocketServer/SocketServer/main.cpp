/*
    This software is owned by Mehmet Tezel.
    You can change and use as you wish.
    (Provided that the source is specified)
    https://github.com/TzLcorp
    TezeL Corporation
*/

#include <iostream>
#include <cstdio>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma warning(disable : 4996) // Disables _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib") // Winsock Library

#define _WINSOCK_DEPRECATED_NO_WARNINGS // I hate warnings
#define MANAGE_OPERATION_0            0 // If you want to do operation define new variables and use client.


typedef struct _SERVERDATA
{
    byte Operation; // Manage Operation
    int id = 0; // IDs of users joining the server.
    int msgCounter = 0; // Counts the messages on the server for user client.
    char msg[128] = { '\0' }; // The reason it is null is for sending empty data initially.
    char userName[16] = { '\0' }; // Usernames of users joining the server.
    char reply[100]; // If the user interacts with the server (this is up to you) the server responds to the user.
    bool isJoined = false; // For someone who joins the server.

}SERVERDATA, * PSERVERDATA; // This structure manages all data.

struct sockaddr_in server;

int main(/*int argc, char* argv[] for use from command line*/) {

    SOCKET s, new_socket;
    SERVERDATA data;
    PSERVERDATA p_data;
    WSADATA wsa;

    printf("\nWSAStartup(Winsock) is initializing...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("\nInitialized.\n");

    printf("Create server.\n");
    char ipAddress[16];
    int port;
    std::cout << "\nEnter IP: ";
    std::cin >> ipAddress;
    std::cout << "Enter Port: ";
    std::cin >> port;

    server.sin_family = AF_INET;             
    server.sin_port = htons(port);
    //server.sin_addr.s_addr = htonl(INADDR_ANY); 
    inet_pton(AF_INET, ipAddress, &(server.sin_addr.s_addr)); // 0.0.0.0 computer's all local address

    p_data = (PSERVERDATA)VirtualAlloc(NULL, port, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Converts port packages to a "pointer" structure variable.

    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) // Create Socket with TCP
    {
        printf("Could not create socket : %d", WSAGetLastError());
        return -1;
    }

    printf("\nSocket created.\n");

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) // Binds created socket
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return -1;
    }

    printf("Bind done.\n");

    listen(s, 5); // Listen as much connection as you value. Max 128.

    printf("Waiting for connections...\n");

    while (true)
    {
        new_socket = accept(s, (struct sockaddr*)&server, NULL); // Create accept socket.
        if (new_socket == INVALID_SOCKET)
        {
            printf("accept failed with error code : %d", WSAGetLastError());
            closesocket(new_socket);
            return -1;
        }

        recv(new_socket, (char*)p_data, sizeof(SERVERDATA), 0); // Receive data from all clients.

        // Some codes.

        if (!p_data->isJoined && strlen(p_data->userName) > 1)
        {
            printf("\n%s is joined\n", p_data->userName);
            p_data->isJoined = true;
            data.id += 1;
            strcpy(data.userName, p_data->userName);
        }

        if (strlen(p_data->msg) >= 1 && strlen(p_data->msg) <= 127) {
            printf("\n%s: %s\n", p_data->userName, p_data->msg);
            data.msgCounter += 1;
            strcpy(data.userName, p_data->userName);
            strcpy(data.msg, p_data->msg);
        }

        if (!_stricmp("sa", p_data->msg))
        {
            strcpy(data.reply, "\nSERVER: as hg\n");
            printf("%s", data.reply);
        }
        else
        {
            strcpy(data.reply, " ");
        }

        send(new_socket, (char*)&data, sizeof(data), 0); // Send data to all clients.

        memset(p_data, 0, sizeof(SERVERDATA)); 
    }

    closesocket(new_socket); // This code can never reach here.
    WSACleanup();

    return 0;
}