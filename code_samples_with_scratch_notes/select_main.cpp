// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "5555"

#include <vector>
class Buffer {
public:
	std::vector<uint8_t> Data;
};

struct ClientInformation {
	SOCKET socket;
	bool connected;

	// Maybe a struct as Packet
	Buffer buffer;
	unsigned int packetLength;
};

struct ServerInfo {
	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	SOCKET listenSocket = INVALID_SOCKET;
	fd_set activeSockets;
	fd_set socketsReadyForReading;
	std::vector<ClientInformation> clients;
} g_ServerInfo;


int Initialize() {	// Initialization 
	WSADATA wsaData;
	int result;

	FD_ZERO(&g_ServerInfo.activeSockets);
	FD_ZERO(&g_ServerInfo.socketsReadyForReading);

	printf("WSAStartup . . . ");
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	else {
		printf("Success!\n");
	}

	ZeroMemory(&g_ServerInfo.hints, sizeof(g_ServerInfo.hints));
	g_ServerInfo.hints.ai_family = AF_INET;			// IPV4
	g_ServerInfo.hints.ai_socktype = SOCK_STREAM;	// Stream
	g_ServerInfo.hints.ai_protocol = IPPROTO_TCP;	// TCP
	g_ServerInfo.hints.ai_flags = AI_PASSIVE;

	printf("Creating our AddrInfo . . . ");
	result = getaddrinfo(NULL, DEFAULT_PORT, &g_ServerInfo.hints, &g_ServerInfo.info);
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
	g_ServerInfo.listenSocket = socket(g_ServerInfo.info->ai_family, 
		g_ServerInfo.info->ai_socktype, g_ServerInfo.info->ai_protocol);
	if (g_ServerInfo.listenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_ServerInfo.info);
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
	result = bind(g_ServerInfo.listenSocket, 
		g_ServerInfo.info->ai_addr, (int)g_ServerInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// [Listen]
	printf("Calling Listen . . . ");
	result = listen(g_ServerInfo.listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	return 0;
}

void ShutDown() {
	// Close
	printf("Closing . . . \n");
	freeaddrinfo(g_ServerInfo.info);
	closesocket(g_ServerInfo.listenSocket);
	WSACleanup();
}

int main(int argc, char** argv)
{
	int result = Initialize();
	if (result != 0) {
		return result;
	}

	// Do our communication

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500 * 1000; // 500 milliseconds, half a second

	int selectResult;

	// select work here
	for (;;) {
		// SocketsReadyForReading will be empty here
		FD_ZERO(&g_ServerInfo.socketsReadyForReading);

		// Add all the sockets that have data ready to be recv'd 
		// to the socketsReadyForReading

		// 1. Add the listen socket, to see if there are any new connections
		FD_SET(g_ServerInfo.listenSocket, &g_ServerInfo.socketsReadyForReading);

		// 2. Add all the connected sockets, to see if the is any information
		//    to be recieved from the connected clients.
		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			ClientInformation& client = g_ServerInfo.clients[i];
			if (client.connected) {
				FD_SET(client.socket, &g_ServerInfo.socketsReadyForReading);
			}
		}

		// socketsReadyForReading contains:
		//  ListenSocket
		//  All connectedClients' socket

		// Client can call:
		//  - connect
		//  - closesocket	- recv returns 0 
		//  - send
		//  - WSAsend

		// socketsReadyForReading fd_set
		// ListenSocket
		// ConnectedClients

		// Select will check all sockets in the SocketsReadyForReading set
		// to see if there is any data to be read on the socket.
		selectResult = select(0, &g_ServerInfo.socketsReadyForReading, NULL, NULL, &tv);
		if (selectResult == SOCKET_ERROR) {
			printf("select() failed with error: %d\n", WSAGetLastError());
			return 1;
		}
		printf(".");

		// Check if our ListenSocket is set. This checks if there is a 
		// new client trying to connect to the server using a "connect" 
		// function call.
		if (FD_ISSET(g_ServerInfo.listenSocket, &g_ServerInfo.socketsReadyForReading)) {
			printf("\n");

			// [Accept]
			printf("Calling Accept . . . ");
			SOCKET clientSocket = accept(g_ServerInfo.listenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET) {
				printf("Accept failed with error: %d\n", WSAGetLastError());
			}
			else {
				printf("Success!\n");
				ClientInformation newClient;
				newClient.socket = clientSocket;
				newClient.connected = true;
				g_ServerInfo.clients.push_back(newClient);
			}


		}

		// { 1, 2, 3, 4, 5, 6 }

		// 1, 2, 4, 5, 6
		// 1, 2, 5, 6

		// iterate through the vector BACKWARDS


		// Check if any of the currently connected clients have sent
		// data using send
		//for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
		for (int i = g_ServerInfo.clients.size() - 1; i >= 0; i--) {
			ClientInformation& client = g_ServerInfo.clients[i];
			if (client.connected == false)
				continue;

			if (FD_ISSET(client.socket, &g_ServerInfo.socketsReadyForReading)) {

				// Act as a ping server 
				//const int buflen = 128;
				//char buf[buflen];

				//int recvResult = recv(client.socket, buf, buflen, 0);

				// Deserialize messages properly

				// Must wait for 4 bytes
				// Packet sent:
				//buffer.writeUint32LE(pkt.packetLength);			// 32
				//buffer.writeUint32LE(pkt.messageId);				// 3
				//buffer.writeUint32LE(pkt.roomName.size());		// 10
				//buffer.writeString(pkt.roomName);					// "networking"
				//buffer.writeUint32LE(pkt.message.size());			// 6
				//buffer.writeString(pkt.message);					// "Hello!"


				int dataStartIndex = client.buffer.Data.size();
				// if we recieved 3 bytes already
				// [0][1][2]  [3] <-- start entering data at index 3
				char* bufPtr = (char*)&(client.buffer.Data[dataStartIndex]);


				int recvResult = recv(client.socket, bufPtr, 128, 0);
				if (recvResult == SOCKET_ERROR) {
				}

				// 1010011101010010100111010111010101010101010101011111101010111
				// 00111010100010100111010111010101010101010101011111101010111
				// 0100111010100010111010111010101010101010101011111101010111
				// 00111010100010100111010111010101010101010101011111101010111
				// 10011010100010100111010111010101010101010101011111101010111
				// 100111010100010100111010111010101010101010101011111101010111



				// 10100111010100010100111010111010101010101010101011111101010111

				//					 WAIT
				// 32 3 10 Network | RECV | ing 6 Hello! |RECV| (Handle your message)

				// recv 3 bytes
				//		Not enough bytes to determine the packet length
				//		wait for the next recv
				// recv 20 bytes
				//		We have the packet length, but we don't have 
				//		The whole packet.
				// recv 9 bytes
				//		We have the full packet, we can deserialize it
				//		and process it

				if (recvResult > 0) {


					if (client.buffer.Data.size() >= 4) {
						client.packetLength = buffer.readUInt32LE();
					}

					if (client.packetLength != 0 &&
						client.packetLength == client.buffer.Data.size())
					{
						// We have all of our data! We can deserialize this message
						// Get the message type (From our protocol)
						unsigned int messageType = client.buffer.ReadUInt32LE();

						ProcessMessage(messageType, client.buffer);
						if (messageType == sendMessageToRoom)
							int roomNameLength = buffer.ReadUInt32LE();
							string roomName = buffer.ReadString(roomNameLength);

							int messageLength = buffer.ReadUInt32LE();
							string roomNameLength = buffer.ReadString(messageLength);
					}

					continue;
				}


				// Saving some time to not modify a vector while 
				// iterating through it. Want remove the client
				// from the vector
				if (recvResult == 0) {
					printf("Client disconnected!\n");
					client.connected = false;
					continue;
				}

				printf("Message From the client:\n%s\n", buf);
				// Check if successful.. 
				// Check for errors..

				int sendResult = send(client.socket, buf, recvResult, 0);
				// Check if successful.. 
				// Check for errors..
			}
		}
	}


	ShutDown();
	return 0;
}