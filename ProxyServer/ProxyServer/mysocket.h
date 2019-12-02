#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

constexpr auto MAXSIZE = 65507; // �������ݱ��ĵ���󳤶�;
constexpr auto HTTP_PORT = 80; //  �������˿�;
constexpr auto SERVER_PORT = "8888"; // ����˿�

WSADATA startSocketProgram(char mainNum, char minNum);

//SOCKET createTCPSocket();
SOCKET initServerSocket();
//SOCKET initSocket2();

void closeTCPsocket(SOCKET sock);

SOCKET acceptSocket(SOCKET listenSocket);

void endSocketProgram();
