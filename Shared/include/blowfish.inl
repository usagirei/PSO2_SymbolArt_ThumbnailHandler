#pragma once

#include <blowfish.h>
#include <cstring>

template<size_t _S, typename _T>
inline _T& crypto_blowfish::util::n_box<_S, _T>::operator[](int n)
{
	return m_Data[n];
}

template<size_t _S, typename _T>
inline crypto_blowfish::util::n_box<_S, _T>::n_box(_T d[_S]) {
	memcpy_s(m_Data, sizeof(_T) * _S, d, sizeof(_T) * _S);
}

template<size_t _S, typename _T>
inline crypto_blowfish::util::n_box<_S, _T>::n_box(_T* d, int s) {
	memcpy_s(m_Data, sizeof(_T) * _S, d, s);
}
