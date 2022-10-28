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

#include <string>

enum MessageType {
	JoinRoom = 1,
	LeaveRoom = 2,
	SendMessageToRoom = 3
};

struct PacketHeader {
	int packetLength;
	int messageId;
};

struct SendMessagePacket : public PacketHeader {
	std::string roomName;
	std::string message;
};
struct JoinRoomPacket : public PacketHeader {
	std::string roomName;
};
struct LeaveRoomPacket : public PacketHeader {
	std::string roomName;
};

#include <vector>
#include <map>
#include <string>
using namespace std;
struct Client {
	SOCKET socket;
};

map<string, vector<Client>> Rooms;

int main(int argc, char** argv)
{
	// Preparing a packet
	SendMessagePacket pkt;
	pkt.roomName = "networking";
	pkt.message = "Hello!";
	pkt.messageId = SendMessageToRoom;
	pkt.packetLength = 
		sizeof(PacketHeader) +
		sizeof(pkt.roomName.size()) + pkt.roomName.size() + 
		sizeof(pkt.roomName.size()) + pkt.message.size();

	// Write to our buffer
	buffer.writeUint32LE(pkt.packetLength);				// 32
	buffer.writeUint32LE(pkt.messageId);				// 3
	buffer.writeUint32LE(pkt.roomName.size());			// 10
	buffer.writeString(pkt.roomName);					// "networking"
	buffer.writeUint32LE(pkt.message.size());			// 6
	buffer.writeString(pkt.message);					// "Hello!"

	// Send our buffer over the network
	std::vector<uint8_t> bufferData; // 32 3 10 "networking" 6 "Hello!"
	SOCKET serverSocket;
	int sendResult = send(serverSocket, (const char*)&(bufferData[0]), pkt.header.packetLength, 0);


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