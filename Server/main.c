#include <stdio.h>
#include <winsock.h>

DWORD WINAPI serverRecieve(LPVOID); // Recieve data from socket
DWORD WINAPI serverSend(LPVOID);    // Send data to socket

void wWSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);  // Init winsock2
SOCKET wSocket(int af, int type, int protocol);                 // Socket creation
void wBind(SOCKET s, const struct sockaddr *name, int namelen); // Bind socket to address:port
void wListen(SOCKET s, int backlog);                            // Listening for clients
SOCKET wAccept(SOCKET s, struct sockaddr *addr, int *addrlen);  // Accept remaining client
void wCloseSocket(SOCKET s);                                    // Socket closing

const char SERVER_IP[] = "127.0.0.1";
const int PORT_NUM = 5555;
const int BUFFER_SIZE = 512;

int main() {
    WSADATA WSAData;
    wWSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKET server = wSocket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN serverAddr;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT_NUM);
    wBind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    wListen(server, 0);
    printf("Listening for incoming connections..." << \n");

    SOCKADDR_IN clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET client = wAccept(server, (SOCKADDR *)&clientAddr, &clientAddrLen);

    printf("\n");
    printf("Client connected!" << \n");
    printf("Now you can user our live char application." << \n");
    printf("Enter \"exit\" to disconnect." << \n");
    printf("\n");

    HANDLE threadRecieve, threadSend;
    threadRecieve = CreateThread(NULL, 0, serverRecieve, &client, 0, 0);
    threadSend = CreateThread(NULL, 0, serverSend, &client, 0, 0);
    if (threadRecieve == NULL || threadSend == NULL) {
        printf("Threads creation error: " << WSAGetLastError() << \n");
    }
    WaitForSingleObject(threadRecieve, INFINITE);
    WaitForSingleObject(threadSend, INFINITE);

    wCloseSocket(client);
    wCloseSocket(server);

    WSACleanup();

    return 0;
}

DWORD WINAPI serverRecieve(LPVOID lpParam) {
    char buffer[BUFFER_SIZE] = {};
    SOCKET client = *(SOCKET *)lpParam;

    while (true) {
        if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("recv() - ERROR: " << WSAGetLastError() << \n");
            exit(-1);
        }

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Client disconnected." << \n");
            break;
        }

        printf("Client: " << buffer << \n");
        memset(buffer, 0, sizeof(buffer));
    }

    return 1;
}

DWORD WINAPI serverSend(LPVOID lpParam) {
    char buffer[BUFFER_SIZE] = {};
    SOCKET client = *(SOCKET *)lpParam;

    while (true) {
        fgets(buffer, sizeof(buffer), stdin);

        if (send(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("send() - ERROR: " << WSAGetLastError() << \n");
            exit(-1);
        }

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Thank you for using the application." << \n");
            break;
        }
    }

    return 1;
}

void wWSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData) {
    if ((WSAStartup(wVersionRequested, lpWSAData)) != 0) {
        printf("WSAStartup() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
}

SOCKET wSocket(int af, int type, int protocol) {
    int server;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("socket() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
    return server;
}

void wBind(SOCKET s, const struct sockaddr *name, int namelen) {
    if (bind(s, name, namelen) == SOCKET_ERROR) {
        printf("bind() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
}

void wListen(SOCKET s, int backlog) {
    if (listen(s, backlog) == SOCKET_ERROR) {
        printf("listen() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
}

SOCKET wAccept(SOCKET s, struct sockaddr *addr, int *addrlen) {
    SOCKET client;
    if ((client = accept(s, addr, addrlen)) == INVALID_SOCKET) {
        printf("accept() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
    return client;
}

void wCloseSocket(SOCKET s) {
    if (closesocket(s) == SOCKET_ERROR) {
        printf("closesocket() - ERROR: " << WSAGetLastError() << \n");
        exit(-1);
    }
}