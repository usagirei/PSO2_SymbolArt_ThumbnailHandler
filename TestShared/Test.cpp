#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <memory>

#include "stb_image.h"
#include "stb_image_write.h"

#include "symbolart.h"
#include "renderer.h"

#include "openglrenderer.h"
#include "debug.h"

HINSTANCE g_hInstDll;


int fileExists(LPCWSTR file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFileW(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		//FindClose(&handle); this will crash
		FindClose(handle);
	}
	return found;
}

struct ComDeleter {
	void operator()(IUnknown* unk) {
		unk->Release();
	}
};

template<typename T>
using com_ptr = std::unique_ptr<T, ComDeleter>;

int wmain(int argc, LPCWSTR* argv)
{
	WCHAR dstName[MAX_PATH];
	OpenGLRenderer renderer;
	std::vector<uint32_t> buf;
	int targetSize = 0;

	for (int i = 1; i < argc; i++) {

		LPCWSTR f = argv[i];
		int newSize;
		if (fileExists(f)) {

			wcscpy_s(dstName, f);
			LPWSTR dot = wcsrchr(dstName, L'.');
			*dot = '\0';
			wcscat_s(dstName, L".png");

			IStream* tmp;
			HRESULT hr = SHCreateStreamOnFileW(argv[1], STGM_READ, &tmp);
			if (!SUCCEEDED(hr)) {
				std::cout << "Failed to Open SAR File";
				continue;
			}

			com_ptr<IStream> stream(tmp);

			std::unique_ptr<SarFile> sar;
			try {
				sar = std::make_unique<SarFile>(stream.get());
			}
			catch (sar_exception& e) {
				std::cout << e.what() << std::endl;
				continue;
			}

			int sw = sar->width();
			int sh = sar->height();
			if (targetSize) {
				int maxDim = __max(sw, sh);
				sw = sw * targetSize / maxDim;
				sh = sh * targetSize / maxDim;
			}

			int numPixels = sw * sh;
			buf.resize(numPixels);
			renderer.Render(*sar, sw, sh, &buf[0]);

			for (auto p = buf.begin(); p < buf.end(); p++) {
				uint32_t pixel = *p;
				pixel = _byteswap_ulong(pixel);
				pixel = (pixel >> 8) | (pixel << 24);
				*p = pixel;
			}

			FILE* outFile;
			_wfopen_s(&outFile, dstName, L"wb");
			stbi_write_func* writeFunc = [](void* ctx, void* data, int size) { fwrite(data, 1, size, static_cast<FILE*>(ctx)); };
			stbi_write_png_to_func(writeFunc, outFile, sw, sh, 4, &buf[0], sw * 4);
			fclose(outFile);

		}
		else if ((newSize = wcstol(f, nullptr, 10)) != 0) {
			targetSize = newSize < 0 ? 0 : newSize;
		}
	}
}
