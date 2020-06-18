#pragma once

#include "memorystream.h"
#include <cstring>

template<typename T>
inline T MemoryStream::read() {
	T val;
	read(&val, 1);
	return val;
}

template<typename T>
inline size_t MemoryStream::read(T* val, int count) {
	size_t blksz = sizeof(T) * count;
	size_t bytesLeft = (m_Buf.size() - m_ReadCursor);
	bytesLeft = (bytesLeft / sizeof(T)) * sizeof(T);
	size_t bytesRead = __min(blksz, bytesLeft);

	memcpy_s(val, bytesRead, rcursor(), bytesRead);
	rseek(bytesRead, SEEK_CUR);

	return bytesRead / sizeof(T);
}

template<typename T>
inline size_t MemoryStream::write(T value) {
	return write(&value, 1);
}

template<typename T>
inline size_t MemoryStream::write(T* value, int count)
{
	size_t blksz = sizeof(T) * count;
	size_t bytesLeft = m_Buf.size() - m_WriteCursor;
	if (blksz > bytesLeft) {
		wreserve(blksz);
		bytesLeft = m_Buf.size() - m_WriteCursor;
	}
	memcpy_s(wcursor(), bytesLeft, value, blksz);
	wseek(blksz, SEEK_CUR);
	return blksz;
}


