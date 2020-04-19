#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>

using std::string;

// start socket program, doing WSAStartup() and check winsock version
WSADATA startSocketProgram(char mainNum, char minNum);

// initalize client's socket, create UDP socket, bind socket and listen on it
SOCKET init_client_socket();

// close socket
void closeSocket(SOCKET sock);

// end our socket program
void endSocketProgram();


