#include <vector>
#include <iostream>

// unsigned int		// 4 byte size
// uint8_t			// 8 bits, 1 byte, unsigned

class Buffer {
public:

	void WriteUInt32LE(unsigned int value);
	void WriteUInt16LE(unsigned short value);
	void WriteUInt8LE(unsigned char value);
	void WriteInt32LE(int value);
	void WriteInt16LE(short value);
	void WriteInt8LE(char value);
	void WriteString(std::string data);

	unsigned int ReadUInt32LE();
	unsigned short ReadUInt16LE();
	unsigned char ReadUInt8LE();
	int ReadInt32LE();
	short ReadInt16LE();
	char ReadInt8LE();
	std::string ReadString();


	std::vector<uint8_t> data;
};

typedef int SOCKET;
struct ClientInfo {
	SOCKET socket;
	Buffer buffer;
};




// CLIENT
Buffer recvBuffer;
Buffer sendBuffer;

void ReadFromNetwork()
{
	// Write to receive buffer


}

void HandleUserInput()
{
	// Use your send buffer

	std::string message;

	// if hit enter
	// process message
}










void WriteUInt32LE(std::vector<uint8_t> &buffer, unsigned int value, int index) {
	buffer[index] = value;				// x x x O
	buffer[index + 1] = value >> 8;		// x x O x
	buffer[index + 2] = value >> 16;	// x O x x
	buffer[index + 3] = value >> 24;	// O x x x
}

void Test(long v) {
	printf("Int!\n");
}

void Test(long long v) {
	printf("Int!\n");
}

void Test(int v) {
	printf("Int!\n");
}

void Test(unsigned int v) {
	printf("Int!\n");
}

void Test(short v) {
	printf("Int!\n");
}

uint32_t ReadUInt32LE(std::vector<uint8_t>& buffer, int index) {
	//uint8_t v = 5;
	//uint8_t count = 3;
	//Test(v << count);

	//long long longV = 5;	//64 bits
	//Test(longV << 3);


	// Bitshift operator will cast to an int before shifting


	// value = 0 0 0 0
	uint32_t value = buffer[index];		// [244] 1 0 0
	// value = 0 0 0 [244]

	value |= buffer[index + 1] << 8;	// 244 [1] 0 0
	// value = 0 0 [1 244]

	value |= buffer[index + 2] << 16;	// 244 1 [0] 0
	// value = 0 [0 1 244]

	value |= buffer[index + 3] << 24;	// 244 1 0 [0]
	// value = [0 0 1 244]

	int result = 0;
	// |=			0 | 0 = 0
	// 00001101		0 | 1 = 1
	// 10101100		1 | 0 = 1
	// 10101101		1 | 1 = 1

	return value;
}

int main(int argc, char** argv) {
	int readIndex = 0;
	int writeIndex = 0;

	std::vector<uint8_t> buffer;
	buffer.resize(8);
	WriteUInt32LE(buffer, 50, 0);	// 00110010
	WriteUInt32LE(buffer, 500, 4);  // 00000001 11110100	

	int readValue50 = ReadUInt32LE(buffer, 0);
	int readValue500 = ReadUInt32LE(buffer, 4);

	printf("%d should be 50\n", readValue50);
	printf("%d should be 500\n", readValue500);

	uint8_t smallType = 50;
	int largerType = smallType;

	int breakpoint1 = 5;

	// buffer should look like this:
	// 00110010 00000000 00000000 00000000
	// 11110100 00000001 00000000 00000000

	// Write 50 to our buffer
	// 50 is 4 bytes (32 bits)
	// buffer.push_back(50);
	// 
	//					int type
	//					32 bits
	//  00000000 00000000 00000001 11110100	
	// 
	//				4x unsigned byte
	//     8bits	8bits	8bits	8bits	
	//  00000000 00000000 00000000 11110100
	unsigned short castShortExample = 500;
	uint8_t castExample = 500;


	//					value 500
	//					32 bits
	//  00000000 00000000 00000001 11110100	
	//					>> 8
	//  00000000 00000000 00000000 00000001   chopped off |11110100|	
	int value500 = 500;
	printf("%d\n", value500);
	printf("%d\n", value500 >> 8);

	//std::cout << value500 << "\n";
	//std::cout << (value500 >> 8) << "\n";


	// Read input from the user
	//int var;
	//std::cin >> var;

	return 0;
}
