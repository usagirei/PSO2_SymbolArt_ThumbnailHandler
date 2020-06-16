#include "ClassFactory.h"

#include "ThumbnailProvider.h"

#include <new>
#include <Shlwapi.h>

#include "config.h"


extern long g_cRefDll;

ClassFactory::ClassFactory() : m_cRef(1)
{
	InterlockedIncrement(&g_cRefDll);
}

ClassFactory::~ClassFactory()
{
	InterlockedDecrement(&g_cRefDll);
}


//
// IUnknown
//

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(ClassFactory, IClassFactory),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef)
	{
		delete this;
	}
	return cRef;
}


// 
// IClassFactory
//

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	ThumbnailProvider* pNew = new (std::nothrow) ThumbnailProvider();
	if (!pNew)
		return E_OUTOFMEMORY;

	HRESULT hr = pNew->QueryInterface(riid, ppv);
	pNew->Release();

	return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
	{
		InterlockedIncrement(&g_cRefDll);
	}
	else
	{
		InterlockedDecrement(&g_cRefDll);
	}
	return S_OK;
}
