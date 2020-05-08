#include "GBN_program.h"

#include <ctime>
#include <chrono>
#include <vector>
#include <cassert>

#include <iostream>

#pragma warning(disable:4996) 

using std::cout;
using std::endl;

//// FRAME大小为1023，为了在1024处放一个\0
//const auto FRAME_SIZE = 1023;
//// buffer比frame多一个byte，用来存放第一个seq
//const auto BUFFER_LENGTH = 1025;

// 这里为了测试方便，把BUFFER和FRAME大小设置的比较小

// FRAME大小为23，为了在24处放一个\0
const auto FRAME_SIZE = 23;
// buffer比frame多一个byte，用来存放第一个seq
const auto BUFFER_LENGTH = 25;

// 超时计时时间，10次接受失败就超时
const auto CLOCK_TIMEOUT = 10;

// 接收buffer大小，其实设置2就足够了
const auto RECV_BUFFER_LENGTH = 10;

// 序列号，指的是发送的包的编号，从0到9.
const auto SEQ_LENGTH = 10;
// 发送窗口大小，指的是一次最多发的包个数
const auto WINDOWS_SIZE = 5;


string get_cur_time() {
	auto time = std::chrono::system_clock::now();
	std::time_t curTime = std::chrono::system_clock::to_time_t(time);

	return string(std::ctime(&curTime));
}

// 根据seq和ack判断当前是否在发送数据窗口
bool is_seq_in_window(int seq, int ack) {
	int length = seq > ack ? seq - ack : seq + SEQ_LENGTH - ack;
	if (length >= WINDOWS_SIZE)
		return false;
	else
		return true;
}

// 超时处理，把seq和packNum重置
void sending_time_out_handler(unsigned& seq, unsigned ack, size_t& packNum) {
	int length = seq >= ack ? seq - ack : seq + SEQ_LENGTH - ack;
	seq = ack + 1;
	// seq位于ack下一个
	seq %= SEQ_LENGTH;
	// 注意这里也要加1
	packNum = packNum - length + 1;
}

void GBN_test(SOCKET& serverSock, struct sockaddr_in& clientAddr, const string& fileToSend) {
	const unsigned TOTAL_PACKAGE_NUM = fileToSend.size() / FRAME_SIZE;
	bool runFlag = true;
	int stage = 0;
	int i;

	// 这里的recvSize相当于指示变量
	int recvSize = 0;
	int clientAddrSize = sizeof(clientAddr);

	int clock = 0;

	// 三个重要参数，指示当前seq，ack和发送的packagenum
	unsigned int curSeq = 0;
	unsigned int curAck = 0;
	size_t packageNum = 0;

	// 注意这里我分了两个buffer，分别存放发送数据和接收数据
	char buffer[BUFFER_LENGTH] = { 0 };
	char recvBuffer[RECV_BUFFER_LENGTH] = { 0 };

	while (runFlag) {
		switch (stage)
		{
		case 0: {
			cout << "now sending 205 code to client" << endl;

			// 设置状态码为205，发送状态码以表示服务器就绪
			buffer[0] = static_cast<unsigned char>(205);
			buffer[1] = '\0';

			i = sendto(serverSock, buffer, 2, 0,
				(SOCKADDR*)& clientAddr, sizeof(clientAddr));
			if (i == SOCKET_ERROR) {
				wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
				closesocket(i);
				WSACleanup();
				exit(1);
			}

			// 进入状态1
			stage = 1;

			break;
		}
		case 1: {
			cout << "now waiting for client to send 200 code" << endl;
			recvSize = recvfrom(serverSock, recvBuffer, RECV_BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// 模拟超时，因为server是非阻塞状态，recvfrom没有收到数据也会返回，所以如果多次返回没有数据就假定为超时
			if (recvSize < 0) {
				if (++clock == CLOCK_TIMEOUT) {
					clock = 0;
					runFlag = false;
					cout << "Timeout Error!" << endl;
				}
			}
			else if (static_cast<unsigned char>(recvBuffer[0]) == 200) {
				cout << "Transfer Established, ready to send file" << endl;
				cout << "----------------------------------\n";
				cout << "File Size is " << fileToSend.size() <<
					" bytes, total packet number is " << TOTAL_PACKAGE_NUM << endl;
				cout << "----------------------------------\n";

				// 设定初始状态，这里的seq必须和client端接收的waitseq保持一致
				curSeq = 1;
				curAck = 0;
				packageNum = 0;

				stage = 2;
				clock = 0;

				ZeroMemory(recvBuffer, sizeof(recvBuffer));
			}
			else {
				cout << "recv code error! abort this transferring!" << endl;
				// runFlag = false直接退出这次传输服务
				runFlag = false;
			}
			break;
		}
		case 2: {
			// 如果当前发送大小在发送窗口内，那么发送一个package
			if (is_seq_in_window(curSeq, curAck) && packageNum <= TOTAL_PACKAGE_NUM) {

				buffer[0] = static_cast<unsigned char>(curSeq);

				// 把发送内容copy到buffer里
				memcpy(&buffer[1], 
					fileToSend.substr(packageNum * FRAME_SIZE, FRAME_SIZE).c_str(), 
					FRAME_SIZE);

				buffer[BUFFER_LENGTH - 1] = '\0';

				cout << "send to client seq num is " << curSeq << endl;

				i = sendto(serverSock, buffer, BUFFER_LENGTH + 1, 0,
					(SOCKADDR*)& clientAddr, sizeof(clientAddr));
				if (i == SOCKET_ERROR) {
					wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
					closesocket(i);
					WSACleanup();
					exit(1);
				}

				
				curSeq++;
				curSeq %= SEQ_LENGTH;
				packageNum++;

				// 这里要额外休眠一下，因为sendto后面就是recvfrom，不等待会出错
				Sleep(200);
			}

			recvSize = recvfrom(serverSock, recvBuffer, RECV_BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// 模拟超时，因为这里是非阻塞状态，所以如果多次返回没有数据就假定为超时
			if (recvSize < 0) {
				if (++clock == CLOCK_TIMEOUT) {
					cout << "Sending Timeout!" << endl;
					// 超时重置seq
					sending_time_out_handler(curSeq, curAck, packageNum);
					clock = 0;
				}
			}
			else {
				// 收到packet
				auto recvAck = static_cast<unsigned int>(recvBuffer[0]);
				
				// 这里情况比较复杂，要判断一下seq是否小于ack，如果是的话，那么要判断recvACK的位置，根据不同情况更新curAck
				if (curSeq < curAck) {
					if (recvAck <= curSeq)
						curAck = recvAck;
					else if (recvAck > curAck)
						curAck = recvAck;
				}
				else {
					if (recvAck > curAck)
						curAck = recvAck;
				}

				cout << "received ack: " << recvAck << " curAck is " << curAck << endl;
				clock = 0;

				// 如果已经发送到结尾
				if (packageNum == TOTAL_PACKAGE_NUM + 1) {

					cout << "now transfer finished! ready to disconnet" << endl;

					buffer[0] = static_cast<unsigned char>(211);
					buffer[1] = '\0';

					// 发送211结束码
					i = sendto(serverSock, buffer, strlen(buffer) + 1, 0,
						(SOCKADDR*)& clientAddr, sizeof(clientAddr));
					if (i == SOCKET_ERROR) {
						wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
						closesocket(i);
						WSACleanup();
						exit(1);
					}

					stage = 3;
					clock = 0;
				}
			}

			ZeroMemory(recvBuffer, sizeof(recvBuffer));
			ZeroMemory(buffer, sizeof(buffer));

			break;
		}

		case 3: {
			// 等待211结束码确认
			recvSize = recvfrom(serverSock, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// 超时重传
			if (recvSize < 0) {
				if (++clock == CLOCK_TIMEOUT) {
					cout << "211 code recv Timeout!" << endl;
					clock = 0;

					cout << "resending 211 code to terminate" << endl;
					buffer[0] = static_cast<unsigned char>(211);
					buffer[1] = '\0';

					i = sendto(serverSock, buffer, strlen(buffer) + 1, 0,
						(SOCKADDR*)& clientAddr, sizeof(clientAddr));
					if (i == SOCKET_ERROR) {
						wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
						closesocket(i);
						WSACleanup();
						exit(1);
					}
				}
			}
			else if (static_cast<unsigned char>(buffer[0]) == 211) {
				cout << "successfully finished this transfer" << endl;
				runFlag = false;
			}

			break;
		}
	
		default:
			Sleep(500);
			break;
		}

		// 每一次操作完都等待一下
		Sleep(500);
	}
}
