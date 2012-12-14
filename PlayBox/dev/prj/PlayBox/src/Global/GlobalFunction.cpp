#include "stdafx.h"
#include "GlobalFunction.h"
#include "YL_Encrypter.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"
#include "YL_FileZip.h"
#include "shlwapi.h"
#include "GlobalParas.h"

bool GlobalFunc::GetTempFilePath( const string& strSrcFile,string& strDesFile )
{
	static LONG s_iFileNum = 0;

	char szDesFilePath[512];
	if( YL_DirInfo::GetTempDir(szDesFilePath,512 ) == 0 )
	{		
		return false;
	}
	strcat( szDesFilePath,"\\" );

	lstrcat(szDesFilePath,TEXT(STR_TEMP_MEDIA_FOLDER));

	if( !YL_DirInfo::MakeDir( szDesFilePath ) )
	{		
		return false;
	}

	char szDir[512];
	strcpy( szDir,szDesFilePath );

	do
	{
		InterlockedIncrement( &s_iFileNum );
		string strSuffix;
		YL_FileInfo::GetFileNameSuffix( strSrcFile,strSuffix);
		sprintf(szDesFilePath,"%s\\%d.%s",szDir,s_iFileNum,strSuffix.c_str());

	}while( PathFileExists( szDesFilePath) );
	
	strDesFile = szDesFilePath;

	return true;
}

void GlobalFunc::CreateGBoxRunInStartUp( bool bAutoRun )
{
	char szPath[512];
	memset(szPath,0,512);
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_STARTUP,FALSE);

	string strRunPath;
	YL_StringUtil::Format( strRunPath, "%s\\%s", szPath, RUN_MAINEXE );
	
	if ( bAutoRun )
	{
		char szHome[512];
		CLhcImg::GetHomePath(szHome, 512);
		strcat(szHome,"\\run.exe");
		::CopyFile(szHome,strRunPath.c_str(),1);
	}
	else
	{
		::DeleteFile(strRunPath.c_str());
	}
}
//是否开机启动
bool GlobalFunc::IsAutoRun()
{
	char szPath[512];
	memset(szPath,0,512);
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_STARTUP,FALSE);
	strcat(szPath,"\\");
	strcat(szPath,RUN_MAINEXE);
	return YL_FileInfo::IsValid(szPath);
}

bool GlobalFunc::GetSkinPath( string& strPath )
{
	//homepath
	char szHome[512];
	CLhcImg::GetHomePath(szHome, 512);
	strPath = string( szHome ) + "\\Resources\\skin\\";
	return true;
}

bool GlobalFunc::GetSkinConfPath( string& strPath )
{
	//homepath
	char szHome[512];
	CLhcImg::GetHomePath(szHome, 512);
	//temppath
	char szTempPath[MAX_PATH];
	memset(szTempPath, 0, MAX_PATH);
	//::GetTempPath(MAX_PATH, szTempPath);  //临时文件夹
	//_snprintf( szTempPath, MAX_PATH, "%s\\Resources\\skin\\", szHome );  //和MainSkin.dat同目录
	SHGetSpecialFolderPath(NULL,szTempPath,CSIDL_APPDATA,FALSE);
	strcat( szTempPath, "\\PP\\" );
	YL_DirInfo::MakeDir( szTempPath );

	//mainskin.dat path
	string strSrc = string( szHome ) + "\\Resources\\skin\\MainSkin.dat";
	//mainskin.xml path
	string strDes = string( szTempPath ) + "MainSkin.xml";
	//////
	if( !YL_FileInfo::IsValid( strDes ) )
	{
		vector<string> vec;
		YL_CFileZip::UnCompress( strSrc, szTempPath, vec, true, "!@#$%^&*()" );	
	}
	strPath = strDes;
	return true;
}

bool GlobalFunc::DelSkinXml()
{
	//temppath
	char szTempPath[MAX_PATH];
	memset(szTempPath, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL,szTempPath,CSIDL_APPDATA,FALSE);
	//mainskin.xml path
	string strDes = string( szTempPath ) + "\\PP\\MainSkin.xml";
	string strDes1 = string( szTempPath ) + "\\PP\\MainSkin.xml.temp";

	::DeleteFile( strDes.c_str() );
	::DeleteFile( strDes1.c_str() );
	return true;
}