// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "BHO_Demo_i.h"
#include "dllmain.h"
#define CLSID_IEPlugin_Str _T("{3543619C-D563-43f7-95EA-4DA7E1CC396A}")
CBHO_DemoModule _AtlModule;

HINSTANCE g_hInstance;
// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			//MessageBox(NULL,L"Message from dll attatch.",L"Hello Lea",MB_OK|MB_ICONINFORMATION);
			g_hInstance = hInstance;
			DisableThreadLibraryCalls(hInstance);
		}

		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
