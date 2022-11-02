// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <string>

int main(int argc, char** argv) {
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

	// Create our UDP Socket
	SOCKET serverSocket;
	printf("Creating our Connection Socket . . . ");
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	struct sockaddr_in serverAddr;

	const int recvBufLen = 128;
	char recvBuf[recvBufLen];

	const char* buf = "Ping";
	int buflen = 4;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// [send]
	for (int i = 0; i < 3; i++)
	{

		printf("Sending message to the server . . . ");
		result = sendto(serverSocket, buf, buflen, 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (result == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		else {
			printf("Success!\n");
			printf("Sent %d bytes to the server!\n", result);
		}

		result = recvfrom(serverSocket, recvBuf, recvBufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
		if (result == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		else {
			printf("Success!\n");
			printf("recv %d bytes from the server: %s!\n", result, recvBuf);
		}
	}
	printf("Closing . . . \n");
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
