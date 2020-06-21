#include <windows.h>
#include <Guiddef.h>
#include <shlobj.h>

#include "ClassFactory.h"
#include "ThumbnailProvider.h"

#include "config.h"
#include "debug.h"
#include "registry.h"


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
	HRESULT hr = S_OK;

	WCHAR szModuleName[MAX_PATH];

	if (!GetModuleFileNameW(g_hInstDll, szModuleName, ARRAYSIZE(szModuleName)))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		//DEBUG_LAUNCH();
		registry::RegistrySetValue(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler, NULL, HANDLER_DESCRIPTION);
		registry::RegistrySetValue<DWORD>(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler, L"HighDefinition", 0);
		registry::RegistrySetValue(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler L"\\InProcServer32", NULL, szModuleName);
		registry::RegistrySetValue(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" SZ_CLSID_ShellExtendionHandler L"\\InProcServer32", L"ThreadingModel", L"Apartment");

		registry::RegistrySetValue<DWORD>(HKEY_CURRENT_USER, L"Software\\Classes\\" SZ_FORMAT_EXTENSION, L"Treatment", 1);
		registry::RegistrySetValue(HKEY_CURRENT_USER, L"Software\\Classes\\" SZ_FORMAT_EXTENSION "\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}", NULL, SZ_CLSID_ShellExtendionHandler);
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
		hr = registry::RegistryDeleteTree(HKEY_CURRENT_USER, rgpszKeys[i]);
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			hr = S_OK;
	}
	return hr;
}
