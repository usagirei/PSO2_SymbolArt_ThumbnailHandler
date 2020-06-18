#pragma once

#include <cstdint>
#include <vector>
#include <memory>

class MemoryStream {
	std::vector<uint8_t> m_Buf;
	int m_BitsLeft;
	int m_ReadCursor;
	int m_WriteCursor;
	uint8_t m_CurBit;

public:
	MemoryStream(size_t initialSize);
	MemoryStream(uint8_t* data, size_t dataSize);

	uint8_t& operator[](size_t pos);
	explicit operator uint8_t* ();

	uint8_t* data();
	size_t size();
	void seek(int n, int mode);

	// Reading

	uint8_t* rcursor();
	size_t rtell();
	uint8_t rpeek(int offset);
	void rseek(int n, int mode);

	bool readBit();

	template<typename T>
	T read();
	template<typename T>
	size_t read(T* val, int count);

	// Writing

	uint8_t* wcursor();
	size_t wtell();
	void wseek(int n, int mode);
	uint8_t wpeek(int offset);

	template<typename T>
	size_t write(T value);
	template<typename T>
	size_t write(T* value, int count);

	size_t wreserve(size_t amount);
};

#include "memorystream.inl"
