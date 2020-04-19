#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>


const unsigned short SERVER_PORT = 8888;

using std::string;

// start socket program, doing WSAStartup() and check winsock version
WSADATA startSocketProgram(char mainNum, char minNum);

// initalize server's socket, create UDP socket, bind socket and listen on it
SOCKET initServerSocket();

// bind server socket, return server socket's addr
// 在代理服务器实验中，bind放在了initServerSocket()函数里，但这里要用到sockaddr_in，所以把这里分出来
struct sockaddr_in bindSocket(SOCKET& serverSock);

// close socket
void closeSocket(SOCKET sock);

// end our socket program
void endSocketProgram();


