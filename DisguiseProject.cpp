//Author: Arianna Marrocu
//Disguise Code Test
//Monitor startup and heartbeat messages from Director and Actor machines in d3 session

#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <Windows.h>
#define SESSION_PORT 7106
#define HEARTBEAT_PORT 7104

#pragma comment(lib, "Ws2_32.lib")


int main(int argc, char const* argv[])
{   
    // Initialize Winsock
    Sleep(5);
    printf("Starting...");
    WSADATA wsaData;
    int iResult;

    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    
    //Declare session address information
    struct sockaddr_in session_addr;
    session_addr.sin_family = AF_INET;
    session_addr.sin_port = htons(SESSION_PORT);
    session_addr.sin_addr.s_addr = INADDR_ANY;
    int addrlen_ses = sizeof(session_addr);
    
    //Declare heartbeat address information
    struct sockaddr_in heartbeat_addr;
    heartbeat_addr.sin_family = AF_INET;
    heartbeat_addr.sin_port = htons(HEARTBEAT_PORT);
    heartbeat_addr.sin_addr.s_addr = INADDR_ANY;
    int addrlen_heart = sizeof(heartbeat_addr);

    //Read variables
    int valread_ses;
    int valread_heart;
    char buffer_ses[1024] = { 0 };
    char buffer_heart[1024] = { 0 };

    //Create socket that will receive session information
    SOCKET session_sockfd;
    session_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    //Create socket that will receive session information
    SOCKET heartbeat_sockfd;
    heartbeat_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    //Binding sockets to different ports
    bind(session_sockfd, (SOCKADDR*)&session_addr, addrlen_ses);
    bind(heartbeat_sockfd, (SOCKADDR*)&heartbeat_addr, addrlen_heart);


    wprintf(L"Receiving datagrams...\n");
    while (true) {
        valread_ses = recvfrom(session_sockfd,
            buffer_ses, sizeof(buffer_ses), 0, (SOCKADDR*)&session_addr, &addrlen_ses);
        valread_heart = recvfrom(heartbeat_sockfd,
            buffer_heart, sizeof(buffer_ses), 0, (SOCKADDR*)&heartbeat_addr, &addrlen_heart);
        buffer_ses[valread_ses] = '\0';
        buffer_heart[valread_heart] = '\0';
        printf("Startup : %s\n", buffer_ses);
        printf("Heartbeat : %s\n", buffer_heart);
    }
    
    return 0;

    
}

