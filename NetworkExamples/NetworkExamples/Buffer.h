#pragma once

#include <vector>

class Buffer {
public:
	Buffer(size_t size);

	void WriteInt32LE(std::size_t index, int32_t value);
	void WriteInt32LE(int32_t value);
	/*
	 * [m_WriteIndex++] <-- uses the value before incrementing it
	 * 
	 * [++m_WriteIndex] <-- Increments the value before using it
	 * m_WriteIndex += 4;
	 **/

	uint32_t ReadUInt32LE(std::size_t index);
	uint32_t ReadUInt32LE();

private:
	// call m_Buffer.resize in your constructor
	std::vector<uint8_t> m_Buffer;

	// initialize these at 0
	int m_WriteIndex;
	int m_ReadIndex;
};
