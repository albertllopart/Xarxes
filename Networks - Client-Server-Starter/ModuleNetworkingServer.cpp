#include "ModuleNetworkingServer.h"
#include <cstdlib>
#include <iostream>
#include "Windows.h"
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

//////////////////////////////////////////////////////////////////////
// ERROR PRINTER
//////////////////////////////////////////////////////////////////////

void ModuleNetworkingServer::printWSErrorAndExit(const char* msg) const
{
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// Create socket (IPv4, stream, TCP)
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		ELOG("socket()");
	}
	LOG("socket done");

	// - Set address reuse
	int enable = 1;
	int result = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (result == SOCKET_ERROR) {
		ELOG("setsockopt()");
	}
	LOG("setsockopt SO_REUSEADDR done");

	// Address (server)
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET; // IPv4
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; // Any address
	serverAddr.sin_port = htons(port); // Port

	// - Bind the socket to a local interface
	int bindRes = bind(listenSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr));
	if (bindRes == SOCKET_ERROR) {
		ELOG("bind()");
	}
	std::string str = "bind done on port " + std::to_string(port);	
	LOG(str.c_str());

	// - Enter in listen mode
	int listenRes = listen(listenSocket, 1);
	if (listenRes == SOCKET_ERROR) {
		ELOG("listen()");
	}
	LOG("listen done");

	// - Add the listenSocket to the managed list of sockets using addSocket()
	addSocket(listenSocket);

	state = ServerState::Listening;

	return true;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}
		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in& socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// Set the player name of the corresponding connected socket proxy
	ClientMessage clientMessage;
	packet >> clientMessage;

	if (clientMessage == ClientMessage::Hello)
	{
		std::string playerName;
		packet >> playerName;

		for (auto& connectedSocket : connectedSockets)
		{
			OutputMemoryStream packet;
			if (connectedSocket.socket == socket)
			{
				
				for (auto& client : connectedSockets)
				{
					if (client.playerName == playerName)
					{
						packet << ServerMessage::Rejected;
						packet << "Username already taken!";
						sendPacket(packet, socket);
						onSocketDisconnected(socket);
						break;
					}
				}
			}
			packet << ServerMessage::Welcome;
			packet << "Welcome home buddy";

			sendPacket(packet, socket);

			for (auto& client : connectedSockets)
			{
				std::string user = " ";
				user += playerName += " joined ";
				SendMessageServer(user.c_str(), client.socket);
			}
		}
	}
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	// Remove the connected socket from the list
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			connectedSockets.erase(it);
			break;
		}
	}
}

