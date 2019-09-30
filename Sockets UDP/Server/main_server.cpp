#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

enum Protocol
{
	UDP,
	TCP
};

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

void server(int port)
{
	// TODO-1: Winsock init
	WSAData wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("SERVER -> ERROR WSAStartup: ");
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP
	SOCKET s = socket(AF_INET, SOCK_DGRAM, UDP);

	// TODO-3: Force address reuse
	int enable = 1;
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)& enable, sizeof(int));
	if (iResult == SOCKET_ERROR)
	{
		printWSErrorAndExit("SERVER -> ERROR setsockopt: ");
	}

	// TODO-4: Bind to a local address
	struct sockaddr_in sAddress;
	sAddress.sin_family = AF_INET; // IPv4
	sAddress.sin_port = htons(port); // Port
	sAddress.sin_addr.S_un.S_addr = INADDR_ANY; // Any local IP address

	//BIND
	iResult = bind(s, (const struct sockaddr*) & sAddress, sizeof(sAddress));
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("SERVER -> ERROR bind: ");
	}

	while (true)
	{
		// TODO-5:
		// - Receive 'ping' packet from a remote host
		// - Receive 'pong' packet from the server
		char buf_ping[10];
		int flags = 0;
		int sizeOfAddress = sizeof(sAddress);
		iResult = recvfrom(s, buf_ping, sizeof(char) * 10, flags, (struct sockaddr*) & sAddress, &sizeOfAddress);
		if (iResult == 0)
		{
			printWSErrorAndExit("SERVER -> ERROR recvfrom: ");
		}

		std::cout << buf_ping << std::endl;

		// - Answer with a 'pong' packet
		std::string buf_pong = "PONG";
		iResult = sendto(s, buf_pong.c_str(), strlen(buf_pong.c_str()), flags, (const struct sockaddr*) & sAddress, sizeof(sAddress));
		if (iResult == 0)
		{
			printWSErrorAndExit("SERVER -> ERROR sendto: ");
		}

		// - Control errors in both cases
	}

	// TODO-6: Close socket
	iResult = closesocket(s);
	{
		printWSErrorAndExit("SERVER -> ERROR closesocket: ");
	}

	// TODO-7: Winsock shutdown
	iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("SERVER -> ERROR WSACleanup: ");
	}
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
