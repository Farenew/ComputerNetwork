#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>

constexpr auto HTTP_PORT = "80";  
constexpr auto SERVER_PORT = "8888"; 

using std::string;

// start socket program, doing WSAStartup() and check winsock version
WSADATA startSocketProgram(char mainNum, char minNum);

// initalize server's socket, set TCP protocol, create socket, bind socket and listen on it
SOCKET initServerSocket();

// close TCP socket
void closeTCPsocket(SOCKET sock);

// accept client socket from listenSocket
SOCKET acceptSocket(SOCKET listenSocket);

// end our socket program
void endSocketProgram();

// create local socket and connect to remote host, return local host
SOCKET connetToRemote(string host);
