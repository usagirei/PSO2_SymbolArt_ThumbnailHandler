#pragma once

// IClassFactory
#include <unknwn.h>
#include <windows.h>


class ClassFactory final 
    : public IClassFactory
{
public:
  // IUnknown
  IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
  IFACEMETHODIMP_(ULONG) AddRef();
  IFACEMETHODIMP_(ULONG) Release();

  // IClassFactory
  IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
  IFACEMETHODIMP LockServer(BOOL fLock);

  ClassFactory();

protected:
  ~ClassFactory();

private:
  long m_cRef;
};
