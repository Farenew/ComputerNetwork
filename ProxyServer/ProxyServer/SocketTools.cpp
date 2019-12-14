#include "SocketTools.h"

// from accepted client socket to get client ip and client port
sockaddr_in getpeerInfo(SOCKET clientSock) {
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int i = getpeername(clientSock, (struct sockaddr*) & addr, &len);
	if (i != 0) {
		printf("get peer failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}
	return addr;
}

string getIPfromSockaddr(sockaddr_in& sa) {
	char ip[120];
	inet_ntop(sa.sin_family, &sa.sin_addr, ip, sizeof(ip));
	return string(ip);
}

unsigned getPortfromSockaddr(sockaddr_in& sa) {
	return (unsigned)ntohs(sa.sin_port);
}

// from host name resolve all ipv4
vector<string> getIPv4FromHost(string hostName) {
	struct addrinfo hints, * infoptr;
	vector<string> Ipv4;
	char host[256];

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;

	getaddrinfo(hostName.c_str(), NULL, &hints, &infoptr);

	for (auto p = infoptr; p != nullptr; p = p->ai_next) {
		getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
		Ipv4.emplace_back(string(host));
	}
	return Ipv4;
}

