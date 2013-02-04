// BHO_Demo.cpp : DLL ������ʵ�֡�


#include "stdafx.h"
#include "resource.h"
#include "BHO_Demo_i.h"
#include "dllmain.h"
#define CLSID_IEPlugin_Str L"{4A194146-1E93-4F29-B8CB-E3E46DC0CB67}"
extern HINSTANCE g_hInstance;
// ����ȷ�� DLL �Ƿ���� OLE ж��
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// ����һ���๤���Դ������������͵Ķ���
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - ������ӵ�ϵͳע���
STDAPI DllRegisterServer(void)
{
    // ע��������Ϳ�����Ϳ��е����нӿ�
	HKEY hk;
	TCHAR dllpath[1024];
	DWORD n;

	// Get the full path to this DLL's file so we can register it
	GetModuleFileName(g_hInstance,dllpath,1024);
	// Create our key under HKCR\\CLSID
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IEPlugin_Str,0,NULL,0,KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) return SELFREG_E_CLASS;
	// Set the name of our BHO
	RegSetValueEx(hk,NULL,0,REG_SZ,(const BYTE*)_T("CodeProject Example BHO"),24*sizeof(TCHAR));
	RegCloseKey(hk);
	// Create the InProcServer32 key
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IEPlugin_Str _T("\\InProcServer32"),0,NULL,0,KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) return SELFREG_E_CLASS;
	// Set the path to this DLL
	RegSetValueEx(hk,NULL,0,REG_SZ,(const BYTE*)dllpath,(_tcslen(dllpath)+1)*sizeof(TCHAR));
	// Set the ThreadingModel to Apartment
	RegSetValueEx(hk,_T("ThreadingModel"),0,REG_SZ,(const BYTE*)_T("Apartment"),10*sizeof(TCHAR));
	RegCloseKey(hk);
	// Now register the BHO with Internet Explorer
	//32bit
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") CLSID_IEPlugin_Str,0,NULL,0,KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) return SELFREG_E_CLASS;
	//64bit
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") CLSID_IEPlugin_Str,0,NULL,0,KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) return SELFREG_E_CLASS;
	// I believe the following tells explorer.exe not to load our BHO
	n=1;
	RegSetValueEx(hk,_T("NoExplorer"),0,REG_DWORD,(const BYTE*)&n,sizeof(DWORD));
	RegCloseKey(hk);

	//MessageBox(NULL,L"Registered successfully.",L"Lea",MB_OK|MB_ICONINFORMATION);


    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - �����ϵͳע������Ƴ�
STDAPI DllUnregisterServer(void)
{
	// Remove the Internet Explorer BHO registration
	//32bit
	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") CLSID_IEPlugin_Str);
	//64bit
	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") CLSID_IEPlugin_Str);
	// Remove the COM registration, starting with the deeper key first since RegDeleteKey is not recursive
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IEPlugin_Str _T("\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IEPlugin_Str);


	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - ���û����߰��������ϵͳע��������/ɾ��
//              �	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL)
    {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    	{
    		AtlSetPerUserRegistration(true);
    	}
    }

    if (bInstall)
    {	
    	hr = DllRegisterServer();
    	if (FAILED(hr))
    	{	
    		DllUnregisterServer();
    	}
    }
    else
    {
    	hr = DllUnregisterServer();
    }

    return hr;
}


