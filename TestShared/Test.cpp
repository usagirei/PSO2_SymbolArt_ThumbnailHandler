#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"

#include "symbolart.h"
#include "renderer.h"

#include "openglrenderer.h"
#include "debug.h"

HINSTANCE g_hInstDll;

int wmain(int argc, LPCWSTR* argv)
{
	IStream* stream;
	HRESULT hr = SHCreateStreamOnFile(argv[1], STGM_READ, &stream);

	if (!SUCCEEDED(hr)) {
		std::cout << "Error reading input";
		exit(1);
	}

	WCHAR dstName[MAX_PATH];
	wcscpy_s(dstName, argv[1]);

	SarFile sar(stream);
	stream->Release();

	OpenGLRenderer renderer;

	int zoom = 4;
	int sw = sar.width() * zoom;
	int sh = sar.height() * zoom;

	uint8_t* data = new uint8_t[sw * sh * 4];
	renderer.Render(sar, sw, sh, data);

	stbi_write_png("dump.png", sw, sh, 4, data, sw * 4);

	
}
