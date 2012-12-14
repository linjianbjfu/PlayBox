#ifndef _KOOWO_TOOLS_H
#define _KOOWO_TOOLS_H

#include "atlstr.h"
#include <string>
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

// retrieve the storage device descriptor data for a device. 
typedef struct _STORAGE_DEVICE_DESCRIPTOR {
	ULONG  Version;
	ULONG  Size;
	UCHAR  DeviceType;
	UCHAR  DeviceTypeModifier;
	BOOLEAN  RemovableMedia;
	BOOLEAN  CommandQueueing;
	ULONG  VendorIdOffset;
	ULONG  ProductIdOffset;
	ULONG  ProductRevisionOffset;
	ULONG  SerialNumberOffset;
	CP_STORAGE_BUS_TYPE  BusType;
	ULONG  RawPropertiesLength;
	UCHAR  RawDeviceProperties[1];

} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_QUERY_TYPE {
	PropertyStandardQuery = 0,
	PropertyExistsQuery,
	PropertyMaskQuery,
	PropertyQueryMaxDefined

} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_PROPERTY_ID {
	StorageDeviceProperty = 0,
	StorageAdapterProperty,
	StorageDeviceIdProperty

} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

// retrieve the properties of a storage device or adapter. 
typedef struct _STORAGE_PROPERTY_QUERY {
	STORAGE_PROPERTY_ID  PropertyId;
	STORAGE_QUERY_TYPE  QueryType;
	UCHAR  AdditionalParameters[1];

} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
char chFirstDriveFromMask (ULONG unitmask);
void ReInitUSB_Disk_Letter(char * szMoveDiskName);

HBITMAP LoadAnImage(const char* FileName);


#endif