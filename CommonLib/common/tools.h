#ifndef _KOOWO_TOOLS_H
#define _KOOWO_TOOLS_H

#include "atlstr.h"
#include <string>
#include <winioctl.h>
using namespace std;

#ifdef __AFXWIN_H__
#define CSTRING CString
#else
#define CSTRING ATL::CString
#endif

#define DRVUNKNOWN		0
#define DRVFIXED		1
#define DRVREMOTE		2
#define DRVRAM			3
#define DRVCD			4
#define DRVREMOVE		5
#define MAX_LOADSTRING		100

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

void MoveFileDelayAbPath(CSTRING& fileName);

HANDLE win32exec(const char* cmdline, const char* workpath, bool waiting);
HANDLE win32Showexec(const char* cmdline, const char* workpath, bool waiting);
void RunExplorer(CSTRING title, CSTRING url , CSTRING prepTitle);
void BringWindowToTop(HWND hwnd ,DWORD type);
void ShowNoActiveTopWindow(HWND hWnd);

void RandomIntValue(int iNum,int* pStoreRandomValues);

unsigned int GetCurrentDate();
unsigned long str2ulong( const string& s );
long str2long( const string& s );
double str2double (const string &s);
unsigned int str2uint( const string& s );
int str2int( const string& s );
string ulong2str( unsigned long d );
string long2str( long d );
string uint2str( unsigned int d );
string int2str( int d );

COLORREF str2rgb(string strClr);

string GetFileNameFromUrl(string url);
string GetExePath();
bool ParsePath(string path, string& basepath, string& filename, string& ext);
bool CutLogFile(string path, int nLen);
string FormatTime(const SYSTEMTIME& time);

string GetKeyValueStrUnescape(const string& line, const string& key);
string GetKeyValueStr1(const string& line, const string& key);
string GetKeyValueStr(const string& line, const string& key);
void ConvertGB2312ToUnicode(char *strGB2312,wchar_t *pszWBuff,int iBufferLen);
void   ReplaceInvalidPlaylist(const string& strSrc,string& strDes);
// RS Hash Function
long RSHash(const char *str);
bool IsVista();
bool IsNetAlive();
string GBToUTF8(const char* str);
string UTF8ToGB(const char* str);
bool IsNumber( string strNum );

typedef enum CP_STORAGE_BUS_TYPE {
	CP_BusTypeUnknown = 0x00,
	CP_BusTypeScsi,
	CP_BusTypeAtapi,
	CP_BusTypeAta,
	CP_BusType1394,
	CP_BusTypeSsa,
	CP_BusTypeFibre,
	CP_BusTypeUsb,
	CP_BusTypeRAID,
	CP_BusTypeMaxReserved = 0x7F
} CP_STORAGE_BUS_TYPE, *CP_PSTORAGE_BUS_TYPE;

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
char chFirstDriveFromMask (ULONG unitmask);
void ReInitUSB_Disk_Letter(char * szMoveDiskName);

HBITMAP LoadAnImage(const char* FileName);

int PinOrUnpinCmd(int iResIndex);
void TaskBarPin();
void TaskBarUnPin();
void SendInstallMsgToSvr(int iType);

#endif