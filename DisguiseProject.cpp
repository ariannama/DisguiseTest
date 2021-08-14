//Author: Arianna Marrocu
//Disguise Code Test
//Monitor startup and heartbeat messages from Director and Actor machines in d3 session

#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <vector>
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

    //Variables to store datagrams
    int valread_ses, valread_heart, activity, max_sd;
    char buffer_ses[1024] = { 0 };
    char buffer_heart[1024] = { 0 };
    string delim = "|";
    vector<string> words{};

    //Create socket that will receive session information
    SOCKET session_sockfd;
    session_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    //Create socket that will receive heartbeat information
    SOCKET heartbeat_sockfd;
    heartbeat_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //Handle error in creation of socket
    if (session_sockfd == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    if (heartbeat_sockfd == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    //Create set of file descriptors containing both sockets
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(session_sockfd, &readfds);
    FD_SET(heartbeat_sockfd, &readfds);
    max_sd = max(session_sockfd, heartbeat_sockfd);

    
    //Binding sockets to different ports
    bind(session_sockfd, (SOCKADDR*)&session_addr, addrlen_ses);
    bind(heartbeat_sockfd, (SOCKADDR*)&heartbeat_addr, addrlen_heart);

    //Continuously receive UDP datagrams
    cout << "Receiving UDP datagrams...";
    while (true) {
        //Empty out fd_set every time to receive messages coming to both ports
        FD_ZERO(&readfds);
        
        FD_SET(session_sockfd, &readfds);
        FD_SET(heartbeat_sockfd, &readfds);

        //Identify if theres is are datagrams being sent to the sockets in fd_set
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        words.clear();

        //If the activity is in the heartbeat socket, read datagram
        if (FD_ISSET(heartbeat_sockfd, &readfds)) {
            valread_heart = recv(heartbeat_sockfd,
                buffer_heart, sizeof(buffer_heart), 0);
            //Parsing datagram
            if (valread_heart > 0) {
                string s1 = buffer_heart;
                size_t n1 = s1.find("MACHINESTATUS");
                size_t pos = 0;
                if (n1 != string::npos) {                    
                    cout << "Heartbeat message: \n";                
                    while ((pos = s1.find(delim)) != string::npos) {
                        words.push_back(s1.substr(0, pos));
                        s1.erase(0, pos + delim.length());
                    }
                    cout << "\tMachine version: " << words[1] << "\n";
                    cout << "\tFPS: " << words[2] << "\n";
                    words.clear();                
                }
            }
            else {
                cout << "recv failed with error: \n" << WSAGetLastError();
            }
        }
        //If the activity is in the session socket, read datagram
        else if (FD_ISSET(session_sockfd, &readfds)) {
            valread_ses = recv(session_sockfd,
                buffer_ses, sizeof(buffer_ses), 0);
            //Parsing datagram
            if (valread_ses > 0) {                
                string s2 = buffer_ses;
                size_t n1 = s2.find("SESSION2");
                size_t n2 = s2.find("MACHINE");
                size_t pos = 0;
                if (n1 != string::npos) {
                    cout << "Director starting up: \n";
                    while ((pos = s2.find(delim)) != string::npos) {
                        words.push_back(s2.substr(0, pos));
                        s2.erase(0, pos + delim.length());
                    }
                    cout << "\tSession name: " << words[1] << "\n";
                    cout << "\tCreator: " << words[2] << "\n";
                    words.clear();
                }
                if (n2 != string::npos) {
                    cout << "Actor starting up";
                    while ((pos = s2.find(delim)) != string::npos) {
                        words.push_back(s2.substr(0, pos));
                        s2.erase(0, pos + delim.length());
                    }
                    cout << "\tMachine ID: " << words[1] << "\n";
                    cout << "\tSession name: " << words[2] << "\n";
                    
                }
            }
            else {
                cout << "recv failed with error: \n" << WSAGetLastError();
            }
        }
        
        
    }
    
    return 0;

    
}

