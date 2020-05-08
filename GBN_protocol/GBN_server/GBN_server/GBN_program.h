#pragma once

#include <string>
#include <winsock2.h>

using std::string;

string get_cur_time();

void GBN_test(SOCKET& serverSock, struct sockaddr_in& clientAddr, const string& fileToSend);
