#include <stdio.h>
#include <winsock.h>

DWORD WINAPI clientReceive(LPVOID); // Recieve data from socket
DWORD WINAPI clientSend(LPVOID);    // Send data to socket

void wWSAStartup(WORD, LPWSADATA);             // Init winsock2
SOCKET wSocket(int, int, int);                 // Socket creation
void wConnect(SOCKET, struct sockaddr *, int); // Connecnt to server
void wCloseSocket(SOCKET);                     // Socket closing

const char SERVER_IP[] = "127.0.0.1";
const int PORT_NUM = 12121;
const int BUFFER_SIZE = 512;

int main() {
    WSADATA WSAData;
    wWSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKET server = wSocket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN serverAddr;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT_NUM);
    wConnect(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    printf("\n");
    printf("Connected to server!\n");
    printf("Now you can user our live char application.\n");
    printf("Enter \"exit\" to disconnect.\n");
    printf("\n");

    HANDLE threadRecieve, threadSend;
    threadRecieve = CreateThread(NULL, 0, clientReceive, &server, 0, 0);
    threadSend = CreateThread(NULL, 0, clientSend, &server, 0, 0);
    if (threadRecieve == NULL || threadSend == NULL) {
        printf("Threads creation error: WSAGetLastError() << \n");
    }
    WaitForSingleObject(threadRecieve, INFINITE);
    WaitForSingleObject(threadSend, INFINITE);

    wCloseSocket(server);

    WSACleanup();

    return 0;
}

DWORD WINAPI clientReceive(LPVOID lpParam) {
    char buffer[BUFFER_SIZE] = {};
    SOCKET client = *(SOCKET *)lpParam;

    while (true) {
        if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("recv() - ERROR: WSAGetLastError() << \n");
            exit(-1);
        }

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: buffer << \n");
        memset(buffer, 0, sizeof(buffer));
    }

    return 1;
}

DWORD WINAPI clientSend(LPVOID lpParam) {
    char buffer[BUFFER_SIZE] = {};
    SOCKET client = *(SOCKET *)lpParam;

    while (true) {
        fgets(buffer, sizeof(buffer), stdin);

        if (send(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("send() - ERROR: WSAGetLastError() << \n");
            exit(-1);
        }

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Thank you for using the application.\n");
            break;
        }
    }

    return 1;
}

void wWSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData) {
    if (WSAStartup(wVersionRequested, lpWSAData) != 0) {
        printf("WSAStartup() - ERROR: WSAGetLastError() << \n");
        exit(-1);
    }
}

SOCKET wSocket(int af, int type, int protocol) {
    int server;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("socket() - ERROR: WSAGetLastError() << \n");
        exit(-1);
    }
    return server;
}

void wConnect(SOCKET s, struct sockaddr *addr, int namelen) {
    if (connect(s, addr, namelen) == SOCKET_ERROR) {
        printf("connect() - ERROR: WSAGetLastError() << \n");
        exit(-1);
    }
}

void wCloseSocket(SOCKET s) {
    if (closesocket(s) == SOCKET_ERROR) {
        printf("closesocket() - ERROR: WSAGetLastError() << \n");
        exit(-1);
    }
}