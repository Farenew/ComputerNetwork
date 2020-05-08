#include <iostream>
#include "mysocket.h"
#include "GBN_program.h"
#include <string>

#include <fstream>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

// server socket
SOCKET serverSock;

using std::cout;
using std::endl;

const int BUFFER_LENGTH = 1024;

// 测试发送的文件
auto GBN_TESTING_FILE = "G:/tt.txt";

int main() {
	// start winsock version 2.2
	startSocketProgram(2, 2);
	// allocate socket 
	serverSock = initServerSocket();
	// bind socket
	struct sockaddr_in serverAddr = bindSocket(serverSock);

	// receive data buffer
	char buffer[BUFFER_LENGTH];
	ZeroMemory(buffer, sizeof(buffer));

	struct sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	int recvSize = 0;

	int i = 0;

	cout << "Receiving datagrams....\n";
	while (true) {
		 recvSize = recvfrom(serverSock, buffer, BUFFER_LENGTH, 0,
			(SOCKADDR*)& clientAddr, &clientAddrSize);

		 if (recvSize < 0) {
			 Sleep(200);
			 continue;
		 }

		 cout << "receiving from client:";
		 cout << buffer << endl;

		 if (strcmp(buffer, "-time") == 0) {
			 string curTime = get_cur_time();
			 // 这里sendto的size都做了加一，是为了包括字符串的\0结尾。
			 i = sendto(serverSock, curTime.c_str(), curTime.size()+1, 0,
				 (SOCKADDR*)& clientAddr, sizeof(clientAddr));
		 }
		 else if (strcmp(buffer, "-quit") == 0) {
			 i = sendto(serverSock, "Good Bye!", strlen("Good Bye!")+1, 0,
				 (SOCKADDR*)& clientAddr, sizeof(clientAddr));

			 closeSocket(serverSock);
			 endSocketProgram();
			 return 0;
		 }
		 else if (strcmp(buffer, "-testgbn") == 0) {
			 // 读文件到ss里面
			 std::ifstream file(GBN_TESTING_FILE, std::ios_base::in);
			 std::stringstream ss;
			 ss << file.rdbuf();
			
			 GBN_test(serverSock, clientAddr, ss.str());
		 }
		 else {
			 i = sendto(serverSock, buffer, strlen(buffer)+1, 0,
				 (SOCKADDR*)& clientAddr, sizeof(clientAddr));
		 }

		 if (i == SOCKET_ERROR) {
			 wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
			 closesocket(i);
			 WSACleanup();
			 return 1;
		 }

		 ZeroMemory(buffer, sizeof(buffer));
		 Sleep(500);
	}





	// close server socket 
	closeSocket(serverSock);
	// close winsock program, release resources
	endSocketProgram();
}

