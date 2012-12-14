#include "stdafx.h"
#include "ConfigParaDefault.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "../../Global/GlobalVariable.h"


//swf缺省存放目录
//剩余空间最大盘下创建目录，假如该目录无法创建，就在程序安装盘，假如仍然无法创建，就挨个试验哪个盘符能创建
string ConfigParaDefault::GetDefaultAPP_SWF_PATH()
{
	string strSwfPath;
	//1剩余空间最大的盘符下创建
	string strLargestTotalBytesDisk;
	if( YL_DirInfo::GetLargestFreeBytesDisk(strLargestTotalBytesDisk) )
	{
		if( strLargestTotalBytesDisk.length() >= 1 )
		{
			strLargestTotalBytesDisk = strLargestTotalBytesDisk.substr( 0, 1 );
		}
		YL_StringUtil::Format( strSwfPath, "%s:\\%s", strLargestTotalBytesDisk.c_str(), SWF_SAVE_PATH );
		//创建该目录
		if( YL_DirInfo::MakeDir( strSwfPath ) )
		{
			return strSwfPath;
		}	
	}
	//2程序安装盘符下创建
	char szPath[MAX_PATH];
	memset( szPath, 0, MAX_PATH );
	CLhcImg::GetHomePath( szPath, MAX_PATH );
	szPath[1] = '\0'; //szPath只剩下盘符
	YL_StringUtil::Format( strSwfPath, "%s:\\%s", szPath, SWF_SAVE_PATH );
	//创建该目录
	if( YL_DirInfo::MakeDir( strSwfPath ) )
	{
		return strSwfPath;
	}
	//3各个盘符轮流创建
	vector<string> vecDisk;
	YL_DirInfo::GetDiskNames( vecDisk );
	for( int i=0; i<vecDisk.size(); i++ )
	{
		string strDisk = vecDisk[i];
		if( strDisk.length() >= 1 )
		{
			strDisk = strDisk.substr( 0, 1 );
		}
		YL_StringUtil::Format( strSwfPath, "%s:\\%s", strDisk.c_str(), SWF_SAVE_PATH );
		//创建该目录
		if( YL_DirInfo::MakeDir( strSwfPath ) )
		{
			return strSwfPath;
		}
	}
	//4无法创建成功，只能返回空
	strSwfPath.clear();
	return strSwfPath;
}

//
// added by wang
//
DWORD ConfigParaDefault::GetDefaultAPP_Dwon_Number()
{
	DWORD dwDownNumber = 0;
	dwDownNumber =5;
	return  dwDownNumber;
}

string ConfigParaDefault::GetDefaultAPP_PicSave_Path()
{
	string strTemp;
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	char szDir[MAX_PATH] = { 0 };

	if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&pidl))) {
			// 如果成功返回true
			SHGetPathFromIDList(pidl, szDir);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	strTemp = szDir;
	return strTemp;
}

BOOL ConfigParaDefault::GetDefaultAPP_IS_Pop_Up_Dialog_Box()
{
	BOOL bRs = FALSE;
	// 询问用户是否每次下载都弹出下载路径选择窗口
	// 一般还是每次下载都弹出下载路径选择窗口吧?????
	bRs = TRUE;
	return bRs;
}

bool ConfigParaDefault::IsPathValid( string& strPath )
{
	YL_DirInfo::MakeDir( strPath );
	//是否能放置一个文件
    string strTmpFile = strPath + "\\cache.tmp";
	YL_FileInfo::SetFileContent( strTmpFile, "cache" );

	bool bValid = YL_FileInfo::IsValid( strTmpFile );
	::DeleteFile( strTmpFile.c_str() );
	return bValid;
}
