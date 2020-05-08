#include "GBN_program.h"

#include <iostream>


using std::cout;
using std::endl;


const auto BUFFER_LENGTH = 1025;
// ����Ĵ���ֵ�����serer��һ��
const auto SEQ_LENGTH = 10;

// �����������
std::random_device rd;
std::mt19937 mt(rd());
// ���ȷֲ�
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
	
	// ����Ҫ������̬�����ȴ������������ݡ�
	int iMode = 0; //1����������0������ 
	ioctlsocket(clientSocket, FIONBIO, (u_long FAR*) & iMode);

	// �洢������ַ���
	string recvString;

	bool runFlag = true;
	int stage = 0;

	int recvSize = 0;
	int serverAddrSize = sizeof(serverAddr);

	int i;

	int recvSeq = 0;
	int waitSeq = 0;

	char buffer[BUFFER_LENGTH] = { 0 };

	// ������ѭ������
	while (runFlag) {
		switch (stage)
		{
		case 0: {
			recvSize = recvfrom(clientSocket, buffer, BUFFER_LENGTH, 0,
				(SOCKADDR*)& serverAddr, &serverAddrSize);

			// ����̬�����recvSizeӦ���Ǵ���0�ģ���ΪҪ�ȵ������ݲŻ����
			if (recvSize < 0) {
				cout << "unable to recv from server!" << endl;
				exit(1);
			}

			// ��stage 0���������ӣ��ж��Ƿ��յ�205״̬��
			if (static_cast<unsigned char>(buffer[0]) == 205) {

				cout << "received 205 code from server, ready for transmission!" << endl;
				buffer[0] = 200;
				buffer[1] = '\0';

				// ����200�룬˵��������������
				i = sendto(clientSocket, buffer, strlen(buffer) + 1, 0,
					(SOCKADDR*)& serverAddr, sizeof(serverAddr));
				if (i == SOCKET_ERROR) {
					wprintf(L"stage 0 sendto failed with error: %d\n", WSAGetLastError());
					closesocket(i);
					WSACleanup();
					exit(1);
				}

				// ����״̬
				stage = 1;
				// ��ʼ�ȴ����к�Ϊ1�����������Ҳ����ͬserver��һ�£�server������͵���ű���Ϊ1
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

			// ģ���û�з��͹���
			if (packet_is_lost(packetLossRatio)) {
				cout << "imitating receiving packet lost" << endl;
				continue;
			}

			// �յ���packetС�ڵ��ڵ�ǰ��˵��Ϊ��ȷ���ջ�������
			if(recvSeq <= waitSeq){

				cout << "received seq " << recvSeq << endl;
		
				// ��������࣬��ô�������ûش���ackΪ�����յ���Чack��Ҳ��������Ϊ��ǰ���յ�ack�����ǲ��ش������ֲ�ͬ�����Ҫ��ͬ��ʵ�ִ���
				if(recvSeq < waitSeq) {
					buffer[0] = waitSeq == 0? SEQ_LENGTH-1: waitSeq-1;
					buffer[1] = '\0';
				}
				// �����յİ��պ�����Ҫ�İ�������������İ�
				else {
					// ���յ������ݷŵ��ַ�����
					recvString.append(&buffer[1]);

					// ���ûش���ack
					buffer[0] = waitSeq;
					buffer[1] = '\0';

					// ack��ʧ����Ӱ�챾��waitseq�仯�����ֻҪ��������waitseq�ͼ�һ
					waitSeq++;
					waitSeq %= SEQ_LENGTH;
					
				}

				// ģ��ack����;��ʧ
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
			// ������յ���ֹ�ź�
			else if (static_cast<unsigned char>(buffer[0]) == 211) {
				cout << "received terminate signal" << endl;
				stage = 3;

				// ����211�룬˵�����Խ���������Ͳ��ٵȴ��ˣ�ֱ������runflag������
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
			// ���������recvSeq > waitSeq����ʱ˵��server�������İ���;��ʧ
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