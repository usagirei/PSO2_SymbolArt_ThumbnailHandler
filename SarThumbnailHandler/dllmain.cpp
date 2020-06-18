#include <windows.h>
#include <Guiddef.h>
#include <shlobj.h>

#include "ClassFactory.h"
#include "ThumbnailProvider.h"

#include "config.h"
#include "debug.h"


HINSTANCE  g_hInstDll = NULL;
long  g_cRefDll = 0;
std::unique_ptr<SarRenderer> g_pSarRenderer;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstDll = hModule;

		DEBUG_OPENCONSOLE();
		DEBUG_PRINT("Hello");
		DEBUG_PRINT("PID: " << GetCurrentProcessId());

		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_PROCESS_DETACH:
		DEBUG_PRINT("Bye");
		if (g_pSarRenderer) {
			g_pSarRenderer.reset();
		}

		break;
	}
	return TRUE;
}


STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	if (ppv == NULL)
		return E_INVALIDARG;

	if (!IsEqualCLSID(CLSID_ShelExtensionHandler, rclsid))
		return CLASS_E_CLASSNOTAVAILABLE;

	ClassFactory* pClassFactory = new ClassFactory();
	if (!pClassFactory)
		return E_OUTOFMEMORY;

	HRESULT hr = pClassFactory->QueryInterface(riid, ppv);
	pClassFactory->Release();

	return hr;
}

STDAPI DllCanUnloadNow(void)
{
	return g_cRefDll > 0 ? S_FALSE : S_OK;
}

struct REGISTRY_ENTRY
{
	HKEY   keyRoot;
	LPCWSTR keyName;
	LPCWSTR valueName;
	DWORD   dataType;
	DWORD   dataSize;
	LPCVOID dataPtr;
};

HRESULT CreateRegKeyAndSetValue(const REGISTRY_ENTRY* pEntry)
{
	HKEY hKey;
	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyExW(pEntry->keyRoot, pEntry->keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL));
	if (SUCCEEDED(hr))
	{
		hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, pEntry->valueName, 0, pEntry->dataType, (LPBYTE)pEntry->dataPtr, pEntry->dataSize));
		RegCloseKey(hKey);
	}
	return hr;
}

STDAPI DllRegisterServer(void)
{
	HRESULT hr;

	WCHAR szModuleName[MAX_PATH];

	if (!GetModuleFileNameW(g_hInstDll, szModuleName, ARRAYSIZE(szModuleName)))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		DWORD dwTreatment = 1; // Picture
		DWORD dwModuleNameLen = wcslen(szModuleName) * sizeof(WCHAR);
		const REGISTRY_ENTRY rgRegistryEntries[] =
		{
			// RootKey				KeyName																								ValueName				Type		Size,									Data
			{HKEY_CURRENT_USER,		L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler,										NULL,					REG_SZ,		sizeof(HANDLER_DESCRIPTION),			HANDLER_DESCRIPTION},
			{HKEY_CURRENT_USER,		L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler L"\\InProcServer32",					NULL,					REG_SZ,		dwModuleNameLen,						szModuleName},
			{HKEY_CURRENT_USER,		L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler L"\\InProcServer32",					L"ThreadingModel",		REG_SZ,		sizeof(L"Apartment"),					L"Apartment"},

			{HKEY_CURRENT_USER,		L"Software\\Classes\\" SZ_FORMAT_EXTENSION,															L"Treatment",			REG_DWORD,	sizeof(DWORD),							&dwTreatment},
			{HKEY_CURRENT_USER,		L"Software\\Classes\\" SZ_FORMAT_EXTENSION "\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}",		NULL,					REG_SZ,		sizeof(SZ_CLSID_ShellExtendionHandler),	SZ_CLSID_ShellExtendionHandler},
		};

		hr = S_OK;
		for (int i = 0; i < ARRAYSIZE(rgRegistryEntries) && SUCCEEDED(hr); i++)
		{
			hr = CreateRegKeyAndSetValue(&rgRegistryEntries[i]);
		}
	}
	if (SUCCEEDED(hr))
	{
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	}
	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = S_OK;

	const PCWSTR rgpszKeys[] =
	{
		L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler,
		L"Software\\Classes\\" SZ_FORMAT_EXTENSION "\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}",
	};

	for (int i = 0; i < ARRAYSIZE(rgpszKeys) && SUCCEEDED(hr); i++)
	{
		hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CURRENT_USER, rgpszKeys[i]));
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			hr = S_OK;
		}
	}
	return hr;
}
