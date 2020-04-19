#include "mysocket.h"

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

// 初始化socket来接收UDP
SOCKET init_client_socket() {
	int i = 0;

	// create socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket function failed with error = %d\n", WSAGetLastError());
		exit(1);
	}
	else
		printf("socket for UDP successfully opened\n");

	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 1;
	i = ioctlsocket(ListenSocket, FIONBIO, &iMode);
	if (i != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", i);
	else
		printf("now setting socket as non-blocking mode\n");

	return ListenSocket;
}




// close socket
void closeSocket(SOCKET sock) {
	int iResult = closesocket(sock);
	if (iResult == SOCKET_ERROR) {
		printf("closesocket failed with error = %d\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}
}

// end winsock program
void endSocketProgram() {
	WSACleanup();

	printf("winsock program is successfully closed\n");
}

