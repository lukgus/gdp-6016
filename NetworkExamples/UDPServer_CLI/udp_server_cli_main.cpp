// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "5555"

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
	SOCKET listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("socket: Success!\n");
	}

	struct sockaddr_in recvAddr;

	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(5555);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(listenSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
	if (result != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("bind: Success!\n");
	}

	const int buflen = 128;
	char buf[buflen];

	// if successful, recv returns the number of bytes received
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	int count = 0;
	while (count < 5)
	{
		count++;

		printf("recv . . . ");
		result = recvfrom(listenSocket, buf, buflen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
		if (result == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else {
			printf("Success!\n");
			printf("Message From the client:\n%s\n", buf);

			char ipAddress[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(SenderAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);

			printf(" Client Info: %s : %d\n", ipAddress, SenderAddr.sin_port);
		}

		std::string msg("Pong!");
		result = sendto(listenSocket, msg.c_str(), 5, 0, (SOCKADDR*) & SenderAddr, SenderAddrSize);
		if (result == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else {
			printf("Success!\n");
			printf("Message sent to the client:\n%s\n", msg.c_str());
		}
	}


	// Close
	printf("Closing . . . \n");
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}