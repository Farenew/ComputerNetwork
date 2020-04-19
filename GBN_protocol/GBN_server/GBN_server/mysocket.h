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
// �ڴ��������ʵ���У�bind������initServerSocket()�����������Ҫ�õ�sockaddr_in�����԰�����ֳ���
struct sockaddr_in bindSocket(SOCKET& serverSock);

// close socket
void closeSocket(SOCKET sock);

// end our socket program
void endSocketProgram();


