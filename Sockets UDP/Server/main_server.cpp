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
	// Winsock init
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		printWSErrorAndExit("WSAStartup");
	}
	std::cout << "WSAStartup done" << std::endl;

	// Create socket (IPv4, datagrams, UDP
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET) {
		printWSErrorAndExit("socket");
	}
	std::cout << "socket done" << std::endl;

	// Reuse address
	int enable = 1;
	int result = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (result == SOCKET_ERROR) {
		printWSErrorAndExit("setsockopt");
	}
	std::cout << "setsockopt SO_REUSEADDR done" << std::endl;

	// Address (server)
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET; // IPv4
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; // Any address, will be localhost
	serverAddr.sin_port = htons(port); // Port

	// Bind socket
	int bindRes = bind(s, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (bindRes == SOCKET_ERROR) {
		printWSErrorAndExit("bind");
	}
	std::cout << "bind done on port " << port << std::endl;

	// Server string
	std::string pongString("Pong");

	// Input buffer
	const int inBufferLen = 1300;
	char inBuffer[inBufferLen];

	// From address (client)
	struct sockaddr fromAddr;
	int fromAddrLen = sizeof(fromAddr);

	// From address (client)
	sockaddr fromAddr;
	int fromAddrLen = sizeof(fromAddr);

	while (true)
	{
<<<<<<< HEAD
		std::cout << "Waiting for client data... " << std::flush;

		// Receive
		int bytes = recvfrom(s, inBuffer, inBufferLen, 0, &fromAddr, &fromAddrLen);
		if (bytes >= 0)
=======
		// Input buffer
		const int inBufferLen = 1300;
		char buf_ping[inBufferLen];
		// TODO-5:
		// - Receive 'ping' packet from a remote host
		// - Receive 'pong' packet from the server
		int flags = 0;
		int sizeOfAddress = sizeof(sAddress);
		iResult = recvfrom(s, buf_ping, sizeof(char) * 10, flags, (struct sockaddr*) & sAddress, &sizeOfAddress);
		if (iResult == SOCKET_ERROR)
>>>>>>> origin/master
		{
			std::cout << "Received: " << inBuffer << std::endl;

			// Wait 1 second
			Sleep(1000);

			// Send
			bytes = sendto(s, pongString.c_str(), (int)pongString.size() + 1, 0, (sockaddr*)&fromAddr, fromAddrLen);
			if (bytes == SOCKET_ERROR) {
				printWSErrorAndExit("sendto");
			}

<<<<<<< HEAD
			std::cout << "Sent: '" << pongString.c_str() << "' sent" << std::endl;
		}
		else
=======
		// - Answer with a 'pong' packet
		std::string buf_pong = "PONG";
		iResult = sendto(s, buf_pong.c_str(), strlen(buf_pong.c_str()), flags, (sockaddr*)& fromAddr, fromAddrLen);
		if (iResult == SOCKET_ERROR)
>>>>>>> origin/master
		{
			printWSErrorAndExit("recvfrom");
		}
	}

	// Close socket
	closesocket(s);

	// Winsock shutdown
	WSACleanup();
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
