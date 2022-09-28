// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

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

	struct addrinfo* info = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4 #.#.#.#, AF_INET6 is IPv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	printf("Creating our AddrInfo . . . ");
	result = getaddrinfo("127.0.0.1", "5555", &hints, &info);
	if (result != 0) {
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Create our connection [Socket] 
	SOCKET connectSocket;
	printf("Creating our Connection Socket . . . ");
	connectSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (connectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}
	
	// [Connect] to the server
	printf("Connect to the server . . . ");
	result = connect(connectSocket, info->ai_addr, (int)info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("Failed to connect to the server with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// This will wait for the user to press any button to continue
	system("Pause");

	// TODO: read/write send/recv
	const int recvBufLen = 128;
	char recvBuf[recvBufLen];

	const char* buf = "Hello!";
	int buflen = 6;

	// [send]
	printf("Sending message to the server . . . ");
	result = send(connectSocket, buf, buflen, 0);
	if (result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
		printf("Sent %d bytes to the server!\n", result);
	}

	printf("recieve a message from the server . . . ");
	result = recv(connectSocket, recvBuf, recvBufLen, 0);
	if (result == SOCKET_ERROR) {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
		printf("recv %d bytes from the server!\n", result);
	}

	// Close
	printf("Shutdown . . . ");
	result = shutdown(connectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	printf("Closing . . . \n");
	closesocket(connectSocket);
	WSACleanup();

	return 0;
}
