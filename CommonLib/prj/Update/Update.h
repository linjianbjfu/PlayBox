//define update.h
#include "../../include/YL_HTTPRequest.h"
#include "../../common/MacroStr.h"
#include "../Log/KwLogSvr.h"

struct CmdLimit
{
	char szCmd[32];
	unsigned int nInterval;
	unsigned int nTimesLimit;
	unsigned int nDaysLimit;
};

//for Test_Speed; Download; Install
extern CmdLimit         g_CmdsLimit[4];
extern unsigned int     g_nAppIDLimit;
extern HANDLE           g_hUpdateMutex;
extern char             g_szModule[64];
extern HWND             g_hClientWnd;

void __stdcall BeginUpdate2(const char* szSoft, const char* szModule, HWND hWnd);