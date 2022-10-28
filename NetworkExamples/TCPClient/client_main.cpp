// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "buffer.h"

#include <string>

MessageType type = MessageType::JoinRoom;
struct MessageHeader {
	int packet_length;
	MessageType message_type;
};

struct JoinRoomMessage : MessageHeader {
	int room_name_length;
	std::string room_name;
};

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

	JoinRoomMessage msg;
	msg.room_name = "Network";
	msg.room_name_length = msg.room_name.size();
	msg.message_type = MessageType::JoinRoom;

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

	// input output control socket
	DWORD NonBlock = 1;
	result = ioctlsocket(connectSocket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		printf("ioctlsocket to failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		freeaddrinfo(info);
		WSACleanup();
		return 1;
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

	bool tryAgain = true;

	while (tryAgain)
	{
		result = recv(connectSocket, recvBuf, recvBufLen, 0);
		// Expected result values:
		// 0 = closed connection, disconnection
		// >0 = number of bytes received
		// -1 = SOCKET_ERROR
		// 
		// 
		// NonBlocking recv, it will immediately return
		//		result will be -1 
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				printf("WouldBlock!\n");
				tryAgain = true;
			}
			else
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}
		}
		else {
			printf("Success!\n");
			printf("recv %d bytes from the server!\n", result);
			tryAgain = false;
		}
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
