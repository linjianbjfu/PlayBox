#include "stdafx.h"
#include "ConfigParaDefault.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "../../Global/GlobalVariable.h"


//swfȱʡ���Ŀ¼
//ʣ��ռ�������´���Ŀ¼�������Ŀ¼�޷����������ڳ���װ�̣�������Ȼ�޷��������Ͱ��������ĸ��̷��ܴ���
string ConfigParaDefault::GetDefaultAPP_SWF_PATH()
{
	string strSwfPath;
	//1ʣ��ռ������̷��´���
	string strLargestTotalBytesDisk;
	if( YL_DirInfo::GetLargestFreeBytesDisk(strLargestTotalBytesDisk) )
	{
		if( strLargestTotalBytesDisk.length() >= 1 )
		{
			strLargestTotalBytesDisk = strLargestTotalBytesDisk.substr( 0, 1 );
		}
		YL_StringUtil::Format( strSwfPath, "%s:\\%s", strLargestTotalBytesDisk.c_str(), SWF_SAVE_PATH );
		//������Ŀ¼
		if( YL_DirInfo::MakeDir( strSwfPath ) )
		{
			return strSwfPath;
		}	
	}
	//2����װ�̷��´���
	char szPath[MAX_PATH];
	memset( szPath, 0, MAX_PATH );
	CLhcImg::GetHomePath( szPath, MAX_PATH );
	szPath[1] = '\0'; //szPathֻʣ���̷�
	YL_StringUtil::Format( strSwfPath, "%s:\\%s", szPath, SWF_SAVE_PATH );
	//������Ŀ¼
	if( YL_DirInfo::MakeDir( strSwfPath ) )
	{
		return strSwfPath;
	}
	//3�����̷���������
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
		//������Ŀ¼
		if( YL_DirInfo::MakeDir( strSwfPath ) )
		{
			return strSwfPath;
		}
	}
	//4�޷������ɹ���ֻ�ܷ��ؿ�
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
			// ����ɹ�����true
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
	// ѯ���û��Ƿ�ÿ�����ض���������·��ѡ�񴰿�
	// һ�㻹��ÿ�����ض���������·��ѡ�񴰿ڰ�?????
	bRs = TRUE;
	return bRs;
}

bool ConfigParaDefault::IsPathValid( string& strPath )
{
	YL_DirInfo::MakeDir( strPath );
	//�Ƿ��ܷ���һ���ļ�
    string strTmpFile = strPath + "\\cache.tmp";
	YL_FileInfo::SetFileContent( strTmpFile, "cache" );

	bool bValid = YL_FileInfo::IsValid( strTmpFile );
	::DeleteFile( strTmpFile.c_str() );
	return bValid;
}
