#include "psr.h"

void psr_decompress(MemoryStream& dst, MemoryStream& src) {

	while (true) {

		while (src.readBit())
			dst.write(src.read<uint8_t>());

		int offset = 0;
		int size = 0;

		if (src.readBit()) {
			int v = src.read<uint16_t>();
			if (v == 0)
				break; // EOF

			offset = (v >> 3) - 8192;
			size = v & 7;

			if (size != 0) {
				size += 2;
			}
			else {
				size = src.read<uint8_t>() + 10;
			}
		}
		else {
			int a = src.readBit() ? 1 : 0;
			int b = src.readBit() ? 1 : 0;
			size = (b | (a << 1)) + 2;

			offset = src.read<uint8_t>() - 256;
		}

		for (int i = 0; i < size; i++) {
			uint8_t b = dst.wpeek(offset);
			dst.write(b);
		}

	}

}


