#include "GBN_program.h"

#include <ctime>
#include <chrono>
#include <vector>

#pragma warning(disable:4996) 

string getCurTime() {
	auto time = std::chrono::system_clock::now();
	std::time_t curTime = std::chrono::system_clock::to_time_t(time);

	return string(std::ctime(&curTime));
}

std::vector<bool> slidingWindow(10, true);

void GBN_test(SOCKET serverSock, struct sockaddr_in& clientAddr, const string fileToSend) {
	
}