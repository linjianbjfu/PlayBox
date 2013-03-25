//define update.h
#include "../../include/YL_HTTPRequest.h"
#include "../../common/MacroStr.h"
#include "../Log/KwLogSvr.h"

//for Test_Speed; Download; Install
extern HANDLE           g_hUpdateMutex;
extern char             g_szModule[64];
extern HWND             g_hClientWnd;

void __stdcall BeginUpdate2(const char* szSoft, const char* szModule, HWND hWnd);