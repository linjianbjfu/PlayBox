#include "windows.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "YL_FileInfo.h"



bool YL_FileInfo::IsValid( const char* pszFilePath )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	return IsValid(string(pszFilePath));
}

bool YL_FileInfo::IsValid( const string& strFilePath )
{
	struct __stat64 m_buf;
	return !_stat64( strFilePath.c_str(), &m_buf );
}

bool YL_FileInfo::IsDirectory( const char* pszFilePath )
{
	if( pszFilePath == NULL )
	{
		return false;
	}	
	return IsDirectory(string(pszFilePath));
}

bool YL_FileInfo::IsDirectory( const string& strFilePath )
{
	DWORD  m_dwFileAttr = ::GetFileAttributes( strFilePath.c_str() );
	return m_dwFileAttr != INVALID_FILE_ATTRIBUTES && (m_dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) > 0 ;
}

bool YL_FileInfo::IsAttrSystem( const char* pszFilePath )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	return IsAttrSystem(string(pszFilePath));
}

bool YL_FileInfo::IsAttrSystem( const string& strFilePath )
{	
	DWORD  m_dwFileAttr = ::GetFileAttributes( strFilePath.c_str() );
	return m_dwFileAttr != INVALID_FILE_ATTRIBUTES && (m_dwFileAttr & FILE_ATTRIBUTE_SYSTEM) > 0 ;
}

bool YL_FileInfo::IsAttrReadOnly( const char* pszFilePath )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	return IsAttrReadOnly(string(pszFilePath));
}

bool YL_FileInfo::IsAttrReadOnly( const string& strFilePath )
{	
	DWORD  m_dwFileAttr = ::GetFileAttributes( strFilePath.c_str() );
	return m_dwFileAttr != INVALID_FILE_ATTRIBUTES && (m_dwFileAttr & FILE_ATTRIBUTE_READONLY) > 0 ;
}

bool YL_FileInfo::IsAttrHidden( const char* pszFilePath )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	return IsAttrHidden(string(pszFilePath));
}

bool YL_FileInfo::IsAttrHidden( const string& strFilePath )
{	
	DWORD  m_dwFileAttr = ::GetFileAttributes( strFilePath.c_str() );
	return m_dwFileAttr != INVALID_FILE_ATTRIBUTES && (m_dwFileAttr & FILE_ATTRIBUTE_HIDDEN) > 0;
}

bool YL_FileInfo::GetFileSize( const char* pszFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	return GetFileSize(string(pszFilePath),pUnLowSize,pUnHighSize);
}

bool YL_FileInfo::GetFileSize( const string& strFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize )
{
	if( pUnLowSize == NULL )
	{
		return false;
	}

	struct __stat64 m_buf;
	if( bool bRet = !_stat64( strFilePath.c_str(), &m_buf ) )
	{
		*pUnLowSize = (unsigned int )(m_buf.st_size & 0xffffffff);
		if( pUnHighSize != NULL )
		{
			*pUnHighSize = (unsigned int)(m_buf.st_size >> 32);
		}
		return true;
	}
	
	return false;
}

bool YL_FileInfo::GetFileName( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	string strFileName;
	if( GetFileName( string(pszFilePath),strFileName) && strFileName.size() < unBufferLen )
	{
		strcpy( szBufferName,strFileName.c_str() );
		return true;
	}
	return false;	
}

bool YL_FileInfo::GetFileName( const string& strFilePath,string& strFileName )
{
	size_t n = strFilePath.find_last_of("\\");
	if( n == string::npos )
	{
		strFileName = strFilePath;
		return true;
	}
	strFileName = strFilePath.substr(n+1, strFilePath.length() - n);
	return true;
}


bool YL_FileInfo::GetFileNamePrefix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	
	string strFileNamePrefix;
	if( GetFileNamePrefix( string(pszFilePath),strFileNamePrefix) && strFileNamePrefix.size() < unBufferLen )
	{
		strcpy( szBufferName,strFileNamePrefix.c_str() );
		return true;

	}
	return false;	
}

bool YL_FileInfo::GetFileNamePrefix( const string& strFilePath,string& strFileName )
{
	size_t n = strFilePath.find_last_of("\\");
	if( n == string::npos )
	{
		n = 0;
	}
	else
	{
		n++;
	}

	size_t n2 = strFilePath.find_last_of(".");
	if( n2 == string::npos )
	{
		n2 = strFilePath.size() ;
	}

	strFileName = strFilePath.substr(n, n2 - n);
	return true;
}

bool YL_FileInfo::GetFileNameSuffix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen )
{
	if( pszFilePath == NULL )
	{
		return false;
	}

	string strFileNameSuffix;
	if( GetFileNameSuffix( string(pszFilePath),strFileNameSuffix) && strFileNameSuffix.size() < unBufferLen )
	{
		strcpy( szBufferName,strFileNameSuffix.c_str() );
		return true;

	}
	return false;	
}

bool YL_FileInfo::GetFileNameSuffix( const string& strFilePath,string& strFileName )
{
	size_t n = strFilePath.find_last_of(".");
	if( n == string::npos )
	{
		strFileName = "";
		return true;
	}
	else
	{
		n++;
	}	
	if( n >= strFilePath.size() )
	{
		strFileName = "";
		return true;
	}

	strFileName = strFilePath.substr(n);
	return true;

}


bool YL_FileInfo::GetFileDirPath( const char* pszFilePath,char szBufferDirPath[],unsigned int unBufferLen )
{
	if( pszFilePath == NULL )
	{
		return false;
	}
	char* pTmp =(char*) strrchr(pszFilePath,'\\');
	if( pTmp == NULL || *(pTmp+1) == '\0' )
	{
		return false;
	}
	
	if( ((unsigned int) (pTmp - pszFilePath))  >= unBufferLen )
	{
		return false;
	}
	strncpy(szBufferDirPath,pszFilePath,pTmp-pszFilePath);
	szBufferDirPath[pTmp-pszFilePath] = '\0';
	return true;	
}

bool YL_FileInfo::GetFileDirPath( const string& strFilePath,string& strFileDirPath )
{
	size_t n = strFilePath.find_last_of("\\");
	if( n == string::npos )
	{
		return false;
	}
	strFileDirPath = strFilePath.substr(0,n);
	return true;
}

string YL_FileInfo::GetFileContent( const string& strFileName )
{
	string res = "";
	FILE * f = fopen(strFileName.c_str(), "r");
	if (f)
	{
		char szBuffer[1024];
		memset( szBuffer,0,1024);
		while( fread( szBuffer,1,1023,f) )
		{			
			szBuffer[1023] = '\0';
			res += szBuffer;			
			memset( szBuffer,0,1024);
		}
		fclose(f);
	}
	return res;

}

bool YL_FileInfo::SetFileContent( const string& strFileName, const string& strContent )
{
	FILE * f = fopen(strFileName.c_str(), "w");
	if (f)
	{
		fprintf(f, strContent.c_str());
		fclose(f);
		return true;
	}
	else
	{
		return false;
	}
}