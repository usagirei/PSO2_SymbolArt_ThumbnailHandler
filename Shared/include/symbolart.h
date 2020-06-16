#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#define _MASK(i) ((1 << i) - 1)
#define _OFFSET(x, i) (x >> i)
#define _BITFIELD(x, o, n) (_OFFSET(x, o) & _MASK(n))

struct SarHeader {
	inline uint32_t author() { return m_authorId; }
	inline int layers() { return _BITFIELD(m_Meta, 0, 9); }
	inline int width() { return _BITFIELD(m_Meta, 9, 8); }
	inline int height() { return _BITFIELD(m_Meta, 17, 7); }
	inline int soundEffect() { return _BITFIELD(m_Meta, 24, 8); }

private:
	uint32_t m_authorId;
	uint32_t m_Meta;
};

struct SarLayer {
private:
	static const uint8_t cLookup[];
	static const uint8_t aLookup[];
public:
	struct point {
		uint8_t x;
		uint8_t y;
	};

	inline uint8_t red()	{ return cLookup[_BITFIELD(m_v1, 0, 6)]; }
	inline uint8_t green()	{ return cLookup[_BITFIELD(m_v1, 6, 6)]; }
	inline uint8_t blue()	{ return cLookup[_BITFIELD(m_v1, 12, 6)]; }
	inline uint8_t alpha()	{ return aLookup[_BITFIELD(m_v1, 18, 3)]; }
	inline uint16_t shape() { return _BITFIELD(m_v1, 21, 10); }
	inline bool hidden() { return _BITFIELD(m_v1, 31, 1) == 1; }
	inline point& vertex(int n) { return m_Vertex[n]; }

	//private:
	point m_Vertex[4];
	uint32_t m_v1;
	uint32_t m_v2; // Editor HSV Positions??
};

class SarFile {
private:
	SarHeader m_Header;
	std::vector<SarLayer> m_Layers;
	std::u16string m_Name;
public:
	inline uint32_t authorId() { return m_Header.author(); }
	inline 	uint8_t width() { return m_Header.width(); }
	inline uint8_t height() { return m_Header.height(); }
	inline uint16_t soundEffect() { return m_Header.soundEffect(); }
	inline uint16_t numLayers() { return m_Header.layers(); }
	inline SarLayer& layer(uint16_t n) { return m_Layers[n]; }
	inline std::u16string name() { return m_Name; }

	SarFile(IStream* stream);
	~SarFile();
};

class sar_exception : public std::exception {
	char m_Msg[512];
public:
	sar_exception(const char* msg);
	inline const char* message() { return m_Msg; }
};