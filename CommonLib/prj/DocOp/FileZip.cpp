// FileZip.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../include/YL_FileZip.h"
#include "zip.h"
#include "unzip.h"
#include "../../common/YL_DirInfo.h"

//#pragma comment (lib, "shlwapi")
#include <shellapi.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


void DoFileCopy(const char* sSourceFile, const char* sDestFile)
{
	CopyFile(sSourceFile, sDestFile, FALSE);
	DeleteFile( sSourceFile );
}

void DoDirCopy(const char* sSourceFolder, const char* sDestFolder)
{
	if( GetFileAttributes(sSourceFolder) & FILE_ATTRIBUTE_DIRECTORY )
	{
		YL_DirInfo::MakeDir( sDestFolder );
	}
	else
	{
		DoFileCopy( sSourceFolder,sDestFolder );
	}
}


bool YL_CFileZip::Compress( const vector<string>& vecSrcFiles,const string& strDesFile, string password )
{
	HZIP hz = 0;
	if( password.length() == 0 )
	{
		hz = CreateZip( strDesFile.c_str(), 0);
	}else
	{
		hz = CreateZip( strDesFile.c_str(), password.c_str());
	}
	
	if( hz == NULL )
	{
		return false;
	}

	for( size_t idx = 0;idx < vecSrcFiles.size();idx++ )
	{
		if( ZipAdd( hz,vecSrcFiles[idx].c_str(),vecSrcFiles[idx].c_str()) != ZR_OK )
		{
			DeleteFile( strDesFile.c_str() );
			CloseZip(hz);
			return false;
		}
	}

	CloseZip(hz);
	return true;
}

bool YL_CFileZip::UnCompress( const string& strSrcFile,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password )
{
	HZIP hz = 0;
	if( password.length() == 0 )
	{
		hz = OpenZip( strSrcFile.c_str(), NULL );
	}else
	{
		hz = OpenZip( strSrcFile.c_str(), password.c_str() );
	}
	if( hz == NULL )
	{
		return false;
	}

	string strDirPath;
	
	if( bAllSucc )
	{	
		char szPath[512];		
		DWORD dwRetLen = GetTempPath(512, szPath);
		if( dwRetLen != 0 )
		{
			if( szPath[strlen(szPath)-1] == '\\' )
			{
				szPath[strlen(szPath)-1] = '\0';
			}
			strDirPath = szPath;			
		}
		else
		{
			return false;
		}
	}
	else
	{
		strDirPath = strUnZipDir;
	}


	bool bRet = true;
	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze);
	int numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		string UnZipPath = strDirPath + "\\" + ze.name;
		ZRESULT zResult = UnzipItem(hz, i, UnZipPath.c_str());
		if(zResult != ZR_OK)
		{
			DeleteFile(UnZipPath.c_str());			
			bRet = false;
			break;
		}
				
		if( bAllSucc )
		{
			DoDirCopy( UnZipPath.c_str(), (strUnZipDir+"\\"+ze.name).c_str() );
		}

		vecDesFiles.push_back( string(ze.name) );
	}
	CloseZip(hz);

	if( bRet && bAllSucc )
	{
		string strDirFilePath;

		for(size_t idx = 0;idx < vecDesFiles.size();idx++ )
		{
			strDirFilePath = strDirPath + "\\" + vecDesFiles[idx];

			DWORD dw = GetFileAttributes( strDirFilePath.c_str() );
			if( dw != INVALID_FILE_ATTRIBUTES )
			{
				if( dw & FILE_ATTRIBUTE_DIRECTORY )
				{
					YL_DirInfo::DeleteDir( strDirFilePath );
				}
				else
				{
					DeleteFile( strDirFilePath.c_str() );
				}
			}			
		}

	}	

	return bRet;
}

bool YL_CFileZip::UnCompress( void* pContent,unsigned int unContentLen,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password)
{
	HZIP hz = 0;
	if( password.length() == 0 )
	{
		hz = OpenZip( pContent,unContentLen,NULL );
	}else
	{
		hz = OpenZip( pContent,unContentLen,password.c_str() );
	}
	
	if( hz == NULL )
	{
		return false;
	}

	string strDirPath;

	if( bAllSucc )
	{	
		char szPath[512];		
		DWORD dwRetLen = GetTempPath(512, szPath);
		if( dwRetLen != 0 )
		{
			if( szPath[strlen(szPath)-1] == '\\' )
			{
				szPath[strlen(szPath)-1] = '\0';
			}
			strDirPath = szPath;			
		}
		else
		{
			return false;
		}
	}
	else
	{
		strDirPath = strUnZipDir;
	}


	bool bRet = true;
	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze);
	int numitems=ze.index;

	for (int i=0; i<numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		string UnZipPath = strDirPath + "\\" + ze.name;
		ZRESULT zResult = UnzipItem(hz, i, UnZipPath.c_str());
		if(zResult != ZR_OK)
		{
			DeleteFile(UnZipPath.c_str());			
			bRet = false;
			break;
		}
				
		if( bAllSucc )
		{
			DoDirCopy( UnZipPath.c_str(), (strUnZipDir+"\\"+ze.name).c_str() );
		}

		vecDesFiles.push_back( string(ze.name) );
	}
	CloseZip(hz);

	if( bRet && bAllSucc )
	{
		string strDirFilePath;

		for(size_t idx = 0;idx < vecDesFiles.size();idx++ )
		{
			strDirFilePath = strDirPath + "\\" + vecDesFiles[idx];

			DWORD dw = GetFileAttributes( strDirFilePath.c_str() );
			if( dw != INVALID_FILE_ATTRIBUTES )
			{
				if( dw & FILE_ATTRIBUTE_DIRECTORY )
				{
					YL_DirInfo::DeleteDir( strDirFilePath );
				}
				else
				{
					DeleteFile( strDirFilePath.c_str() );
				}
			}			
		}

	}	

	return bRet;
}