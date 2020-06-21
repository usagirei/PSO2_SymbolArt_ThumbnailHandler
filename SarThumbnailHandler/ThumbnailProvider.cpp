#include "ThumbnailProvider.h"

#include <Shlwapi.h>
#include <memory>

#include "openglrenderer.h"
#include "config.h"
#include "debug.h"
#include "registry.h"

ThumbnailProvider::ThumbnailProvider()
	: m_cRef(1), m_pStream(nullptr)
{
	InterlockedIncrement(&g_cRefDll);
}

ThumbnailProvider::~ThumbnailProvider()
{
	if (m_pStream)
	{
		m_pStream->Release();
	}

	InterlockedDecrement(&g_cRefDll);
}

// IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP ThumbnailProvider::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
	  QITABENT(ThumbnailProvider, IThumbnailProvider),
	  QITABENT(ThumbnailProvider, IInitializeWithStream),
	  { 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ThumbnailProvider::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ThumbnailProvider::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef)
	{
		delete this;
	}

	return cRef;
}


// IInitializeWithStream

IFACEMETHODIMP ThumbnailProvider::Initialize(IStream* pStream, DWORD grfMode)
{
	if (m_pStream != nullptr)
		return E_UNEXPECTED;

	return pStream->QueryInterface(&m_pStream);
}


// IThumbnailProvider

extern bool __debuggerLaunch();
extern void __hexdump(void* d, int l);

IFACEMETHODIMP ThumbnailProvider::GetThumbnail(UINT thumb_size, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{
	DEBUG_LAUNCH();
	DEBUG_PRINT("Request Thumbnail: " << thumb_size);

	std::unique_ptr<SarFile> sar;
	try {
		sar = std::make_unique<SarFile>(m_pStream);
	}
	catch (sar_exception) {
		return E_FAIL;
	}

	

	int maxDim = __max(sar->width(), sar->height());
	thumb_size = __min(8 * maxDim, thumb_size);
	
	int newW = sar->width() * thumb_size / maxDim;
	int newH = sar->height() * thumb_size / maxDim;

	bool isThumbHighRes = thumb_size > 256;

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = newW;
	bmi.bmiHeader.biHeight = -static_cast<LONG>(newH);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	BYTE* pBits;
	HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pBits), NULL, 0);

	if (!hbmp)
		return E_OUTOFMEMORY;

	if (!g_pSarRenderer) {
		try {
			g_pSarRenderer = std::make_unique<OpenGLRenderer>();
		}
		catch (glrenderer_exception) {
			g_pSarRenderer = nullptr;
			return E_FAIL;
		}
	}

	DWORD highDefEnabled;
	HRESULT hr = registry::RegistryGetValue(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler, L"HighDefinition", &highDefEnabled);
	if (!SUCCEEDED(hr))
		highDefEnabled = 0;

	bool renderInHd = isThumbHighRes && (highDefEnabled != 0);

	g_pSarRenderer->SetFlag(OpenGLRenderer::FLAG_HD, renderInHd);
	g_pSarRenderer->Render(*sar, newW, newH, pBits);

	*pdwAlpha = WTSAT_ARGB;
	*phbmp = hbmp;

	return S_OK;
}
