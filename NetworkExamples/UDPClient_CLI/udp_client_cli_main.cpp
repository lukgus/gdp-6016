// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <conio.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <string>

int g_Iteration = 0;

struct PlayerInfo {
	float x, y;
};

struct GameState {
	PlayerInfo player;
};

struct UserInput {
	bool W, A, S, D;
};

struct ServerInfo {
	SOCKET socket;
	sockaddr_in Addr;
	int addressSize;
};

ServerInfo g_ServerInfo;
UserInput g_UserInput;
GameState g_GameState;

bool g_UserSentInput;

void SendUpdateToGameServer()
{
	char input[4];
	input[0] = g_UserInput.W;
	input[1] = g_UserInput.A;
	input[2] = g_UserInput.S;
	input[3] = g_UserInput.D;
	int bufsize = sizeof(UserInput);

	printf("%d Input: %d %d %d %d => ", g_Iteration++, g_UserInput.W, g_UserInput.A, g_UserInput.S, g_UserInput.D);
	int sendResult = sendto(g_ServerInfo.socket, input, bufsize, 0, (SOCKADDR*)&g_ServerInfo.Addr, g_ServerInfo.addressSize);
	if (sendResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
		}

		return;
	}
}

void CheckForUpdateFromGameServer()
{
	const int sizeofGameState = sizeof(GameState);
	char buf[sizeofGameState];

	int recvResult = recvfrom(g_ServerInfo.socket, buf, sizeofGameState, 0, (SOCKADDR*)&g_ServerInfo.Addr, &g_ServerInfo.addressSize);
	if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
		}

		return;
	}

	memcpy(&g_GameState, (const void*)buf, sizeofGameState);

	printf(" { %.2f, %.2f }\n", g_GameState.player.x, g_GameState.player.y);

	int breakpoint = 0;
}

void NetworkUpdate()
{
	SendUpdateToGameServer();

	CheckForUpdateFromGameServer();
}

bool g_DoQuit = false;

void HandleUserInput()
{
	if (_kbhit())
	{
		char ch = _getch();
		if (ch == 27)	// ESCAPE
			g_DoQuit = true;

		if (ch == 'w' || ch == 'W')
			g_UserInput.W = true;
		if (ch == 'a' || ch == 'A')
			g_UserInput.A = true;
		if (ch == 's' || ch == 'S')
			g_UserInput.S = true;
		if (ch == 'd' || ch == 'D')
			g_UserInput.D = true;

		g_UserSentInput = g_UserInput.W || g_UserInput.A || g_UserInput.S || g_UserInput.D;
	}
}

void GameLoop()
{
	HandleUserInput();
	NetworkUpdate();

	// Clear user input for the next loop
	g_UserInput.W = 0;
	g_UserInput.A = 0;
	g_UserInput.S = 0;
	g_UserInput.D = 0;
}


int main(int argc, char** argv) {
	// Initialization 
	WSADATA wsaData;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	else {
		printf("WSAStartup: Success!\n");
	}

	// Create our UDP Socket
	g_ServerInfo.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (g_ServerInfo.socket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("socket: Success!\n");
	}

	g_ServerInfo.Addr.sin_family = AF_INET;
	g_ServerInfo.Addr.sin_port = htons(5555);
	g_ServerInfo.Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	g_ServerInfo.addressSize = sizeof(g_ServerInfo.Addr);

	DWORD NonBlock = 1;
	result = ioctlsocket(g_ServerInfo.socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("ioctlsocket: Success!\n");
	}

	g_UserSentInput = false;
	g_DoQuit = false;
	while (!g_DoQuit)
	{
		GameLoop();
	}

	printf("Closing . . . \n");
	closesocket(g_ServerInfo.socket);
	WSACleanup();

	return 0;
}
