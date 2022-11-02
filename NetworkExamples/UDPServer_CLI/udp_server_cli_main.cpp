// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <conio.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "5555"

struct PlayerInfo {
	float x, y;
};

struct GameState {
	PlayerInfo player;
};

struct ServerInfo {
	SOCKET socket;
};

struct ClientInfo {
	bool HaveInfo;
	sockaddr_in clientAddr;
	int clientAddrSize;
};

struct UserInput {
	bool W, A, S, D;
};

int g_Iteration;
GameState g_GameState;
ClientInfo g_ClientInfo;
UserInput g_UserInput;
ServerInfo g_ServerInfo;
bool g_doQuit = false;

void GetInfoFromClients()
{
	const int bufsize = sizeof(UserInput);
	char buf[bufsize];

	int recvResult = recvfrom(g_ServerInfo.socket, buf, bufsize, 0, (SOCKADDR*)&g_ClientInfo.clientAddr, &g_ClientInfo.clientAddrSize);
	if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
			g_doQuit = true;
		}

		return;
	}

	g_ClientInfo.HaveInfo = true;
	memcpy(&g_UserInput, (const void*)buf, bufsize);

	printf("%d Input: %d %d %d %d => ", g_Iteration++, g_UserInput.W, g_UserInput.A, g_UserInput.S, g_UserInput.D);
}

void UpdateClientsWithGameState()
{
	if (g_ClientInfo.HaveInfo == false)
		return;

	int gameStateSize = sizeof(GameState);
	printf(" { %.2f, %.2f }\n", g_GameState.player.x, g_GameState.player.y);
	int sendResult = sendto(g_ServerInfo.socket, (const char*)&g_GameState, gameStateSize, 0, (SOCKADDR*)&g_ClientInfo.clientAddr, g_ClientInfo.clientAddrSize);
	if (sendResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
			g_doQuit = true;
		}

		return;
	}
}

void GameUpdate()
{
	// Update the game state
	if (g_UserInput.A) {
		g_GameState.player.x--;
	}
	if (g_UserInput.D) {
		g_GameState.player.x++;
	}
	if (g_UserInput.W) {
		g_GameState.player.y++;
	}
	if (g_UserInput.S) {
		g_GameState.player.y--;
	}
}

void GameLoop()
{
	GetInfoFromClients();

	GameUpdate();

	UpdateClientsWithGameState();
}

void HandleUserInput()
{
	if (_kbhit())
	{
		int ch = _getch();

		if (ch == 27)
		{
			g_doQuit = true;
		}
	}
}

int main(int argc, char** argv)
{
	// Initialization 
	WSADATA wsaData;
	int result;

	printf("WSAStartup . . . ");
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Create our socket [Socket]
	printf("Creating our Listen Socket . . . ");
	g_ServerInfo.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (g_ServerInfo.socket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("socket: Success!\n");
	}

	DWORD NonBlock = 1;
	result = ioctlsocket(g_ServerInfo.socket, FIONBIO, &NonBlock);
	if (result != 0) {
		wprintf(L"ioctlsocket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("ioctlsocket: Success!\n");
	}

	struct sockaddr_in recvAddr;
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(5555);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(g_ServerInfo.socket, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
	if (result != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("bind: Success!\n");
	}

	g_ClientInfo.clientAddrSize = sizeof(g_ClientInfo.clientAddr);
	g_ClientInfo.HaveInfo = false;

	while (!g_doQuit)
	{
		HandleUserInput();
		GameLoop();
	}

	// Close
	printf("Closing . . . \n");
	closesocket(g_ServerInfo.socket);
	WSACleanup();

	return 0;
}