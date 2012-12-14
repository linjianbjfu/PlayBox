#ifndef _YL_ENC_FILE_REG_H
#define _YL_ENC_FILE_REG_H

#include "windows.h"
#include <string>
using std::string;

class YL_EncFileReg
{
public:
	static LONG DeleteValue(HKEY regClass, const string &regEntry, const string &valueName);
	static bool WriteString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , const string& strKeyValue);
	static bool WriteString(HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const char* pszKeyValue);
	static bool ReadString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , string& strKeyValue);
	static bool ReadString(HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,char szBuffer[],unsigned int unBuffLen);
	static bool WriteDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName ,const unsigned int& unValue);
	static bool WriteDWORD(HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const unsigned int& unValue);
	static bool ReadDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , unsigned int& unValue);
	static bool ReadDWORD(HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,unsigned int& unValue);
	
	static DWORD GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDefault = NULL);
	static BOOL WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue);
	static int GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault );
	static BOOL WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue );
	static BOOL GetBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault);
	static BOOL WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue);

	static bool GetEncFilePath( string& strPath );
	static string Enc( unsigned int i );
	static string Enc( string str );
	static string Dec2Str( string str );
	static unsigned int Dec2Int( string str );

	static string StrHash( string str );
};


#endif