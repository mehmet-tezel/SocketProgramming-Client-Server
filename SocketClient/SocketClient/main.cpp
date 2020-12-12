/*
*    This software is owned by Mehmet Tezel.
*    You can change and use as you wish.
*    (Provided that the source is specified)
*    https://github.com/TzLcorp
*    TezeL Corporation
*/

#include <Windows.h>
#include <iostream>
#include <cstdio>

#pragma comment(lib,"ws2_32.lib") // Winsock Library

using namespace std;

#define MANAGE_OPERATION_0            0 // If you want to do operation define new variables and use client.

void textcolor(int color); // Changes console text color.

typedef struct _CLIENTDATA
{
    byte Operation; // Manage Operation from client.
    int id = 0; // Your ID.
    int msgCounter = 0; // Counts the messages on the server for user client.
    char msg[128] = { '\0' }; // The reason it is null is for sending empty data initially.
    char userName[16] = { '\0' }; // Your username.
    char reply[100] = { '\0' }; // If the user interacts with the server (this is up to you) the server responds to the user.
    bool isJoined = false; // For someone who joins the server.

}CLIENTDATA, *PCLIENTDATA; // This structure manages all data.

/* The codes I wrote in the while loop 
*  are for a healthy communication with the server. 
*  If you want, you can delete those codes and create 
*  your own client and server from 0.
*/

int main() {

    CLIENTDATA data;
    PCLIENTDATA p_data;
    SOCKET Socket;
    hostent* host;
    int tryConnect = 1;
    int userID;

    WSADATA wd;
    sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
    {
        printf("\nError: Unable to initialize Winsock.\n");
        return 1;
    }

    char ipAddress[16];
    int port;
    cout << "Enter IP: ";
    cin >> ipAddress;
    cout << "Enter Port: ";
    cin >> port;

    host = gethostbyname(ipAddress);

    if (!host)
    {
        printf("\nError: Unable to resolve host.\n");

        WSACleanup();
        return -1;
    }

    printf("Host: %s\n", ipAddress);

    server.sin_family = AF_INET;
    server.sin_addr = *((LPIN_ADDR)*host->h_addr_list);
    server.sin_port = htons(port);

    p_data = (PCLIENTDATA)VirtualAlloc(NULL, port, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Converts port packages to a "pointer" structure variable.

    int userNameHandler = 0;

    while (true)
    {
        label:
        if (userNameHandler <= 0) {
            cout << "Enter username: ";
            cin >> data.userName;
            userNameHandler += 2;
            if (strlen(data.userName) >= 16)
            {
                printf("\nYou can enter max 16 characters!\n");
                userNameHandler = 0;
                goto label;
            }
        }

        Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket.

        if (Socket == INVALID_SOCKET)
        {
            printf("\nError: Unable to create socket.\n");
        }

        if (connect(Socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR && tryConnect <= 3) // Connect to the server.
        {
            printf("\nCould not connect to the server.\n");
            Sleep(2000);
            printf("\nRetrying...(%d)\n", tryConnect);
            Sleep(2000);
            tryConnect += 1;
            data.isJoined = false;
            //closesocket(Socket);
            continue;
        }

        if (tryConnect >= 4 && data.isJoined == false)
        {
            printf("\nCould not connect to the server, please contact your server administrator.\n");
            closesocket(Socket);
            system("PAUSE");
            return -1;
        }

        if (send(Socket, (char*)&data, sizeof(data), 0) == SOCKET_ERROR) // Send data to server.
        {
            printf("\nError: Unable to send data to remote computer.\n");
            closesocket(Socket);
            continue;
        }

        recv(Socket, (char*)p_data, sizeof(CLIENTDATA), 0);  // Receive data from server.
        
        if (data.isJoined == false) {

            userID = p_data->id;
            data.isJoined = true;

            cout << "\n\n["; textcolor(10); cout << "+"; textcolor(7); cout << "] "; textcolor(10); cout << "You are connected!\n" << endl; textcolor(7);
            printf("ID: %d\n", userID);

            tryConnect = 1;
        }

        memset(p_data, 0, sizeof(CLIENTDATA));

        memset(&data.msg, 0, sizeof(data.msg));

        if (data.isJoined == true) {
            cout << "\n>> ";
            scanf_s("\r%[^\n]c", data.msg);
            if (strlen(data.msg) >= 128)
            {
                data.msg[0] = '\0';
                printf("\nYou can enter only 128 characters! (Kicked for server health.)\n\n");
                data.isJoined = false;
                userNameHandler = 0;
                closesocket(Socket);
                continue;
            }
        }
    }

    closesocket(Socket);
    WSACleanup();

	return 0;
}

void textcolor(int color)
{
    static int BACKGROUND;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    GetConsoleScreenBufferInfo(h, &csbiInfo);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
        color + (BACKGROUND << 4));
}