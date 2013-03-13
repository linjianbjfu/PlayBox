#include "tools.h"
#include "MacroInt.h"
#include "YL_StringUtil.h"
#include "YL_URLEncoder.h"
#include "YL_Base64.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include "Wininet.h"
#include <shldisp.h>
#include "YL_FileInfo.h"
#include "LhcImg.h"

#pragma comment(lib,"Wininet.lib")

/**
 * 延迟删除文件，将文件改名，并延迟移动到临时文件夹
*/

void MoveFileDelayAbPath(CSTRING& fileName)
{
	//删不掉的文件更名,延迟删除
	char szTempPath[MAX_PATH], szNum[NUM_LENGTH];
	memset(szTempPath, 0, MAX_PATH);
	memset(szNum, 0, NUM_LENGTH);
	DWORD dwResult=::GetTempPath(MAX_PATH, szTempPath);

	CSTRING TempPath = szTempPath;
	CSTRING temp = fileName.Left(fileName.Find('.'));
	CSTRING postfix = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
	CSTRING tempFile, sysTempFile;

	dwResult=::GetTempPath(MAX_PATH, szTempPath);
	TempPath = szTempPath;
	int n = 0;
	HANDLE hFind;
	WIN32_FIND_DATA ffd; 
	tempFile.Format("%s.%s.tmp", temp.GetBuffer(), itoa(n, szNum, 10));
	hFind = FindFirstFile(tempFile, &ffd);
	while(hFind != INVALID_HANDLE_VALUE)
	{
		n++;
		FindClose(hFind);
		tempFile.Format("%s.%s.tmp", temp.GetBuffer(), itoa(n, szNum, 10));
		hFind = FindFirstFile(tempFile, &ffd);
	}
	tempFile.Format("%s.%s.tmp", temp.GetBuffer(), itoa(n, szNum, 10));
	int ret = temp.ReverseFind('\\');
	CSTRING temp1 = temp;
	if(ret != -1)
	{
		temp1 = temp.Right(temp.GetLength() - ret -1);
	}
	sysTempFile.Format("%s\\%s.tmp", TempPath.GetBuffer(), temp1.GetBuffer());

	if(rename(temp + postfix, tempFile) == 0)
		MoveFileEx(tempFile, sysTempFile, MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING); 
}

void ConvertGB2312ToUnicode(char *strGB2312,wchar_t *pszWBuff,int iBufferLen)
{
	int len=MultiByteToWideChar(CP_ACP, 0,strGB2312, -1, NULL,0);
	if (iBufferLen<len)
	{
		throw exception("Buffer not big enough!");
		return;
	}
	MultiByteToWideChar(CP_ACP, 0, strGB2312, -1, (LPWSTR)pszWBuff, len);
}
/**
 * 功能: 启动一个进程,例如 win32exec("PlayBox.exe", "C:\Program files\YoungPlayBox", false).
*/
HANDLE win32exec(const char* cmdline, const char* workpath, bool waiting) 
{
	STARTUPINFO    		 si;
	PROCESS_INFORMATION  pi;
	char cmd[CMD_LINE_LENGTH];
	int nRet;

	memset(&si, 0, sizeof (STARTUPINFO)) ;
	si.cb = sizeof (STARTUPINFO) ;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	memset(&pi, 0, sizeof (PROCESS_INFORMATION));
	memset(cmd, 0, CMD_LINE_LENGTH);

	if(workpath != "")
	{
		int len = (int)strlen(workpath);
		if(workpath[len-1] != '/' && workpath[len-1] != '\\')
			_snprintf(cmd, CMD_LINE_LENGTH-1, "%s\\%s", workpath, cmdline);
		else
			_snprintf(cmd, CMD_LINE_LENGTH-1, "%s%s", workpath, cmdline);
	}
	else
		_snprintf(cmd, CMD_LINE_LENGTH-1, "%s", cmdline);

	nRet = CreateProcess (NULL, cmd, NULL, NULL,
		FALSE, (DWORD) NORMAL_PRIORITY_CLASS, NULL,
		workpath, &si, &pi);

	if(nRet==0)
	{
		return NULL;
	}

	if(waiting) 
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return pi.hProcess;
}

HANDLE win32Showexec(const char* cmdline, const char* workpath, bool waiting) 
{
	STARTUPINFO    		 si;
	PROCESS_INFORMATION  pi;
	char cmd[2048];
	int nRet;

	memset(&si, 0, sizeof (STARTUPINFO)) ;
	si.cb = sizeof (STARTUPINFO) ;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	memset(&pi, 0, sizeof (PROCESS_INFORMATION));
	memset(cmd, 0, CMD_LINE_LENGTH);

	if(workpath != "")
	{
		int len = (int)strlen(workpath);
		if(workpath[len-1] != '/' && workpath[len-1] != '\\')
			_snprintf(cmd, CMD_LINE_LENGTH-1, "%s\\%s", workpath, cmdline);
		else
			_snprintf(cmd, CMD_LINE_LENGTH-1, "%s%s", workpath, cmdline);
	}
	else
		_snprintf(cmd, CMD_LINE_LENGTH-1, "%s", cmdline);

	nRet = CreateProcess (NULL, cmd, NULL, NULL,
		FALSE, (DWORD) NORMAL_PRIORITY_CLASS, NULL,
		workpath, &si, &pi);
	DWORD dw =GetLastError();

	if(nRet==0)
	{
		return NULL;
	}

	if(waiting) 
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return pi.hProcess;
}

void BringWindowToTop(HWND hwnd ,DWORD type)
{
	DWORD dwFGThreadId, dwFGProcessId,dwThisThreadId;
	HWND hwndForeground = ::GetForegroundWindow();
	dwFGThreadId = GetWindowThreadProcessId(hwndForeground, &dwFGProcessId);
	dwThisThreadId = GetCurrentThreadId();
	AttachThreadInput(dwThisThreadId, dwFGThreadId,TRUE);
	if(type != NULL)
	{
		::ShowWindow(hwnd,type);
	}
	::SetForegroundWindow(hwnd);
	::BringWindowToTop(hwnd);
	AttachThreadInput(dwThisThreadId, dwFGThreadId,FALSE);
}

void RunExplorer(CSTRING title, CSTRING url , CSTRING prepTitle)
{
	//char pbase[PATHSIZE], ipage[PATHSIZE];
	CSTRING info ;

	// 先查找该窗口是否存在，如果已经打开了，直接把该窗口提到前台（好像不行）。
	HWND hwnd = ::FindWindow(NULL, title);
	if (hwnd != NULL)
	{
		BringWindowToTop(hwnd,SW_SHOWMAXIMIZED);
		return;
	}

	ShellExecute(NULL, "open", "iexplore.exe", 	url,"", SW_SHOWMAXIMIZED);
//	ShellExecute(NULL, "open", "", 	url, "", SW_SHOWMAXIMIZED);

	hwnd = NULL;
	for(int i = 0 ;i< 60;i++)
	{
		hwnd = ::FindWindow(NULL , prepTitle);
		if(hwnd != NULL)
		{
			break;
		}
		Sleep(100);
	}
	BringWindowToTop(hwnd,SW_SHOWMAXIMIZED);
}

/*
功能：unsigned long类型转化为string类型
*/
string ulong2str( unsigned long d )
{
	ostringstream oss;
	oss << d;
	return oss.str();
}

/*
功能：long类型转化为string类型
*/
string long2str( long d )
{
	ostringstream oss;
	oss << d;
	return oss.str();
}

/*
功能：unsigned int类型转化为string类型
*/
string uint2str( unsigned int d )
{
	ostringstream oss;
	oss << d;
	return oss.str();
}

/*
功能：int类型转化为string类型
*/
string int2str( int d )
{
	ostringstream oss;
	oss << d;
	return oss.str();
}

/*
功能:string类型转化为unsigned long类型
*/
unsigned long str2ulong( const string& s )
{
	if(s ==  "")
	{
		return 0;
	}

	istringstream iss(s);
	unsigned long i;
	iss >> i;
	return i;
}

/*
功能:string类型转化为long类型
*/
long str2long( const string& s )
{
	if(s ==  "")
	{
		return 0;
	}

	istringstream iss(s);
	long i;
	iss >> i;
	return i;
}

double str2double (const string &s)
{
	if (s == "")
		return 0.00;

	istringstream iss (s);
	double d;
	iss >> d;

	return d;
}

/*
功能:string类型转化为unsigned int类型
*/
unsigned int str2uint( const string& s )
{
	if(s ==  "")
	{
		return 0;
	}

	istringstream iss(s);
	unsigned int i;
	iss >> i;
	return i;
}

/*
功能:string类型转化为int类型
*/
int str2int( const string& s )
{
	if(s ==  "")
	{
		return 0;
	}

	istringstream iss(s);
	int i;
	iss >> i;
	return i;
}

string GetFileNameFromUrl(string url)
{
	string::size_type nIdx = url.find_last_of("\\/");
	if(nIdx != string::npos)
		return url.substr(nIdx + 1, url.length() - nIdx - 1);
	else
		return "";
}

bool ParsePath(string path, string& basepath, string& filename, string& ext)
{
	string::size_type nPos = path.rfind("\\");
	if (nPos == string::npos)
		return false;

	basepath = path.substr(0, nPos+1);
	filename = path.substr(nPos+1, path.length()-nPos-1);

	nPos = filename.rfind(".");

	ext = filename.substr(nPos + 1, path.length() - nPos - 1);
	filename = filename.substr(0, nPos + 1);
	return true;
}

string GetExePath()
{
	char sz[MAX_PATH]={0};
	::GetModuleFileName(NULL, sz, MAX_PATH);
	string ExePath = sz;
	string basepath = "";
	string filename = "";
	string ext = "";
	ParsePath(ExePath, basepath, filename, ext);
	return basepath;
}

bool CutLogFile(string path, int nLen)
{
	if(nLen <= 0)
		return false;

	FILE *file;
	char *szContent = new char[nLen];
	DWORD dwRead = 0;
	if( (file = fopen(path.c_str(), "r")) != NULL )
	{
		fseek( file, 0 - nLen, SEEK_END );
		dwRead = (DWORD)fread( szContent, sizeof(char), nLen, file );
		if(ferror(file))
		{
			dwRead = 0;
		}
		fclose(file);
	}

	if(dwRead == 0)
		return false;

	int n = 0;
	while(n <= nLen && szContent[n] != '\n')
		n++;

	if( (file = fopen(path.c_str(), "w")) != NULL )
	{
		fwrite(szContent + n, sizeof(char), nLen-n, file );
		fclose(file);
	}
	delete[] szContent;
	return true;
}

string FormatTime(const SYSTEMTIME& time)
{
	string strDay, strTime ;
	stringstream stream;
	char fillc = stream.fill('0');

	stream<< setw(2) << time.wMonth << "-" << setw(2) << time.wDay;
	stream>> strDay;
	stream.clear();
	// use two strings (strDay and strTime)here , because of the " " problem
	// when >> a stringtream to a string, >> operation will stop when barge up against " "
	stream<< setw(2) << time.wHour << ":" 
		<< setw(2) << time.wMinute << ":" 
		<< setw(2) << time.wSecond << "."
		<< setw(3) << time.wMilliseconds;
	stream>> strTime;
	strTime = strDay + " " + strTime;
	return strTime;
}

string GetKeyValueStrUnescape(const string& line, const string& key)
{
	vector<string> vecstrs;
	YL_StringUtil::Tokenize(line, vecstrs, "&");
	for(vector<string>::iterator iter = vecstrs.begin(); iter != vecstrs.end(); iter++)
	{
		string::size_type bpos = (*iter).find("=");
		if (bpos == string::npos)
			continue;
		string str = (*iter).substr(0, bpos);
		if(str == key)
		{
			string strval = (*iter).substr(bpos+1, (*iter).length()-bpos-1);
			return UTF8ToGB( YL_URLEncoder::decode((char*)strval.c_str()) );
		}
	}
	return "";
}

string GetKeyValueStr1(const string& line, const string& key) 
{
	string::size_type bpos = line.find(key+"=");
	if (bpos != 0)
		return "";
	bpos += key.length() + 1;
	string::size_type epos = line.find("&", bpos);
	if (epos != string::npos)
		return line.substr(bpos, epos - bpos);
	else
		return line.substr(bpos); 
}
string GetKeyValueStr(const string& line, const string& key) 
{
	//vector<string> vecstrs;
	//YL_StringUtil::Tokenize(line, vecstrs, "&");
	//for(vector<string>::iterator iter = vecstrs.begin(); iter != vecstrs.end(); iter++)
	//{
	//	string::size_type bpos = (*iter).find("=");
	//	if (bpos == string::npos)
	//		continue;
	//	string str = (*iter).substr(0, bpos);
	//	if(str == key)
	//	{
	//		string strval = (*iter).substr(bpos+1, (*iter).length()-bpos-1);
	//		return strval;
	//	}
	//}
	//return "";
	string::size_type bpos = line.find(key+"=");
	if (bpos == string::npos)
		return "";
	bpos += key.length() + 1;
	string::size_type epos = line.find("&", bpos);
	if (epos != string::npos)
		return line.substr(bpos, epos - bpos);
	else
		return line.substr(bpos); 
}

COLORREF str2rgb(string strClr)
{
	string r,g,b;	
	r = "0" + strClr.substr(0,2);
	g = "0" + strClr.substr(2,2);
	b = "0" + strClr.substr(4,2);
	int nr,ng,nb;
	sscanf(r.c_str(), "%x", &nr);
	sscanf(g.c_str(), "%x", &ng);
	sscanf(b.c_str(), "%x", &nb);

	COLORREF color = RGB(nr,ng,nb);
	return color;	
}

void RandomIntValue(int iNum,int* pStoreRandomValues)
{
	static DWORD dwLastThreadID = 0;
	if( dwLastThreadID != GetCurrentThreadId() )
	{
		srand(GetTickCount());
		dwLastThreadID = GetCurrentThreadId();
	}	

	int iRandValue = 0;

	for(int idx = 0;idx < iNum;idx++)
	{
		pStoreRandomValues[idx] = -1;
	}

	for( int idx = 0;idx < iNum;idx++ )
	{
		do{			
			int i = rand();
			iRandValue = i % (int)iNum;
		}while( pStoreRandomValues[iRandValue] != -1 );

		pStoreRandomValues[iRandValue] = idx;		
	}	
}

unsigned int GetCurrentDate()
{
	time_t timer;
	time(&timer);
	return (unsigned int)(timer/(60 * 60 * 24));
}

long RSHash(const char *str)
{
    long b = 479551;
    long a = 63489;
    long hash = 0;

    while (*str)
    {
            hash = hash * a + (*str++);
            a *= b;
    }

    return (hash & 0x7FFFFFFF);
}

bool IsVista()
{
	static bool s_bGetVersion	= false;
	static bool s_bIsVista		= false;

	if( s_bGetVersion )
	{
		return s_bIsVista;
	}

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return false;
	}

	switch (osvi.dwPlatformId)
	{	

	case VER_PLATFORM_WIN32_NT:

		if ( osvi.dwMajorVersion == 6  )
		{
			s_bIsVista = true;			
		}
		else
		{
			s_bIsVista = false;
		}
		break;
	default:
		s_bIsVista = false;
		break;
	}
		
	s_bGetVersion = true;

	return s_bIsVista;
}

#define s_WSTR_INVALID_FILENAME L"\\/\"*<>:?|"

void   ReplaceInvalidPlaylist(const string& strSrc,string& strDes)
{
	int len=MultiByteToWideChar(CP_ACP, 0,strSrc.c_str(), -1, NULL,0);
	unsigned short * wszUtf8 = new unsigned short[len];
	memset(wszUtf8, 0, len * 2);
	MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), -1, (LPWSTR)wszUtf8, len);

	for( size_t idx = 0;idx < len-1;idx++ )
	{
		if( wcschr( s_WSTR_INVALID_FILENAME,wszUtf8[idx] ) != 0 )
		{		
			wszUtf8[idx] = ' ';
		}
	}

	DWORD dwNum = WideCharToMultiByte(CP_ACP,NULL,(LPWSTR)wszUtf8,-1,NULL,0,NULL,FALSE);

	char *buffer = new char[ dwNum ];
	memset(buffer, 0, dwNum);
	WideCharToMultiByte(CP_ACP,NULL,(LPWSTR)wszUtf8,-1,buffer,dwNum,NULL,NULL);

	strDes = buffer;

	delete[] buffer;
	delete[] wszUtf8;
}

void ShowNoActiveTopWindow(HWND hWnd)
{
	RECT rect;
	HWND hwndact;

	int nFullWidth  = GetSystemMetrics(SM_CXSCREEN);
	int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
	hwndact = ::GetForegroundWindow();
	if(hwndact == NULL)
	{
		ShowWindow(hWnd, SW_SHOWNOACTIVATE);
		return;
	}

	::GetWindowRect(hwndact, &rect);
	if(hWnd == NULL)
		return;

	if(nFullWidth ==  rect.right - rect.left && nFullHeight == rect.bottom - rect.top)
	{
		::SetWindowPos(hWnd, hwndact, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		ShowWindow(hWnd,SW_SHOWNOACTIVATE);
	}else{
		::SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	}
}

/****************************************************************************
*
*    FUNCTION: GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
*
*    PURPOSE:  get the info of specified device
*
****************************************************************************/
BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY	Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length
	BOOL bResult;					// IOCTL return val

	// specify the query type
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,			// device handle
			IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
			&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
			pDevDesc, pDevDesc->Size,				// output data buffer
			&dwOutBytes,							// out's length
			(LPOVERLAPPED)NULL);					

	return bResult;
}

/****************************************************************************
*
*    FUNCTION: chFirstDriverFrameMask(ULONG unitmask)
*
*    PURPOSE:  get the logic name of driver
*
****************************************************************************/
char chFirstDriveFromMask (ULONG unitmask)
{

      char i;
      for (i = 0; i < 26; ++i)  
      {
           if (unitmask & 0x1) 
				break;
            unitmask = unitmask >> 1;
      }
    return (i + 'A');
}

/****************************************************************************
*
*    FUNCTION: ReInitUSB_Disk_Letter()
*
*    PURPOSE:  get the usb disks, and filling the 'szMoveDiskName' with them
*
****************************************************************************/
void ReInitUSB_Disk_Letter(char * szMoveDiskName)
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			drive, drivetype;
	TCHAR			szBuf[300];
	HANDLE			hDevice;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	for(k=0; k<26; k++)
		szMoveDiskName[k] = '\0';	
	k = 0;		
	// Get available drives we can monitor
	MaxDriveSet = CurDriveSet = 0;

	MaxDriveSet = GetLogicalDrives();
	CurDriveSet = MaxDriveSet;
	char szDrvName[33];
	for ( drive = 0; drive < 32; ++drive )  
	{
		if ( MaxDriveSet & (1 << drive) )  
		{
			DWORD temp = 1<<drive;
			_stprintf( szDrvName, _T("%c:\\"), 'A'+drive );
			switch ( GetDriveType( szDrvName ) )  
			{
				case 0:					// The drive type cannot be determined.
				case 1:					// The root directory does not exist.
					drivetype = DRVUNKNOWN;
					break;
				case DRIVE_REMOVABLE:	// The drive can be removed from the drive.
					drivetype = DRVREMOVE;
					szMoveDiskName[k] = chFirstDriveFromMask(temp);
					k++;
					break;
				case DRIVE_CDROM:		// The drive is a CD-ROM drive.
					break;
				case DRIVE_FIXED:		// The disk cannot be removed from the drive.
					drivetype = DRVFIXED;
					sprintf(szBuf, "\\\\?\\%c:", 'A'+drive);
					hDevice = CreateFile(szBuf, GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

					if (hDevice != INVALID_HANDLE_VALUE)
					{

						pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

						pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

						if(GetDisksProperty(hDevice, pDevDesc))
						{
							if(pDevDesc->BusType == CP_BusTypeUsb)
							{
								szMoveDiskName[k] = chFirstDriveFromMask(temp);
								k++;
							}
						}

						delete pDevDesc;
						CloseHandle(hDevice);
					}
					
					break;
				case DRIVE_REMOTE:		// The drive is a remote (network) drive.
					drivetype = DRVREMOTE;
					szMoveDiskName[k] = chFirstDriveFromMask(temp);
					k++;
					break;
				case DRIVE_RAMDISK:		// The drive is a RAM disk.
					drivetype = DRVRAM;
					break;
			}
		}
	}
}

// Function LoadAnImage: accepts a file name and returns a HBITMAP.
// On error, it returns 0.
HBITMAP LoadAnImage(const char* FileName)
{
	// Use IPicture stuff to use JPG / GIF files
	IPicture* p;
	IStream* s;
	HGLOBAL hG;
	void* pp;
	FILE* fp;

	// Read file into memory
	fp = fopen(FileName, "rb");
	if (!fp)
	{
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	int fs = ftell(fp);
	fseek(fp,0,SEEK_SET);
	hG = GlobalAlloc(GPTR,fs);
	if(!hG)
	{
		fclose(fp);
		return NULL;
	}
	pp = (void*)hG;
	fread(pp,1,fs,fp);
	fclose(fp);

	// Create an IStream so IPicture can
	CreateStreamOnHGlobal(hG, false, &s);
	if(!s)
	{
		GlobalFree(hG);
		return NULL;
	}

	OleLoadPicture(s,0,false,IID_IPicture,(void**)&p);

	if(!p)
	{
		s->Release();
		GlobalFree(hG);
		return NULL;
	}
	s->Release();
	GlobalFree(hG);

	HBITMAP hB = 0;
	p->get_Handle((unsigned int*)&hB);

	// Copy the image. Necessary, because upon p's release,
	// the handle is destroyed.
	HBITMAP hBB = (HBITMAP)CopyImage(hB, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);

	p->Release();
	return hBB;
}

bool IsNetAlive()
{
	DWORD dwOnline;
	return InternetGetConnectedState(&dwOnline, 0);
}

string GBToUTF8(const char* str)
{
     string result;
     WCHAR *strSrc;
     TCHAR *szRes;

     //获得临时变量的大小
     int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
     strSrc = new WCHAR[i+1];
     MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

     //获得临时变量的大小
     i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
     szRes = new TCHAR[i+1];
     int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

     result = szRes;
     delete []strSrc;
     delete []szRes;

     return result;
}

string UTF8ToGB(const char* str)
{
     string result;
     WCHAR *strSrc;
     TCHAR *szRes;

     //获得临时变量的大小
     int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
     strSrc = new WCHAR[i+1];
     MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

     //获得临时变量的大小
     i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
     szRes = new TCHAR[i+1];
     WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

     result = szRes;
     delete []strSrc;
     delete []szRes;

     return result;
}

bool IsNumber( string strNum )
{
	for( size_t i=0;i<strNum.length(); i++)
	{
		if( strNum.at(i) < '0' || strNum.at(i) > '9')
		{
			return false;
		}
	}
	return true;
}

//5386 pin
int PinOrUnpinCmd(int iResIndex)
{
	char szPath[MAX_PATH];
	if(!CLhcImg::GetHomePath(szPath, MAX_PATH))
		return 0;

	string strDirPath = string(szPath);
	if (strDirPath.empty())
		return 0;
	string strFileName = "PlayBox.exe";

	HMODULE hShell32 = ::LoadLibrary("shell32.dll");
	if (!hShell32)
		return 1;

	char szCmd[512];
	int nLen = ::LoadString(hShell32, iResIndex, szCmd, 512);

	::CoInitialize(NULL);
	CComPtr<IShellDispatch> pShell;
	if (FAILED(::CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID*)&pShell)) || !pShell)
		return 1;

	CComPtr<Folder> pFolder;
	if (FAILED(pShell->NameSpace(CComVariant(strDirPath.c_str()), &pFolder)) || !pFolder)
		return 1;
	CComPtr<FolderItem> pItem;
	if (FAILED(pFolder->ParseName(CComBSTR(strFileName.c_str()), &pItem)) || !pItem)
		return 1;

	CComPtr<FolderItemVerbs> pItemVerbs;
	if (FAILED(pItem->Verbs(&pItemVerbs)) || !pItemVerbs)
		return 1;
	long lCount = 0;
	pItemVerbs->get_Count(&lCount);
	for (long i = 0; i < lCount; i++)
	{
		CComPtr<FolderItemVerb> pItemVerb;
		pItemVerbs->Item(CComVariant(i), &pItemVerb);
		if (pItemVerb)
		{
			CComBSTR bstrName;
			pItemVerb->get_Name(&bstrName);
			if (bstrName == szCmd)
			{
				pItemVerb->DoIt();
				return 0;
			}
		}
	}
	return 0;
}

void TaskBarPin()
{
	PinOrUnpinCmd(5386);
}

void TaskBarUnPin()
{
	PinOrUnpinCmd(5387);
}