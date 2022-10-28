#include <iostream>
#include <string>

#include <conio.h>

void NetworkUpdate() {
	// ioctlsocket (Set to nonblocking)
	// recv, and handle..
}

void SendMessageToServer(std::string& msg) {
	std::cout << ">> [header][message]: " << msg << '\n';
	msg.clear();

	msg = "/connect 127.0.0.1 5555";

	// parse the message into our packet data
}

std::string message;
bool quit;

void HandleUserInput() {
	if (_kbhit()) {
		int key = _getch();
		if (key == 13) {
			// Enter! 
			SendMessageToServer(message);
		}
		else if (key == 27) {
			quit = true;
		}
		else if (key == 8) {
			message.pop_back();
			std::cout << message << '\n';
		}
		else
		{
			message += (char)key;
			std::cout << message << '\n';
		}
	}
}

int main(int argc, char** argv) {
	quit = false;
	while(!quit) {
		HandleUserInput();
		NetworkUpdate();
	}

	return 1;
}
