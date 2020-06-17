#pragma once

#include <windows.h>
#include <thumbcache.h>

#include <memory>

#include "renderer.h"

extern HINSTANCE					g_hInstDll;
extern long							g_cRefDll;
extern std::unique_ptr<SarRenderer> g_pSarRenderer;

class ThumbnailProvider final :
	public IInitializeWithStream,
	public IThumbnailProvider
{
public:
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IInitializeWithStream
	IFACEMETHODIMP Initialize(IStream* pStream, DWORD grfMode);

	// IThumbnailProvider
	IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha);

	ThumbnailProvider();

protected:
	~ThumbnailProvider();

private:
	long m_cRef;
	IStream* m_pStream;
};

