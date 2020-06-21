#pragma once

#include <Windows.h>

namespace registry {

	template<typename TValue>
	struct registry_io {

		HRESULT write(HKEY root, LPCWSTR key, LPCWSTR valueName, const TValue& value);

		template<int _Size>
		HRESULT write(HKEY root, LPCWSTR key, LPCWSTR valueName, const TValue (&value)[_Size]);

		HRESULT read(HKEY root, LPCWSTR key, LPCWSTR valueName, TValue* value);
	};

	static inline HRESULT RegistryDeleteTree(HKEY root, LPCWSTR key) {
		HRESULT hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CURRENT_USER, key));
		return hr;
	}

	static inline HRESULT RegistrySetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, DWORD dataType, LPCBYTE data, DWORD dataSize) {

		HKEY hKey;
		HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyExW(root, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL));
		if (SUCCEEDED(hr))
		{
			hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, valueName, 0, dataType, data, dataSize));
			RegCloseKey(hKey);
		}
		return hr;
	}

	static inline HRESULT RegistryGetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, LPDWORD dataType, LPBYTE data, LPDWORD dataSize) {

		HKEY hKey;
		HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(root, key, 0, KEY_READ, &hKey));

		if (!SUCCEEDED(hr))
			return hr;

		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, valueName, NULL, dataType, data, dataSize));
		if (!SUCCEEDED(hr))
			return hr;

		RegCloseKey(hKey);
		return hr;
	}

	template<typename TValue>
	static inline HRESULT RegistryGetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, TValue* value) {
		registry_io<TValue> reader;
		return reader.read(root, key, valueName, value);
	}

	template<typename TValue, int ArraySize = sizeof(TValue)>
	static inline HRESULT RegistryGetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, TValue(*value)[ArraySize]) {
		registry_io<TValue> reader;
		return reader.read(root, key, valueName, value);
	}

	template<typename TValue>
	static inline HRESULT RegistrySetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, const TValue& value) {
		registry_io<TValue> writer;
		return writer.write(root, key, valueName, value);
	}

	template<typename TValue, int ArraySize = sizeof(TValue)>
	static inline HRESULT RegistrySetValue(HKEY root, LPCWSTR key, LPCWSTR valueName, const TValue(&value)[ArraySize]) {
		registry_io<TValue> writer;
		return writer.write(root, key, valueName, value);
	}
}

template<>
inline HRESULT registry::registry_io<DWORD>::read(HKEY root, LPCWSTR key, LPCWSTR valueName, DWORD* value)
{
	DWORD dType;
	DWORD dSize;
	HRESULT hr = registry::RegistryGetValue(root, key, valueName, &dType, nullptr, &dSize);
	if (!SUCCEEDED(hr))
		return hr;

	if (dType != REG_DWORD || dSize != sizeof(DWORD))
		return E_FAIL;

	hr = registry::RegistryGetValue(root, key, valueName, &dType, (LPBYTE)value, &dSize);
	return hr;
}

template<>
inline HRESULT registry::registry_io<DWORD>::write(HKEY root, LPCWSTR key, LPCWSTR valueName, const DWORD& value)
{
	return registry::RegistrySetValue(root, key, valueName, REG_DWORD, (LPCBYTE)&value, sizeof(DWORD));
}

template<>
template<int _Size>
inline HRESULT registry::registry_io<WCHAR>::write(HKEY root, LPCWSTR key, LPCWSTR valueName, const WCHAR(&value)[_Size])
{
	return registry::RegistrySetValue(root, key, valueName, REG_SZ, (LPCBYTE)&value, sizeof(WCHAR) * _Size);
}
