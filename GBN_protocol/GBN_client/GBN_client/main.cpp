#include <iostream>
#include "mysocket.h"
#include <string>
#include <random>
#include <sstream>

#include "GBN_program.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

using std::cout;
using std::endl;

// �����buffer��С��GBN������Ĳ�ͬ����GBN���Բ��ֻ��������buffer
const int BUFFER_LENGTH = 1024;

const char* SERVER_ADDR = "127.0.0.1";
const unsigned short SERVER_PORT = 8888;

int main() {
	// start winsock version 2.2
	startSocketProgram(2, 2);
	// allocate socket 
	SOCKET clientSocket = init_client_socket();

	// ��ʼ���������ĵ�ַ���˿�
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	int serverAddrSize = sizeof(serverAddr);

	// receive data buffer
	char buffer[BUFFER_LENGTH];
	ZeroMemory(buffer, sizeof(buffer));

	int i = 0; // i����״̬����������һЩ���������״̬
	string input;

	while (true) {
		cout << "\n-----------------------------------\n";
		cout << "input '-time' to get time\n";
		cout << "input '-quit' to stop transmission\n";
		cout << "input '-testgbn X Y to start receive from server\n";
		cout << "-----------------------------------\n";

		std::getline(std::cin, input);

		// �����ǰ����ָ��Ϊ-testgbn���������Խ׶Σ���������������
		if (input.substr(0, 8) == "-testgbn") {

			string packetLossRatio, ackLossRatio;

			// ��-testgbn X Y�ֽ�
			std::istringstream ss(input);
			std::getline(ss, input, ' ');	// input == "-testgbn"
			std::getline(ss, packetLossRatio, ' ');	// packetLossRatio = X
			std::getline(ss, ackLossRatio, ' ');	// ackLossRatio = Y

			i = sendto(clientSocket, input.c_str(), input.size(), 0,
				(SOCKADDR*)& serverAddr, sizeof(serverAddr));

			if (i == SOCKET_ERROR) {
				wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}

			// ����GBN���ݴ��䣬����������string
			auto recvData = GBN_test_client(clientSocket, serverAddr, 
				std::stof(packetLossRatio), std::stof(ackLossRatio));
			
			if (recvData.size() != 0)
				cout << "received content: " << recvData << endl;
			else
				cout << "testgbn failed!" << endl;
		}
		else {

			i = sendto(clientSocket, input.c_str(), input.size(), 0,
				(SOCKADDR*)& serverAddr, sizeof(serverAddr));
			if (i == SOCKET_ERROR) {
				wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}

			// sendto�����ȴ�һ���ٽ��գ���Ȼ�ͻ��ղ���
			Sleep(200);

			i = recvfrom(clientSocket, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& serverAddr, &serverAddrSize);

			cout << "recving from server: ";
			cout << buffer << endl;
		}

		//ZeroMemory(buffer, sizeof(buffer));
		Sleep(500);
	}

	// close server socket 
	closeSocket(clientSocket);
	// close winsock program, release resources
	endSocketProgram();
}

