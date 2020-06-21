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
	std::unique_ptr<OpenGLRenderer> renderer;
	try {
		renderer = std::make_unique<OpenGLRenderer>();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}
	std::vector<uint32_t> buf;
	int targetSize = 0;
	bool renderHd = false;

	for (int i = 1; i < argc; i++) {

		LPCWSTR f = argv[i];
		int newSize;
		if (fileExists(f)) {


			std::unique_ptr<SarFile> sar;
			try {
				IStream* tmp;
				HRESULT hr = SHCreateStreamOnFileW(f, STGM_READ, &tmp);
				if (!SUCCEEDED(hr)) {
					std::cout << "Failed to Open SAR File";
					continue;
				}

				com_ptr<IStream> stream(tmp);
				sar = std::make_unique<SarFile>(stream.get());
			}
			catch (sar_exception& e) {
				std::cout << e.what() << std::endl;
				continue;
			}

			DEBUG_SCOPE({
				wcscpy_s(dstName, f);
				LPWSTR dot = wcsrchr(dstName, L'.');
				*dot = '\0';
				wcscat_s(dstName, L".txt");

				FILE * outFile;
				_wfopen_s(&outFile, dstName, L"wb");

				for (int i = 0; i < sar->numLayers(); i++) {
					SarLayer& sl = sar->layer(i);
					fprintf_s(outFile, "Shape: ", sl.shape());
					uint16_t shape = sl.shape();
					for (int i = 0; i < 10; i++) {
						bool set = shape & (1 << (10 - 1 - i));
						fprintf_s(outFile, "%d", set ? 1 : 0);
						if (i == 1 || i == 5)
							fprintf_s(outFile, "_");
					}
					fprintf_s(outFile, " %04d\n", sl.shape());
				}
				fclose(outFile);
			});


			int sw = sar->width();
			int sh = sar->height();
			if (targetSize != 0) {
				int maxDim = __max(sw, sh);
				sw = sw * targetSize / maxDim;
				sh = sh * targetSize / maxDim;
			}

			int numPixels = sw * sh;
			buf.resize(numPixels);
			renderer->SetFlag(OpenGLRenderer::FLAG_HD, renderHd);
			renderer->Render(*sar, sw, sh, &buf[0]);

			for (auto p = buf.begin(); p < buf.end(); p++) {
				uint32_t pixel = *p;
				pixel = _byteswap_ulong(pixel);
				pixel = (pixel >> 8) | (pixel << 24);
				*p = pixel;
			}

			wcscpy_s(dstName, f);
			LPWSTR dot = wcsrchr(dstName, L'.');
			*dot = '\0';

			if (targetSize) {
				WCHAR strbuf[64];
				swprintf_s(strbuf, L"_%d", targetSize);
				wcscat_s(dstName, strbuf);
			}

			if(renderHd) {
				wcscat_s(dstName, L"_hd");
			}

			wcscat_s(dstName, L".png");

			FILE* outFile;
			_wfopen_s(&outFile, dstName, L"wb");
			stbi_write_func* writeFunc = [](void* ctx, void* data, int size) { fwrite(data, 1, size, static_cast<FILE*>(ctx)); };
			stbi_write_png_to_func(writeFunc, outFile, sw, sh, 4, &buf[0], sw * 4);
			fclose(outFile);

		}
		else if ((newSize = wcstol(f, nullptr, 10)) != 0) {
			targetSize = abs(newSize);
			renderHd = newSize < 0;
		}
	}
}
