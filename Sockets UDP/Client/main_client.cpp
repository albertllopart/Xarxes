#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"

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

void client(const char *serverAddrStr, int port)
{
	// TODO-1: Winsock init
	WSAData wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR WSAStartup: ");
	}
	
	// TODO-2: Create socket (IPv4, datagrams, UDP)
	SOCKET s = socket(AF_INET, SOCK_DGRAM, UDP);

	// TODO-3: Create an address object with the server address
	sockaddr_in sAddress;
	sAddress.sin_family = AF_INET;
	sAddress.sin_port = SERVER_PORT;
	inet_pton(AF_INET, SERVER_ADDRESS, &sAddress.sin_addr);

	//BIND
	iResult = bind(s, (const struct sockaddr*) & sAddress, sizeof(sAddress));
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR bind: ");
	}

	while (true)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		const char *buf_ping = "PING";
		int flags = 0;
		iResult = sendto(s, buf_ping, strlen(buf_ping) + 1, flags, (const struct sockaddr*)&sAddress, sizeof(sAddress));
		if (iResult != NO_ERROR)
		{
			printWSErrorAndExit("ERROR sendto: ");
		}

		// - Receive 'pong' packet from the server
		char* buf_pong = nullptr;
		int sizeOfAddress = sizeof(sAddress);
		iResult = recvfrom(s, buf_pong, strlen(buf_pong) + 1, flags, (struct sockaddr*)&sAddress, &sizeOfAddress);
		if (iResult != NO_ERROR)
		{
			printWSErrorAndExit("ERROR recvfrom: ");
		}

		// - Control errors in both cases
		std::cout << buf_pong << "/n";
	}

	// TODO-5: Close socket
	iResult = closesocket(s);
	{
		printWSErrorAndExit("ERROR closesocket: ");
	}

	// TODO-6: Winsock shutdown
	iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR WSACleanup: ");
	}
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
