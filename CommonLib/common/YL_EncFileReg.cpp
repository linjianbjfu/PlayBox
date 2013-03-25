#include "YL_EncFileReg.h"
#include "YL_RegInfo.h"
#include "YL_Base64.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"
#include "GlobalParas.h"
#include "tools.h"

string YL_EncFileReg::StrHash( string str )
{
	return str;
	//do not hash
	//string strRet = "A";
	//strRet += long2str( RSHash( str.c_str() ) );
	//return strRet;
}

string YL_EncFileReg::Enc( unsigned int i )
{
	return Enc( uint2str( i ) );
}

string YL_EncFileReg::Enc( string str )
{
	return str;
	//do not enc
	//char szRet[1024];
	//memset( szRet, 0, 1024 );
	//YL_Base64Encode( szRet, str.c_str(), str.length(), "young" );
	//string strRet = string( szRet );
	//return strRet;
}

string YL_EncFileReg::Dec2Str( string str )
{
	return str;
	//do not dec
	//char szRet[1024];
	//memset( szRet, 0, 1024 );
	//YL_Base64Decode( szRet, str.c_str(), "young" );
	//return string( szRet );
}

unsigned int YL_EncFileReg::Dec2Int( string str )
{
	string strDes = Dec2Str( str );
	return str2int( strDes );
}

bool YL_EncFileReg::GetEncFilePath( string& strPath )
{
	strPath.clear();
	YL_RegInfo::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, strPath);
	if(strPath.length() <= 0)
	{
		string str = GetExePath();
		if(str.length() <= 0)
			return false;
		else if(str[str.length()-1] == '\\')
			str = str.substr(0, str.length()-1);

		YL_RegInfo::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, str.c_str());
		strPath = str;
	}
	strPath += "\\Resources\\Profiles\\conf.ini";
	return true;
}

LONG YL_EncFileReg::DeleteValue(HKEY regClass, const string &regEntry, const string &valueName)
{
	string strPath;
	GetEncFilePath( strPath );
	string strSection = StrHash( regEntry );
	string strValue   = StrHash( valueName );
	WritePrivateProfileString( strSection.c_str(), strValue.c_str(), NULL, strPath.c_str() );
	return 0L;
}

bool YL_EncFileReg::WriteString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , const string& strKeyValue)
{
	string strPath;
	GetEncFilePath( strPath );
	string strSection = StrHash( strRegEntry );
	string strKey     = StrHash( strKeyName );
	string strValue   = Enc( strKeyValue );
	WritePrivateProfileString( strSection.c_str(), strKey.c_str(), strValue.c_str(), strPath.c_str() );
	return true;
}

bool YL_EncFileReg::WriteString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const char* pszKeyValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL || pszKeyValue == NULL )
	{
		return false;
	}
	return WriteString( hRegKey,string(pszRegEntry),string(pszKeyName),string(pszKeyValue) );
}


bool YL_EncFileReg::ReadString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , string& strKeyValue)
{
	string strPath;
	GetEncFilePath( strPath );
	string strSection = StrHash( strRegEntry );
	string strKey     = StrHash( strKeyName );
	char szValue[1024];
	memset(szValue,0,1024);
	GetPrivateProfileString( strSection.c_str(), strKey.c_str(), "", szValue, 1024, strPath.c_str() );
	
	if( strlen(szValue) == 0 )
	{
		strKeyValue.clear();
		return false;
	}else
	{
		strKeyValue = Dec2Str( string(szValue) );
		return true;
	}	
}

bool YL_EncFileReg::ReadString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,char szBuffer[],unsigned int unBuffLen)
{
	if( pszRegEntry == NULL || pszKeyName == NULL)
	{
		return false;
	}
	string strKeyValue;
	if( ReadString( hRegKey,string(pszRegEntry),string(pszKeyName),strKeyValue) && strKeyValue.length() <= unBuffLen - 1  )
	{
		strcpy( szBuffer,strKeyValue.c_str() );
		return true;
	}
	return false;
}


bool YL_EncFileReg::WriteDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName ,const unsigned int& unValue)
{
	string strPath;
	GetEncFilePath( strPath );
	string strSection = StrHash( strRegEntry );
	string strKey     = StrHash( strKeyName );
	string strValue   = Enc( unValue );
	WritePrivateProfileString( strSection.c_str(), strKey.c_str(), strValue.c_str(), strPath.c_str() );
	return true;
}
bool YL_EncFileReg::WriteDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const unsigned int& unValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL )
	{
		return false;
	}
	return WriteDWORD( hRegKey,string(pszRegEntry),string(pszKeyName),unValue );
}

bool YL_EncFileReg::ReadDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , unsigned int& unValue)
{
	string strPath;
	GetEncFilePath( strPath );
	string strSection = StrHash( strRegEntry );
	string strKey     = StrHash( strKeyName );
	char szValue[1024];
	memset(szValue,0,1024);
	GetPrivateProfileString( strSection.c_str(), strKey.c_str(), "", szValue, 1024, strPath.c_str() );
	if( strlen(szValue) == 0 )
	{
		unValue = 0;
		return false;
	}else
	{
		unValue = Dec2Int( string(szValue) );
		return true;
	}
}

bool YL_EncFileReg::ReadDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,unsigned int& unValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL )
	{
		return false;
	}
	return ReadDWORD( hRegKey,string(pszRegEntry),string(pszKeyName),unValue );
}

DWORD YL_EncFileReg::GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDefault)
{
	if( !ReadString(HKEY_PLAYBOX_ROOT,lpSection,lpKey,lpBuffer,dwBufSize) )
	{
		memset( lpBuffer, 0, dwBufSize );
		if( lpDefault != NULL )
		{
			_snprintf( lpBuffer, dwBufSize, "%s", lpDefault );
		}		
	}
	return dwBufSize;
}

BOOL YL_EncFileReg::WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue)
{
	return WriteString(HKEY_PLAYBOX_ROOT,lpSection,lpKey,lpValue);
}

int YL_EncFileReg::GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault )
{
	unsigned int iRet = 0;
	if( !ReadDWORD(HKEY_PLAYBOX_ROOT,lpSection,lpKey,iRet) )
	{
		iRet = nDefault;
	}
	return iRet;
}

BOOL YL_EncFileReg::WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue )
{
	return WriteDWORD(HKEY_PLAYBOX_ROOT,lpSection,lpKey,nValue);
}

BOOL YL_EncFileReg::GetBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault)
{
	return (bool)GetInt( lpSection, lpKey, (int)bDefault );

}

BOOL YL_EncFileReg::WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue)
{
	return WriteInt( lpSection, lpKey, (int)bValue );
}