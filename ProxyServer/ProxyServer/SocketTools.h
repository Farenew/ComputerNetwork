#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

// get peer info from socket
sockaddr_in getpeerInfo(SOCKET clientSock);

// from sockaddr_in struct get ip 
std::string getIPfromSockaddr(sockaddr_in& sa);

// from sockaddr_in struct get port
unsigned getPortfromSockaddr(sockaddr_in& sa);

// from host name resolve all ipv4
vector<string> getIPv4FromHost(string hostName);

