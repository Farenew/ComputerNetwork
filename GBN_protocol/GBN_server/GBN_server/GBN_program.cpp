#include "GBN_program.h"

#include <ctime>
#include <chrono>
#include <vector>
#include <cassert>

#include <iostream>

#pragma warning(disable:4996) 

using std::cout;
using std::endl;

//// FRAME��СΪ1023��Ϊ����1024����һ��\0
//const auto FRAME_SIZE = 1023;
//// buffer��frame��һ��byte��������ŵ�һ��seq
//const auto BUFFER_LENGTH = 1025;

// ����Ϊ�˲��Է��㣬��BUFFER��FRAME��С���õıȽ�С

// FRAME��СΪ23��Ϊ����24����һ��\0
const auto FRAME_SIZE = 23;
// buffer��frame��һ��byte��������ŵ�һ��seq
const auto BUFFER_LENGTH = 25;

// ��ʱ��ʱʱ�䣬10�ν���ʧ�ܾͳ�ʱ
const auto CLOCK_TIMEOUT = 10;

// ����buffer��С����ʵ����2���㹻��
const auto RECV_BUFFER_LENGTH = 10;

// ���кţ�ָ���Ƿ��͵İ��ı�ţ���0��9.
const auto SEQ_LENGTH = 10;
// ���ʹ��ڴ�С��ָ����һ����෢�İ�����
const auto WINDOWS_SIZE = 5;


string get_cur_time() {
	auto time = std::chrono::system_clock::now();
	std::time_t curTime = std::chrono::system_clock::to_time_t(time);

	return string(std::ctime(&curTime));
}

// ����seq��ack�жϵ�ǰ�Ƿ��ڷ������ݴ���
bool is_seq_in_window(int seq, int ack) {
	int length = seq > ack ? seq - ack : seq + SEQ_LENGTH - ack;
	if (length >= WINDOWS_SIZE)
		return false;
	else
		return true;
}

// ��ʱ������seq��packNum����
void sending_time_out_handler(unsigned& seq, unsigned ack, size_t& packNum) {
	int length = seq >= ack ? seq - ack : seq + SEQ_LENGTH - ack;
	seq = ack + 1;
	// seqλ��ack��һ��
	seq %= SEQ_LENGTH;
	// ע������ҲҪ��1
	packNum = packNum - length + 1;
}

void GBN_test(SOCKET& serverSock, struct sockaddr_in& clientAddr, const string& fileToSend) {
	const unsigned TOTAL_PACKAGE_NUM = fileToSend.size() / FRAME_SIZE;
	bool runFlag = true;
	int stage = 0;
	int i;

	// �����recvSize�൱��ָʾ����
	int recvSize = 0;
	int clientAddrSize = sizeof(clientAddr);

	int clock = 0;

	// ������Ҫ������ָʾ��ǰseq��ack�ͷ��͵�packagenum
	unsigned int curSeq = 0;
	unsigned int curAck = 0;
	size_t packageNum = 0;

	// ע�������ҷ�������buffer���ֱ��ŷ������ݺͽ�������
	char buffer[BUFFER_LENGTH] = { 0 };
	char recvBuffer[RECV_BUFFER_LENGTH] = { 0 };

	while (runFlag) {
		switch (stage)
		{
		case 0: {
			cout << "now sending 205 code to client" << endl;

			// ����״̬��Ϊ205������״̬���Ա�ʾ����������
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

			// ����״̬1
			stage = 1;

			break;
		}
		case 1: {
			cout << "now waiting for client to send 200 code" << endl;
			recvSize = recvfrom(serverSock, recvBuffer, RECV_BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// ģ�ⳬʱ����Ϊserver�Ƿ�����״̬��recvfromû���յ�����Ҳ�᷵�أ����������η���û�����ݾͼٶ�Ϊ��ʱ
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

				// �趨��ʼ״̬�������seq�����client�˽��յ�waitseq����һ��
				curSeq = 1;
				curAck = 0;
				packageNum = 0;

				stage = 2;
				clock = 0;

				ZeroMemory(recvBuffer, sizeof(recvBuffer));
			}
			else {
				cout << "recv code error! abort this transferring!" << endl;
				// runFlag = falseֱ���˳���δ������
				runFlag = false;
			}
			break;
		}
		case 2: {
			// �����ǰ���ʹ�С�ڷ��ʹ����ڣ���ô����һ��package
			if (is_seq_in_window(curSeq, curAck) && packageNum <= TOTAL_PACKAGE_NUM) {

				buffer[0] = static_cast<unsigned char>(curSeq);

				// �ѷ�������copy��buffer��
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

				// ����Ҫ��������һ�£���Ϊsendto�������recvfrom�����ȴ������
				Sleep(200);
			}

			recvSize = recvfrom(serverSock, recvBuffer, RECV_BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// ģ�ⳬʱ����Ϊ�����Ƿ�����״̬�����������η���û�����ݾͼٶ�Ϊ��ʱ
			if (recvSize < 0) {
				if (++clock == CLOCK_TIMEOUT) {
					cout << "Sending Timeout!" << endl;
					// ��ʱ����seq
					sending_time_out_handler(curSeq, curAck, packageNum);
					clock = 0;
				}
			}
			else {
				// �յ�packet
				auto recvAck = static_cast<unsigned int>(recvBuffer[0]);
				
				// ��������Ƚϸ��ӣ�Ҫ�ж�һ��seq�Ƿ�С��ack������ǵĻ�����ôҪ�ж�recvACK��λ�ã����ݲ�ͬ�������curAck
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

				// ����Ѿ����͵���β
				if (packageNum == TOTAL_PACKAGE_NUM + 1) {

					cout << "now transfer finished! ready to disconnet" << endl;

					buffer[0] = static_cast<unsigned char>(211);
					buffer[1] = '\0';

					// ����211������
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
			// �ȴ�211������ȷ��
			recvSize = recvfrom(serverSock, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& clientAddr, &clientAddrSize);

			// ��ʱ�ش�
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

		// ÿһ�β����궼�ȴ�һ��
		Sleep(500);
	}
}
