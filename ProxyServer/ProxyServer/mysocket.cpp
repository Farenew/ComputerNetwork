#include <string>
#include "mysocket.h"

using std::string;

WSADATA startSocketProgram(char mainNum, char minNum) {
	WSADATA wsaData;

	int iResult;
	// Initialize Winsock with version mainNum.minNum
	iResult = WSAStartup(MAKEWORD(mainNum, minNum), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		exit(1);
	}

	if (LOBYTE(wsaData.wVersion) != mainNum || HIBYTE(wsaData.wVersion) != minNum) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		exit(1);
	}
	else
		printf("Current Winsock dll version check passed\n");

	return wsaData;
}

// this init procedure is old, origin from HIT's lab
SOCKET initServerSocket2() {

	sockaddr_in ProxyServerAddr;
	SOCKET listenSocket = INVALID_SOCKET;

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket function failed with error = %d\n", WSAGetLastError());
		exit(1);
	}
	else
		printf("socket successfully opened\n");

	ProxyServerAddr.sin_family = AF_INET;
	ProxyServerAddr.sin_port = htons(27015);
	ProxyServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(listenSocket, (SOCKADDR*)& ProxyServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	else
		printf("socket successfully binded\n");

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	else
		printf("now socket listen on port %s\n", SERVER_PORT);

	return listenSocket;
}


SOCKET initServerSocket() {
	struct addrinfo protoStat;
	struct addrinfo* result;
	int i;

	ZeroMemory(&protoStat, sizeof(protoStat));

	// AF_INET: The Internet Protocol version 4 (IPv4)address family.
	protoStat.ai_family = AF_INET;

	// SOCK_STREAM: application layer to TCP
	protoStat.ai_socktype = SOCK_STREAM;

	// TCP protocol
	protoStat.ai_protocol = IPPROTO_TCP;

	// 表明后续使用INADDR_ANY
	protoStat.ai_flags = AI_PASSIVE;

	// 这里相当于用getaddrinfo封装出一个addrinfo结构到result里
	i = getaddrinfo(NULL, SERVER_PORT, &protoStat, &result);
	if (i != 0) {
		printf("getaddrinfo failed: %d\n", i);
		WSACleanup();
		exit(1);
	}

	// create socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket function failed with error = %d\n", WSAGetLastError());
		exit(1);
	}
	else
		printf("socket successfully opened\n");

	// bind this socket to this IP address and port from result
	i = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (i == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	else
		printf("socket successfully binded\n");

	// free memory allocated by getaddrinfo
	freeaddrinfo(result);

	// listen on socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	else
		printf("now socket listen on port %s\n", SERVER_PORT);

	return ListenSocket;
}

// accept client socket from listenSocket
SOCKET acceptSocket(SOCKET listenSocket) {
	SOCKET acceptSock = INVALID_SOCKET;
	acceptSock = accept(listenSocket, NULL, NULL);
	if (acceptSock == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(acceptSock);
		WSACleanup();
		return 1;
	}
	/*else {
		printf("accept socket\n");
	}*/
	return acceptSock;
}

// close TCP socket
void closeTCPsocket(SOCKET sock) {
	int iResult = closesocket(sock);
	if (iResult == SOCKET_ERROR) {
		printf("closesocket failed with error = %d\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}

	printf("socket is safely cloesd\n");
}

// end winsock program
void endSocketProgram() {
	WSACleanup();

	printf("winsock program is successfully closed\n");
}

