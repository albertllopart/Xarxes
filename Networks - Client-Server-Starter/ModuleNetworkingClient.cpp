#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		ELOG("socket()");
	}
	LOG("socket done");
	// - Create the remote address object
	struct sockaddr_in serverAddr;
	const int serverAddrLen = sizeof(serverAddr);
	serverAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, serverAddressStr, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(serverPort); // Port

	// - Connect to the remote address
	int connectRes = connect(s, (const sockaddr*)&serverAddr, serverAddrLen);
	if (connectRes == SOCKET_ERROR) {
		ELOG("connect");
	}
	LOG("connect done");
	// - Add the created socket to the managed list of sockets using addSocket()
	addSocket(s);
	// If everything was ok... change the state
	state = ClientState::Start;
	LOG("everything ok, changing state...");

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	if (state == ClientState::Start)
	{
		// TODO(jesus): Send the player name to the server
		send(s, playerName.c_str(), (int)playerName.size() + 1, 0);
		LOG("Sent %s to the server", playerName.c_str());
	}

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", playerName.c_str());

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, byte * data)
{
	state = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

