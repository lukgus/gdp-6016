// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

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

	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// IPV4
	hints.ai_socktype = SOCK_STREAM;	// Stream
	hints.ai_protocol = IPPROTO_TCP;	// TCP
	hints.ai_flags = AI_PASSIVE;

	printf("Creating our AddrInfo . . . ");
	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &info);
	if (result != 0) {
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Create our socket [Socket]
	printf("Creating our Listen Socket . . . ");
	SOCKET listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// 12,14,1,3:80				Address lengths can be different
	// 123,111,230,109:55555	Must specify the length
	// Bind our socket [Bind]
	printf("Calling Bind . . . ");
	result = bind(listenSocket, info->ai_addr, (int)info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// [Listen]
	printf("Calling Listen . . . ");
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// [Accept] <-- Blocking call!
	printf("Calling Accept . . . ");
	SOCKET clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("Accept failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Read/Write until the client disconnects..
	//   or the server disconnects the client

	// TODO:
	const int buflen = 128;
	char buf[buflen];
	printf("recv . . . ");

	// if successful, recv returns the number of bytes received
	result = recv(clientSocket, buf, buflen, 0);
	if (result == SOCKET_ERROR) {
		printf("recv failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		closesocket(listenSocket);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
		printf("Message From the client:\n%s\n", buf);
	}

	printf("send the message back . . . ");
	result = send(clientSocket, buf, result, 0);
	if (result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(info);
		closesocket(listenSocket);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
		printf("Sent %d bytes to the client!\n", result);
	}


	// Close
	printf("Closing . . . \n");
	freeaddrinfo(info);
	closesocket(listenSocket);
	closesocket(clientSocket);	// --> recv() tells the client the Server Disconnected
	WSACleanup();

	return 0;
}