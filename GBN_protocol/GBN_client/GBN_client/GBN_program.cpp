#include "GBN_program.h"

#include <iostream>


using std::cout;
using std::endl;


const auto BUFFER_LENGTH = 1025;
// 这里的窗口值必须和serer里一致
const auto SEQ_LENGTH = 10;

// 随机数生成器
std::random_device rd;
std::mt19937 mt(rd());
// 均匀分布
std::uniform_real_distribution<double> dist(0.0, 1.0);


bool packet_is_lost(float prob) {
	if (dist(mt) < prob)
		return true;
	else
		return false;
}

bool ack_is_lost(float prob) {
	if (dist(mt) < prob)
		return true;
	else
		return false;
}

string GBN_test_client(SOCKET& clientSocket, struct sockaddr_in& serverAddr, float packetLossRatio, float ackLossRatio) {
	
	// 这里要用阻塞态，来等待服务器的数据。
	int iMode = 0; //1：非阻塞，0：阻塞 
	ioctlsocket(clientSocket, FIONBIO, (u_long FAR*) & iMode);

	// 存储传输的字符串
	string recvString;

	bool runFlag = true;
	int stage = 0;

	int recvSize = 0;
	int serverAddrSize = sizeof(serverAddr);

	int i;

	int recvSeq = 0;
	int waitSeq = 0;

	char buffer[BUFFER_LENGTH] = { 0 };

	// 测试主循环部分
	while (runFlag) {
		switch (stage)
		{
		case 0: {
			recvSize = recvfrom(clientSocket, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& serverAddr, &serverAddrSize);

			// 阻塞态情况下recvSize应该是大于0的，因为要等到有数据才会接收
			if (recvSize < 0) {
				cout << "unable to recv from server!" << endl;
				exit(1);
			}

			// 在stage 0，建立连接，判断是否收到205状态码
			if (static_cast<unsigned char>(buffer[0]) == 205) {

				cout << "received 205 code from server, ready for transmission!" << endl;
				buffer[0] = 200;
				buffer[1] = '\0';

				// 发送200码，说明连接正常建立
				i = sendto(clientSocket, buffer, strlen(buffer) + 1, 0,
					(SOCKADDR*)& serverAddr, sizeof(serverAddr));
				if (i == SOCKET_ERROR) {
					wprintf(L"stage 0 sendto failed with error: %d\n", WSAGetLastError());
					closesocket(i);
					WSACleanup();
					exit(1);
				}

				// 更改状态
				stage = 1;
				// 初始等待序列号为1，这里的设置也必须同server处一致，server最初发送的序号必须为1
				waitSeq = 1;
			}
			else {
				cout << "rececived code is not 205, error!" << endl;
				runFlag = false;
			}
			break;
		}
		case 1: {

			recvSize = recvfrom(clientSocket, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& serverAddr, &serverAddrSize);

			if (recvSize < 0) {
				cout << "unable to recv from server!" << endl;
				exit(1);
			}

			recvSeq = static_cast<unsigned short>(buffer[0]);

			// 模拟包没有发送过来
			if (packet_is_lost(packetLossRatio)) {
				cout << "imitating receiving packet lost" << endl;
				continue;
			}

			// 收到的packet小于等于当前，说明为正确接收或者冗余
			if(recvSeq <= waitSeq){

				cout << "received seq " << recvSeq << endl;
		
				// 如果是冗余，那么这里设置回传的ack为最后接收的有效ack；也可以设置为当前接收的ack或者是不回传。三种不同设计需要不同的实现代码
				if(recvSeq < waitSeq) {
					buffer[0] = waitSeq == 0? SEQ_LENGTH-1: waitSeq-1;
					buffer[1] = '\0';
				}
				// 当接收的包刚好是需要的包，而不是冗余的包
				else {
					// 把收到的内容放到字符串里
					recvString.append(&buffer[1]);

					// 设置回传的ack
					buffer[0] = waitSeq;
					buffer[1] = '\0';

					// ack丢失不会影响本地waitseq变化，因此只要接收正常waitseq就加一
					waitSeq++;
					waitSeq %= SEQ_LENGTH;
					
				}

				// 模拟ack包中途丢失
				if (ack_is_lost(ackLossRatio)) {
					cout << "imitating ack lost" << endl;
				}
				else {

					cout << "send ack " << static_cast<unsigned int>(buffer[0]) << endl;

					i = sendto(clientSocket, buffer, 2, 0,
						(SOCKADDR*)& serverAddr, sizeof(serverAddr));
					if (i == SOCKET_ERROR) {
						wprintf(L"stage 0 sendto failed with error: %d\n", WSAGetLastError());
						closesocket(i);
						WSACleanup();
						exit(1);
					}
				}
				
			}
			// 如果接收到终止信号
			else if (static_cast<unsigned char>(buffer[0]) == 211) {
				cout << "received terminate signal" << endl;
				stage = 3;

				// 发送211码，说明测试结束，这里就不再等待了，直接设置runflag结束。
				buffer[0] = 211;
				buffer[1] = '\0';
				i = sendto(clientSocket, buffer, 2, 0,
					(SOCKADDR*)& serverAddr, sizeof(serverAddr));
				if (i == SOCKET_ERROR) {
					wprintf(L"stage 0 sendto failed with error: %d\n", WSAGetLastError());
					closesocket(i);
					WSACleanup();
					exit(1);
				}

				runFlag = false;
			}
			// 这种情况是recvSeq > waitSeq，此时说明server发过来的包中途丢失
			else {
				cout << "received package not expected" << endl;
			}

			break;
		}
		default:
			break;
		}

		ZeroMemory(buffer, BUFFER_LENGTH);
		Sleep(500);
	}


	return recvString;
}