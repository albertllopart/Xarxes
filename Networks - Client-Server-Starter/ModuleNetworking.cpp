#include "Networks.h"
#include "ModuleNetworking.h"
#include <list>

static uint8 NumModulesUsingWinsock = 0;



void ModuleNetworking::reportError(const char* inOperationDesc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = WSAGetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	ELOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
}

void ModuleNetworking::disconnect()
{
	for (SOCKET socket : sockets)
	{
		shutdown(socket, 2);
		closesocket(socket);
	}

	sockets.clear();
}

bool ModuleNetworking::init()
{
	if (NumModulesUsingWinsock == 0)
	{
		NumModulesUsingWinsock++;

		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(version, &data) != 0)
		{
			reportError("ModuleNetworking::init() - WSAStartup");
			return false;
		}
	}

	return true;
}

bool ModuleNetworking::preUpdate()
{
	if (sockets.empty()) return true;

	// NOTE(jesus): You can use this temporary buffer to store data from recv()
	const uint32 incomingDataBufferSize = Kilobytes(1);
	byte incomingDataBuffer[incomingDataBufferSize];

	// TODO(jesus): select those sockets that have a read operation available

	// New socket set
	fd_set readfds;
	FD_ZERO(&readfds);

	// Fill the set
	for (auto s : sockets) {
		FD_SET(s, &readfds);
	}

	// Timeout (return immediately)
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// Select (check for readability)
	int res = select(0, &readfds, nullptr, nullptr, &timeout);
	if (res == SOCKET_ERROR) {
		ELOG("SELECT SOCKET ERROR");
		return false;
	}
	
	// TODO(jesus): for those sockets selected, check wheter or not they are
	// a listen socket or a standard socket and perform the corresponding
	// operation (accept() an incoming connection or recv() incoming data,
	// respectively).

	for (int i = 0; i < readfds.fd_count; i++)
	{
	// On accept() success, communicate the new connected socket to the
	// subclass (use the callback onSocketConnected()), and add the new
	// connected socket to the managed list of sockets.
	// On recv() success, communicate the incoming data received to the
	// subclass (use the callback onSocketReceivedData()).
	// Fill this array with disconnected sockets

		SOCKET current = readfds.fd_array[i];

		if (isListenSocket(current))
		{
			sockaddr_in bindAddr;
			int fromlen = sizeof(bindAddr);

			SOCKET client = accept(current, (sockaddr*)&bindAddr, &fromlen);

			if (client == INVALID_SOCKET)
			{
				wchar_t* error = NULL;
				FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, WSAGetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPWSTR)&error, 0, NULL);

				ELOG("Socket error: %ls", error);

				continue;
			}

			onSocketConnected(client, bindAddr);
			sockets.push_back(client);

		}
		else
		{
			InputMemoryStream packet;
			int bytesRead = recv(current, packet.GetBufferPtr(), packet.GetCapacity(), 0);
			if (bytesRead == SOCKET_ERROR)
			{
				int lastError = WSAGetLastError();

				if (lastError == WSAEWOULDBLOCK)
				{
					// Do nothing special, there was no data to receive
				}
				else
				{
					WLOG("Socket disconnected");

					onSocketDisconnected(current);

					for (std::vector<SOCKET>::iterator it = sockets.begin(); it != sockets.end(); ++it)
					{
						if ((*it) == current)
						{
							sockets.erase(it);
							break;
						}
					}
					continue;
				}
			}
			else if (bytesRead == 0)
			{
				onSocketDisconnected(current);
				for (std::vector<SOCKET>::iterator it = sockets.begin(); it != sockets.end(); ++it)
				{
					if ((*it) == current)
					{
						sockets.erase(it);
						break;
					}
				}
				continue;
			}
			else // Success
			{
				// Process received data
				packet.SetSize(bytesRead);
				onSocketReceivedData(current, packet);
			}

		}
	}
	// TODO(jesus): Finally, remove all disconnected sockets from the list
	// of managed sockets.

	return true;
}

bool ModuleNetworking::cleanUp()
{
	disconnect();

	NumModulesUsingWinsock--;
	if (NumModulesUsingWinsock == 0)
	{

		if (WSACleanup() != 0)
		{
			reportError("ModuleNetworking::cleanUp() - WSACleanup");
			return false;
		}
	}

	return true;
}

bool ModuleNetworking::sendPacket(const OutputMemoryStream& packet, SOCKET socket)
{
	int result = send(socket, packet.GetBufferPtr(), packet.GetSize(), 0);
	if (result == SOCKET_ERROR)
	{
		reportError("send");
		return false;
	}
	return true;
}

void ModuleNetworking::addSocket(SOCKET socket)
{
	sockets.push_back(socket);
	DLOG("addSocket(): socked added");
}
