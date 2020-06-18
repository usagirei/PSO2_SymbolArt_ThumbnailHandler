#include "memorystream.h"

MemoryStream::MemoryStream(size_t len)
	: m_Buf(len), m_BitsLeft(0), m_ReadCursor(0), m_WriteCursor(0), m_CurBit(0) {

}

MemoryStream::MemoryStream(uint8_t* data, size_t len)
	: m_Buf(data, data + len), m_BitsLeft(0), m_ReadCursor(0), m_WriteCursor(len), m_CurBit(0) {

}

uint8_t& MemoryStream::operator[](size_t pos) {
	return m_Buf[pos];
}

uint8_t* MemoryStream::data()
{
	return &m_Buf[0];
}

uint8_t MemoryStream::rpeek(int offset) {
	return m_Buf[m_ReadCursor + offset];
}

MemoryStream::operator uint8_t* () {
	return &m_Buf[0];
}

size_t MemoryStream::size() {
	return m_Buf.size();
}

void MemoryStream::seek(int n, int mode)
{
	rseek(n, mode);
	wseek(n, mode);
}

bool MemoryStream::readBit() {
	if (m_BitsLeft == 0) {
		m_CurBit = read<uint8_t>();
		m_BitsLeft = 8;
	}

	bool set = (m_CurBit & 1);
	m_CurBit >>= 1;
	--m_BitsLeft;

	return set;
}

void MemoryStream::rseek(int n, int mode) {
	switch (mode)
	{
	case SEEK_CUR:
		m_ReadCursor += n;
		return;
	case SEEK_SET:
		m_ReadCursor = n;
		return;
	case SEEK_END:
		m_ReadCursor = m_Buf.size() + n;
		return;
	}

}

uint8_t* MemoryStream::rcursor()
{
	return &m_Buf[m_ReadCursor];
}

size_t MemoryStream::rtell()
{
	return m_ReadCursor;
}

uint8_t* MemoryStream::wcursor()
{
	return &m_Buf[m_WriteCursor];
}

size_t MemoryStream::wtell()
{
	return m_WriteCursor;
}

void MemoryStream::wseek(int n, int mode)
{
	switch (mode)
	{
	case SEEK_CUR:
		m_WriteCursor += n;
		return;
	case SEEK_SET:
		m_WriteCursor = n;
		return;
	case SEEK_END:
		m_WriteCursor = m_Buf.size() + n;
		return;
	}
}

uint8_t MemoryStream::wpeek(int offset)
{
	return m_Buf[m_WriteCursor + offset];
}

size_t MemoryStream::wreserve(size_t amount) {
	size_t curSz = m_Buf.size();
	size_t bytesNeeded = m_WriteCursor + amount;
	if (curSz < bytesNeeded) {
		m_Buf.resize(bytesNeeded);
	}
	return m_Buf.size();
}
