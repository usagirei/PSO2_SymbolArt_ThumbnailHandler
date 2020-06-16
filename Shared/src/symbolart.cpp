#include "symbolart.h"
#include "blowfish.h"
#include "memorystream.h"
#include "psr.h"
#include "debug.h"

SarFile::SarFile(IStream* stream)
{
	STATSTG stat;
	stream->Stat(&stat, STATFLAG_NONAME);

	ULONG64 file_size = stat.cbSize.QuadPart;
	ULONG read;

	uint8_t k[] = { 9, 7, 193, 43 };
	crypto_blowfish::blowfish b(k, 4);

	char sar_magic[3];
	stream->Read(sar_magic, 3, &read);
	if (strncmp((char*)sar_magic, "sar", 3))
		throw sar_exception("Not a SAR File");

	uint8_t sar_flag;
	stream->Read(&sar_flag, 1, &read);

	MemoryStream sar(file_size);
	sar.write<char>(sar_magic, 3);
	sar.write<uint8_t>(sar_flag);

	int toRead = file_size - 4;
	sar.wreserve(toRead);
	stream->Read(sar.wcursor(), toRead, &read);
	sar.wseek(read, SEEK_CUR);

	DEBUG_HEXDUMP_EN(*sar.data(), __min(256, file_size));

	int toDecrypt = (toRead / 8) * 8;
	b.decrypt(sar.data() + 4, sar.data() + 4, toDecrypt);

	DEBUG_HEXDUMP_EN(*sar.data(), __min(256, file_size));

	bool compressed = sar_flag & 0x80;
	if (compressed) {

		for (int i = 4; i < sar.size(); i++) {
			sar[i] = sar[i] ^ 0x95;
		}

		MemoryStream dec(file_size);
		dec.wseek(0, SEEK_SET);
		dec.write<char>(sar_magic, 3);
		dec.write<uint8_t>(sar_flag ^ 0x80);

		sar.rseek(4, SEEK_SET);
		psr_decompress(dec, sar);
		sar = dec;
	}

	DEBUG_HEXDUMP_EN(*sar.data(), __min(256, file_size));

	sar.rseek(4, SEEK_SET);
	sar.read<SarHeader>(&m_Header, 1);

	DEBUG_SCOPE({
		FILE * fp;
		fopen_s(&fp, "dump.bin", "wb");
		fwrite(sar, 1, sar.size(), fp);
		fclose(fp);
		});

	DEBUG_HEXDUMP_E(m_Header);
	DEBUG_BINDUMP_E(m_Header);
	DEBUG_PRINT_E(m_Header.author());
	DEBUG_PRINT_E(m_Header.layers());
	DEBUG_PRINT_E(m_Header.width());
	DEBUG_PRINT_E(m_Header.height());
	DEBUG_PRINT_E(m_Header.soundEffect());

	m_Layers.resize(m_Header.layers());
	sar.read<SarLayer>(m_Layers.data(), m_Header.layers());

	DEBUG_SCOPE({
		for (int i = 0; i < m_Header.layers(); i++) {
			SarLayer& layer = m_Layers[i];

			DEBUG_PRINT("--");
			DEBUG_HEXDUMP_E(layer);
			DEBUG_BINDUMP_E(m_Layers[i]);
			DEBUG_PRINTF("Color: %d %d %d %d", layer.red(), layer.green(), layer.blue(), layer.alpha());
			DEBUG_PRINTF("Shape: %d", layer.shape());
			for (int i = 0; i < 4; i++) {
				DEBUG_PRINTF("Vertex %d: %d %d", i, signed(layer.vertex(i).x - 128), signed(layer.vertex(i).y - 128));
			}
		}
	});

	int nameLen = (sar.size() - sar.rtell()) / 2;

	std::u16string u16_str(nameLen + 1, '\0');
	sar.readBytes(&u16_str[0], nameLen * 2);
	m_Name = u16_str;
}

SarFile::~SarFile()
{
}

sar_exception::sar_exception(const char* msg)
{
	strcpy_s(m_Msg, msg);
}

const uint8_t SarLayer::cLookup[] = {
	0x00, 0x01, 0x03, 0x04, 0x05, 0x07, 0x08, 0x0A,
	0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1B,
	0x1D, 0x20, 0x23, 0x26, 0x29, 0x2C, 0x2F, 0x32,
	0x35, 0x38, 0x3C, 0x3F, 0x43, 0x47, 0x4B, 0x4F,
	0x53, 0x57, 0x5B, 0x5F, 0x64, 0x68, 0x6D, 0x72,
	0x76, 0x7B, 0x80, 0x85, 0x8A, 0x90, 0x95, 0x9B,
	0xA0, 0xA6, 0xAB, 0xB1, 0xB7, 0xBD, 0xC3, 0xCA,
	0xD0, 0xD6, 0xDD, 0xE3, 0xEA, 0xF1, 0xF8, 0xFF
};

const uint8_t SarLayer::aLookup[] = {
	0x0A, 0x1B, 0x32, 0x4F, 0x72, 0x9B, 0xCA, 0xFF
};
