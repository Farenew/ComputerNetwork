#include <iostream>
#include "mysocket.h"
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

using std::cout;
using std::endl;

const int BUFFER_LENGTH = 1024;

const char* SERVER_ADDR = "127.0.0.1";
const unsigned short SERVER_PORT = 8888;

int main() {
	// start winsock version 2.2
	startSocketProgram(2, 2);
	// allocate socket 
	SOCKET clientSocket = init_client_socket();

	// 初始化服务器的地址，端口
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	int serverAddrSize = sizeof(serverAddr);

	// receive data buffer
	char buffer[BUFFER_LENGTH];
	ZeroMemory(buffer, sizeof(buffer));

	int i = 0;
	string input;

	while (true) {
		cout << "\n-----------------------------------\n";
		cout << "input '-time' to get time\n";
		cout << "input '-quit' to stop transmission\n";
		cout << "input '-testgbn to start receive from server\n";
		cout << "-----------------------------------\n";

		std::cin >> input;

		i = sendto(clientSocket, input.c_str(), input.size(), 0,
			(SOCKADDR*)& serverAddr, sizeof(serverAddr));
		if (i == SOCKET_ERROR) {
			 wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
			 closesocket(clientSocket);
			 WSACleanup();
			 return 1;
		}


		// 必须等待一下再接收，不然就会收不到
		Sleep(200);
		
		i = recvfrom(clientSocket, buffer, BUFFER_LENGTH, 0,
			(SOCKADDR*)& serverAddr, &serverAddrSize);
		//if (i == SOCKET_ERROR) {
		//	wprintf(L"receive failed with error: %d\n", WSAGetLastError());
		//	closesocket(clientSocket);
		//	WSACleanup();
		//	return 1;
		//}

		cout << "recving from server: ";
		cout << buffer << endl;


		//ZeroMemory(buffer, sizeof(buffer));
		Sleep(500);
	}

	// close server socket 
	closeSocket(clientSocket);
	// close winsock program, release resources
	endSocketProgram();
}

