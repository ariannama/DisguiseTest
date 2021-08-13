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

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char const* argv[])
{   
    // Initialize Winsock
    cout << "Starting...\n";
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
    int valread_ses, valread_heart, activity, max_sd = 2;
    char buffer_ses[1024] = { 0 };
    char buffer_heart[1024] = { 0 };

    //Create socket that will receive session information
    SOCKET session_sockfd;
    session_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    //Create socket that will receive session information
    SOCKET heartbeat_sockfd;
    heartbeat_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(session_sockfd, &readfds);
    FD_SET(heartbeat_sockfd, &readfds);

    
    //Binding sockets to different ports
    bind(session_sockfd, (SOCKADDR*)&session_addr, addrlen_ses);
    bind(heartbeat_sockfd, (SOCKADDR*)&heartbeat_addr, addrlen_heart);


    cout << "Receiving UDP datagrams...";
    while (true) {
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }
        if (FD_ISSET(heartbeat_sockfd, &readfds)) {
        valread_heart = recv(heartbeat_sockfd,
            buffer_heart, sizeof(buffer_heart), 0);
        cout << "Heartbeat :" << buffer_heart << "\n";
        }
        else if (FD_ISSET(session_sockfd, &readfds)) {
            valread_ses = recv(session_sockfd,
                buffer_ses, sizeof(buffer_ses), 0);
            cout << "Startup: " << buffer_ses << "\n";
        }
        
        
    }
    
    return 0;

    
}

