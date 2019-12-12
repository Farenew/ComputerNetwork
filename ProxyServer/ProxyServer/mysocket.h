#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

constexpr auto HTTP_PORT = 80;  
constexpr auto SERVER_PORT = "8888"; 

// start socket program, doing WSAStartup() and check winsock version
WSADATA startSocketProgram(char mainNum, char minNum);

// initalize server's socket, set TCP protocol, create socket, bind socket and listen on it
SOCKET initServerSocket();

// close TCP socket
void closeTCPsocket(SOCKET sock);

// accept client socket from listenSocket
SOCKET acceptSocket(SOCKET listenSocket);

// get peer info from socket
sockaddr_in getpeerInfo(SOCKET clientSock);

// end our socket program
void endSocketProgram();

// from sockaddr_in struct get ip 
std::string getIPfromSockaddr(sockaddr_in& sa);

// from sockaddr_in struct get port
unsigned getPortfromSockaddr(sockaddr_in& sa);