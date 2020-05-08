#pragma once

#include <string>
#include <winsock2.h>
#include <random>

using std::string;

string GBN_test_client(SOCKET& clientSocket, struct sockaddr_in& serverAddr, float packetLossRatio, float ackLossRatio);