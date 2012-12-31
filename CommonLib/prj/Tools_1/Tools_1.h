#include <tchar.h>
#include <string>
using namespace std;
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TOOLS_1_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TOOLS_1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


//Set the interface.
int __stdcall CallTools(int argc, _TCHAR* argv1 ,_TCHAR *argv2 ,_TCHAR *argv3 ,_TCHAR *argv4);

typedef void (__stdcall *LPCREATERECOGNISE)(const char* szSoft, const char* szModule, HWND hWnd);
void Update4SendLog(void *pparam);
void SendRealMsg( string& strMsg ,int iMsgType);
//string FormatMsg(int nMsgType, const string& msg);
string Trim( string str ); //去掉str前后的双引号或者单引号
BOOL	GetMyDoucuments(TCHAR *szPath);
