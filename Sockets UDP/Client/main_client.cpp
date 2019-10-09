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
	// Winsock init
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		printWSErrorAndExit("WSAStartup");
	}
<<<<<<< HEAD
	std::cout << "WSAStartup done" << std::endl;

	// Create socket (IPv4, datagrams, UDP
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET) {
		printWSErrorAndExit("socket");
=======
	
	// TODO-2: Create socket (IPv4, datagrams, UDP)
	SOCKET s = socket(AF_INET, SOCK_DGRAM, UDP);

	// TODO-3: Create an address object with the server address
	sockaddr_in sAddress;
	sAddress.sin_family = AF_INET;
	sAddress.sin_port = SERVER_PORT;
	inet_pton(AF_INET, SERVER_ADDRESS, &sAddress.sin_addr);

	// From address (will come from server)
	sockaddr fromAddr;

	//BIND
	iResult = bind(s, (const struct sockaddr*) & sAddress, sizeof(sAddress));
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("CLIENT -> ERROR bind: ");
>>>>>>> origin/master
	}
	std::cout << "socket done" << std::endl;

	// Client string
	std::string pingString("Ping");

	// Server Address
	struct sockaddr_in serverAddr;
	const int serverAddrLen = sizeof(serverAddr);
	serverAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, serverAddrStr, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port); // Port

	// From address (will come from server)
	struct sockaddr fromAddr;

	// Input buffer
	const int inBufferLen = 1300;
	char inBuffer[inBufferLen];

	while (true)
	{
<<<<<<< HEAD
		// Send
		int bytes = sendto(s, pingString.c_str(), (int)pingString.size() + 1, 0, (sockaddr*)&serverAddr, serverAddrLen);
		if (bytes >= 0)
=======
		// TODO-4:
		// - Send a 'ping' packet to the server
		std::string buf_ping = "PING";
		int flags = 0;
		iResult = sendto(s, buf_ping.c_str(), strlen(buf_ping.c_str())+1, flags, (const struct sockaddr*)&sAddress, sizeof(sAddress));
		if (iResult == SOCKET_ERROR)
>>>>>>> origin/master
		{
			std::cout << "Sent: " << pingString.c_str() << std::endl;

<<<<<<< HEAD
			std::cout << "Waiting for server data... " << std::flush;

			// Receive
			int fromAddrLen = sizeof(fromAddr);
			bytes = recvfrom(s, inBuffer, inBufferLen, 0, &fromAddr, &fromAddrLen);
			if (bytes == SOCKET_ERROR) {
				printWSErrorAndExit("recvfrom");
			}

			std::cout << "Received: " << inBuffer << std::endl;

			// Wait 1 second
			Sleep(1000);
		}
		else
=======
		// - Receive 'pong' packet from the server
		char buf_pong[10];
		int fromAddrLen = sizeof(fromAddr);
		iResult = recvfrom(s, buf_pong, sizeof(char) * 10, flags, &fromAddr, &fromAddrLen);
		if (iResult == SOCKET_ERROR)
>>>>>>> origin/master
		{
			printWSErrorAndExit("sendto");
		}
	}

	// Close socket
	closesocket(s);

	// Winsock shutdown
	WSACleanup();
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
