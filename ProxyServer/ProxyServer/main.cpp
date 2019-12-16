
#include <stdio.h>
#include <iostream>
#include "mysocket.h"
#include "HttpParser.h"
#include "SocketTools.h"

#pragma comment(lib, "Ws2_32.lib")

// maxsize for our recv buffer
constexpr auto MAX_BUFFER = 65536;

// receive buffer
static char recvBuf[MAX_BUFFER];

// used to store buffer from remote server
static char remoteBuf[MAX_BUFFER];

HttpParser* hp;

int main() {

	// start winsock version 2.2
	startSocketProgram(2, 2);
	// set up TCP protocol, create socket, bind and listen to it
	SOCKET serverSock = initServerSocket();

	// receive buffer length
	int recvLen = 1;
	// remote buffer receive length
	int remoteRecvLen = 1;

	// client socket we are going to accept
	SOCKET clientSock = INVALID_SOCKET;

	SOCKET connectSocket = INVALID_SOCKET;

	
	do {
		
		clientSock = acceptSocket(serverSock);

		recvLen = recv(clientSock, recvBuf, MAX_BUFFER, 0);

		if (recvLen > 0) {

			hp = new HttpParser();
			hp->parse(recvBuf);

			hp->printRequestLine();

			// make sure our host exists
			if (hp->headerLines.find("Host") != hp->headerLines.end()) {
				connectSocket = connetToRemote(hp->headerLines["Host"]);
			}
			else {
				printf("cannot find host in HTTP request!!!\n");
				WSACleanup();
				return 1;
			}

			// send buffer message to remote server
			int i = send(connectSocket, recvBuf, (int)strlen(recvBuf), 0);
			if (i == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}

			// Receive data until remote server closes the connection
			do {
				remoteRecvLen = recv(connectSocket, remoteBuf, MAX_BUFFER, 0);
				if (remoteRecvLen > 0) {
					printf("Bytes received: %d\n", remoteRecvLen);

					send(clientSock, remoteBuf, sizeof(remoteBuf), 0);
				}
				else if (remoteRecvLen == 0)
					printf("Connection closed\n");
				else
					printf("recv failed: %d\n", WSAGetLastError());
			} while (remoteRecvLen > 0);
		

			 //auto peerInfo = getpeerInfo(clientSock);
			 //printf("IP %s\nPort %d\n", getIPfromSockaddr(peerInfo).c_str(), getPortfromSockaddr(peerInfo));

			delete hp;
		}
		else if (recvLen == 0)
			printf("Connection closing...\n");
		else
			printf("recv failed %d\n", WSAGetLastError());

		Sleep(200);

	} while (recvLen > 0);
		
	// close server socket 
	closeTCPsocket(serverSock);

	// close winsock program, release resources
	endSocketProgram();
}