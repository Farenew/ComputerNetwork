
#include <stdio.h>
#include <iostream>
#include <process.h>

#include "mysocket.h"
#include "HttpParser.h"
#include "SocketTools.h"

#pragma comment(lib, "Ws2_32.lib")

// maxsize for our recv buffer
constexpr auto MAX_BUFFER = 65536;

// receive buffer, receive from local client 
static char recvBuf[MAX_BUFFER];

// used to store buffer from remote server
static char remoteBuf[MAX_BUFFER];

// used to parse http request
static HttpParser* hp;

// server socket
SOCKET serverSock;

// declaration of thread program
unsigned __stdcall threadProxyProgram(void* clientS);

// sock parameter, pass to thread program
struct socks {
	SOCKET serverSock;
	SOCKET clientSock;
};

int main() {
	// start winsock version 2.2
	startSocketProgram(2, 2);
	// set up TCP protocol, create socket, bind and listen to it
	serverSock = initServerSocket();

	HANDLE subprogram;

	while (1) {

		socks* sockPara = new socks;

		// server socket is initialized already
		sockPara->serverSock = serverSock;
		// accept client socket
		sockPara->clientSock = acceptSocket(serverSock);

		if (sockPara->clientSock != INVALID_SOCKET) {
			// call thread program to process 
			subprogram = (HANDLE)_beginthreadex(NULL, 0, &threadProxyProgram, (void*)sockPara, 0, 0);
			CloseHandle(subprogram);
		}
		else 
			printf("cannot accept local call!\n");
		
		Sleep(200);
	}
		
	// close server socket 
	closeTCPsocket(serverSock);

	// close winsock program, release resources
	endSocketProgram();
}

// thread program, used to deal with CURRENT SOCKET CONNECTION
unsigned __stdcall threadProxyProgram(void* sockPara) {
	// receive buffer length
	int recvLen = 1;
	// remote buffer receive length
	int remoteRecvLen = 1;

	// remote socket to be connected
	SOCKET connectSocket = INVALID_SOCKET;

	// build current client socket
	SOCKET clientSock = ((socks*)sockPara)->clientSock;

	// recv data from client socket
	recvLen = recv(clientSock, recvBuf, MAX_BUFFER, 0);

	if (recvLen > 0) {

		hp = new HttpParser();
		hp->parse(recvBuf);

		printf("\n");

		hp->printRequestLine();


		// get user IP, we can filter specific user here
		//auto peerInfo = getpeerInfo(clientSock);
		//printf("IP %s\nPort %d\n", getIPfromSockaddr(peerInfo).c_str(), getPortfromSockaddr(peerInfo));


		// redirect request, we can change our http parser's url to construct new request
		//
		//

		// website filter, read http parser to judge if current website should be filtered or not
		//
		///



		// make sure our request is HTTP
		if (hp->requesLine.method == "GET") {

			// connect to remote server
			connectSocket = connetToRemote(hp->headerLines["Host"]);

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
				// receive data from remote connection
				remoteRecvLen = recv(connectSocket, remoteBuf, MAX_BUFFER, 0);
				if (remoteRecvLen > 0) {

					printf("Bytes received: %d\n", remoteRecvLen);
					printf("%s\n", remoteBuf);

					send(clientSock, remoteBuf, sizeof(remoteBuf), 0);
				}
				else if (remoteRecvLen == 0)
					printf("remote connection closed\n");
				else
					printf("recv from remote server failed: %d\n", WSAGetLastError());
			} while (remoteRecvLen > 0);


			// close remote connection
			closeTCPsocket(connectSocket);
		}
		
		delete hp;
	}
	else if (recvLen == 0)
		printf("client connection closed\n");
	else
		printf("recv from client socket failed %d\n", WSAGetLastError());

	// close client connection
	closeTCPsocket(clientSock);
	// wait
	Sleep(200);
	// delete thread parameter to free memory
	delete (socks*)sockPara;

	// close currnet thread
	_endthreadex(0);

	return 0;
}