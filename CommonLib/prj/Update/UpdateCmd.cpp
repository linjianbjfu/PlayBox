#include "stdafx.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_StringUtil.h"
#include "../../common/YL_Ini.h"
#include "../../common/tools.h"
#include "../../common/MacroStr.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_Base64.h"
#include "../../include/YL_HTTPDownFile.h"
#include "../../include/YL_P2PDllExport.h"
#include "UserReg.h"
#include "Update.h"
#include "UpdateConf.h"
#include "CheckCmdLimit.h"
#include "PlayerConfig.h"
#include "Tlhelp32.h"
#include <time.h>
#include <string>
#include <process.h>
#include <io.h>
#include "zlib.h"
#include "ScanSoftWare.h"
#include <sys/stat.h>

using namespace std;

#define UPDATE_LOG_FILE_LENGTH_LIMIT     3000*1024
#define	RETRY_CONFIG_WAIT_TIME			 (1 * 60 * 1000)

#define STR_NEW_LYRIC_VERSION     "NEW_LYRIC"
#define STR_MB_NEW_LYRIC_VER	  "MB_NEW_LYRIC"
#define STR_DOWNLOAD_BREAKPOINT   "BreakPoint"

extern bool P2PDownloadFile( const Sign& sign );

//�ϴ���־�ļ�act.log
bool UploadFile(LPCTSTR lpszFile, LPCTSTR lpszURL)
{
//	//load file
//	DWORD dwFileSize = 0;
//	WIN32_FIND_DATA filedata;
//	char* szContent;
//	YL_CHTTPRequest client;
//	//�����־�ļ�������, ����һ�����ļ�
//	HANDLE hFind = FindFirstFile(lpszFile, &filedata);
//	if(hFind == INVALID_HANDLE_VALUE)
//	{
//		//������־�ļ�Ŀ¼
//		string strLogFilePath(lpszFile);
//		string::size_type nTok = strLogFilePath.find_last_of("\\/");
//		string strLogDir = strLogFilePath.substr(0, nTok);
//		YL_DirInfo::MakeDir(strLogDir);
//		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "Can't Find act.log File." );
//		//������־�ļ�
//		HANDLE  hFile = CreateFile(lpszFile, GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//		if( hFile == INVALID_HANDLE_VALUE )
//		{
//			YL_Log(UPDATE_LOG_FILE, LOG_FATAL, "upload log", "Create act.log Error.");
//		}
//		CloseHandle( hFile );
//		szContent = new char[1];
//		szContent[0] = '\0';
//	}
//	else
//	{
//		FindClose(hFind);
//		dwFileSize = filedata.nFileSizeLow;
//		//������� 300K, ֻ��ȡǰ 300K
//		if( dwFileSize > UPDATE_LOG_FILE_LENGTH_LIMIT)
//		{
//			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "UploadFile Get too Large log File, Size is %d.", dwFileSize );
//			dwFileSize = UPDATE_LOG_FILE_LENGTH_LIMIT;
//		}
//		szContent = new char[dwFileSize+1];
//		if( dwFileSize > 0)
//		{
//			FILE*	file;
//			//�����ϴ��ļ�������
//			file = fopen( lpszFile,"rb" );
//			if( file == NULL )
//			{
//				return false;
//			}
//			BYTE*   byContent = new BYTE[dwFileSize];
//			if( fread( byContent, dwFileSize, 1, file ) == 0 )
//			{
//				delete[] byContent;		
//				fclose(file);
//				return false;
//			}
//			fclose(file);
//			memcpy(szContent, byContent, dwFileSize);
//			delete[] byContent;
//		}
//		szContent[dwFileSize] = '\0';
//	}
//
//	char szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
//	YL_UserId::Get(YL_UserId::USER_ID, );
//	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
//	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );
//
//	//���͸�svr����־��Ŀǰֻ�Ǽ򵥵�ֻ����־����
//	string strStat = string( szContent );
//
//	delete[] szContent;
//	
//	bool bRet = false;
//	char* szBuffer  = NULL;
//	unsigned char *pzip = NULL;
//	//�޸���־�ϴ�Э�� 
//	char len[NUM_LENGTH], szTransfer[1024];
//	memset(len, 0, NUM_LENGTH);
//	memset( szTransfer, 0, 1024 );
//
//	string strURL;
//	YL_StringUtil::Format( strURL, "%s/ulog?type=clog&ver=%s&id=%s&source=%s",
//		lpszURL,
//		szVersion,
//		szID,
//		szInstallSRC );
//
//	//ѹ����������־
//	uLong ziplen = compressBound((uLong)strStat.length());
//	pzip = new unsigned char[ziplen];
//	if(pzip == NULL)
//	{
//		goto cleanup;
//	}
//
//	if(compress(pzip, &ziplen, (byte*)strStat.c_str(), (uLong)strStat.length()) != Z_OK)
//	{
//		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "Upload", "Compress LogFile error!" );
//		goto cleanup;
//	}
//
//	strStat.clear();
//
//	YL_Log("Uplog.txt",LOG_DEBUG,"Updata","Wait upConfigLog Event");
//	WaitForSingleObject( g_EventUpConfigLog,600*1000 );
//	YL_Log("Uplog.txt",LOG_DEBUG,"Updata","Wait upConfigLog Event---DONE");
//
//	if(!client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_POST, 10*60*1000, pzip, (unsigned int)ziplen))
//	{
//		LogFunctionalMsg(STR_ACT_FATAL_ERROR, "TYPE:NET_ERR|DES:UPLOAD_LOG_FAIL");
//		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "Upload", "Upload Log:HTTP Request is error!" );
//		goto cleanup;
//	}
//
//cleanup:
//	if(pzip != NULL)
//		delete[] pzip;
//	return bRet;
	return false;
}

bool SendLogToServer()
{
	////�����־�ϴ�ʱ���,Ĭ����һ���ϴ�һ��
	//if(!CheckTimeStamp(STR_UPLOAD_LOG_KEY, DEFAULT_CHECK_UPDATE_INTERVAL))
	//{
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "SendLogToServer CheckTimeStamp Fail." );
	//	return false;
	//}

	//YL_Log( UPDATE_LOG_FILE, LOG_NOTICE, "UpdateCmd", "SendLogToServer CheckTimeStamp Success." );
	//char szFilePath[MAX_PATH], svr[MAX_PATH];
	//memset(szFilePath, 0, MAX_PATH);
	//memset(svr, 0, MAX_PATH);
	//if(!CLhcImg::GetUpLogServer(svr, MAX_PATH))
	//{
	//	_snprintf(svr, MAX_PATH-1, "%s", "http://115.182.59.18");
	//}
	//CLhcImg::GetLogFilePath(szFilePath, MAX_PATH);

	//string out = string(szFilePath) + ".out";
	//::_unlink(out.c_str());
	//rename(szFilePath, out.c_str());

	//bool bUpSuc = UploadFile(out.c_str(), svr);
	//return bUpSuc;
	return false;
}

//��������ļ����Ƿ�˵���µ�������
bool CheckUpdate()
{
	////�������������ļ����·��
	//char szDesFilePath[MAX_PATH], szConfigPath[MAX_PATH];
	//if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH) || !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ))
	//{
	//	//Fatal Error!
	//	return false;
	//}

	////����ļ��޸�ʱ���ǵ���ģ����˳�
	//struct __stat64 buf;
	//if(_stat64( szDesFilePath, &buf ) == 0)
	//{
	//	__time64_t time;
	//	_time64(&time);
	//	unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwLast = (unsigned int)(buf.st_mtime/(60 * 60 * 24));
	//	if(dwDate == dwLast)
	//		return true;
	//}

	////���������ʱ������Ĭ��Ϊһ����һ��
	//int  nRet = CheckCmdLimit(1);
	//if(nRet == CMD_CHECK_FALSE)
	//{
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate CheckCmdLimit FALSE." );
	//	return false;
	//}

	////�������ã�Ŀǰֻ��������������ʵʱ��־�б�,�м��wait 45��
	//bool bNewConf = UpdateUserConfig(szDesFilePath);
	//YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "UpdateUserConfig Finished, is success: %d.", bNewConf);
	//if(::IsWindow(g_hClientWnd))
	//{
	//	// ȡ���ý���(�����Ƿ���������) ֪ͨ������¹��
	//	LRESULT l = SendMessage(g_hClientWnd, WM_USER+13, 0, 0);
	//	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "Send Conf Msg to GameBox, is success: %d.", l);
	//}

	//if(bNewConf == false)
	//	return false;

	//YL_Ini ini(szDesFilePath);
	//char szVersion[VERSION_LENGTH], szUpdateFile[VERSION_LENGTH];
	//memset(szVersion, 0, VERSION_LENGTH);
	//memset(szUpdateFile, 0, VERSION_LENGTH);
	////����������Ҫ�����İ汾��
	//if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	//{
	//	string strWaitVer;
	//	//���� WAIT_FOR_NEXT ״̬, �ȴ���һ���汾
	//	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", strWaitVer))
	//	{
	//		if(strWaitVer == szVersion)
	//		{
	//			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate Should Wait for Next Version." );
	//			return false;
	//		}
	//	}
	//	// ����������ʾdelay����
	//	UpdateSectionConfig("UpdateTips", szConfigPath, szDesFilePath);

	//	char szError[REG_STR_LENGTH];
	//	memset(szError, 0, REG_STR_LENGTH);
	//	_snprintf(szError, REG_STR_LENGTH-1, "TYPE:CHECK_UPDATE|NEW_V:%s", szVersion);
	//	LogFunctionalMsg(STR_UPDATE_MSG, szError);
	//	return true;
	//}
	return false;
}
//���������������٣�ѡ�����ٶ����ģ�����������
bool DownloadFile()
{
	//char szFileName[VERSION_LENGTH], szHomePath[MAX_PATH], szTempConfigPath[MAX_PATH];
	//string strFilePath;
	//memset(szFileName, 0, VERSION_LENGTH); 
	////����Ŀ��·��
	//if(!CLhcImg::GetTempConfigPath(szTempConfigPath, MAX_PATH))
	//{
	//	return false;
	//}
	//if(!CLhcImg::GetHomePath(szHomePath, MAX_PATH))
	//{
	//	return false;
	//}

	//YL_Ini ini(szTempConfigPath);
	//if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) <= 0)
	//{
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile Can't Get Update File Name from Temp Config." );
	//	return false;
	//}
	////������������ʹ���
	//int  nRet = CheckCmdLimit(3);
	//if(nRet == CMD_CHECK_FAIL)
	//{
	//	//����Wait_For_Next 
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile CheckCmdLimit Fail." );
	//	SetWaitForNext();
	//	return false;
	//}
	//else if(nRet == CMD_CHECK_FALSE)
	//{
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile CheckCmdLimit FALSE." );
	//	return false;
	//}

	//string strDir = string(szHomePath) + "\\update";
	//YL_DirInfo::MakeDir( strDir );
	//strFilePath = string(szHomePath) + "\\update\\" + szFileName;
	//// http����
	//// ����ѡ����������������
	//TestSpeed();
	//// ��ʼ�����ļ�
	//LogFunctionalMsg("DOWNLOAD_START", "");
	//unsigned int nFileSize = 0;
	//YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT, nFileSize);
	//YL_CHTTPDownFile downfile;
	//if(!CLhcImg::GetUpdateServer(szTempConfigPath, MAX_PATH))
	//{
	//	return false;
	//}
	//string url = string(szTempConfigPath) + "/" + szFileName;
	//if(downfile.DownloadFile(url, strFilePath, &nFileSize))
	//{
	//	LogFunctionalMsg("DOWNLOAD_SUCCESS", "");
	//	return true;
	//}
	//else
	//{
	//	char szError[REG_STR_LENGTH];
	//	memset(szError, 0, REG_STR_LENGTH);
	//	_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:DOWNN_FAIL|%s", szFileName);
	//	LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
	//	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT, nFileSize);
	//}
	return false;
}

//���������װ�Ƿ�ɹ�
//����ɹ��Ļ������¼�ɹ�����־��ɾ��ע�����Щ����ͱ���һЩ�����Ϣ
bool CheckInstallState()
{
	//string strInstall;
	//if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY, strInstall))
	//{
	//	//��װ�ű�д��ע����������Ϊ�������������ж���װʧ�ܣ���д��PLAYER_RUNNING
	//	//�����װ�ɹ�����д��Success
	//	if(strInstall != "PLAYER_RUNNING")
	//	{
	//		//���Ͱ�װ״̬�Ͱ�װ֮ǰ�İ汾��
	//		string strLastVer;
	//		YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, strLastVer);
	//		strInstall = strInstall + "|" + strLastVer;
	//		//���Ͱ�װ״̬ ������ͳɹ� ɾ����װ���
	//		LogFunctionalMsg("INSTALL_STATE", strInstall.c_str());

	//		//ɾ��ԭ���İ汾��
	//		YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION);
	//		//ɾ����װ���
	//		YL_EncFileReg::DeleteValue( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY );
	//		//���������Ϣ��¼�ڱ���
	//		SendPlayerInfo();

	//		//�����װ�ɹ� �ص��������״̬ 
	//		ChangeCurrentCmd(1);
	//		return true;
	//	}
	//}
	return false;
}

/**
 * ����: ��װ���ص� updateĿ¼�µ����
 * ����ֵ: ��װ�ɹ����� true;
 *         ��װʧ�ܷ��� false.
*/
bool InstallSoft()
{
	////����ϴΰ�װ�ɹ�
	//if(CheckInstallState())
	//	return true;

	////��ⰲװ�����ʹ���
	//int  nRet = CheckCmdLimit(4);
	//if(nRet == CMD_CHECK_FAIL)
	//{
	//	//����Wait_For_Next 
	//	SetWaitForNext();
	//	return false;
	//}
	//else if(nRet == CMD_CHECK_FALSE)
	//{
	//	return false;
	//}

	//char szHomePath[MAX_PATH];
	//if(CLhcImg::GetHomePath(szHomePath, MAX_PATH))
	//{
	//	char szFileName[VERSION_LENGTH], szConfFilePath[MAX_PATH];
	//	memset(szFileName, 0, VERSION_LENGTH); 
	//	memset(szConfFilePath, 0, MAX_PATH);
	//	if(!CLhcImg::GetTempConfigPath(szConfFilePath, MAX_PATH))
	//	{
	//		return false;
	//	}

	//	//��¼��װ֮ǰ�İ汾��
	//	char szLastVersion[MAX_PATH];
	//	memset( szLastVersion, 0, MAX_PATH );
	//	CLhcImg::GetSoftwareVersion(szLastVersion, MAX_PATH);
	//	YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, szLastVersion);

	//	//���а�װ�ļ�
	//	YL_Ini ini(szConfFilePath);
	//	if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) > 0)
	//	{
	//		string strFileName = szFileName;
	//		string strBasePath = szHomePath;
	//		//����ļ�������
	//		if( _access( (strBasePath + "\\update\\" + szFileName).c_str(), 0) == ENOENT )
	//		{
	//			char szError[REG_STR_LENGTH];
	//			memset(szError, 0, REG_STR_LENGTH);
	//			_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:NO_FILE|%s", szFileName);
	//			LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
	//			return false;
	//		}
	//		if(win32exec((strFileName + " /S").c_str(), (strBasePath + "\\update").c_str(), false) == NULL)
	//		{
	//			char szError[REG_STR_LENGTH];
	//			memset(szError, 0, REG_STR_LENGTH);
	//			_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:RUN_FAIL|%s", szFileName);
	//			LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
	//			return false;
	//		}
	//		return true;
	//	}
	//}
	return false;
}

bool ExecCurrentCmd()
{
	//// g_szModuleΪ�մ���ʾ�ϴ����һ����־
	//if(g_szModule[0] == 0)
	//	return SendLogToServer();
	////�������������ļ����·��
	//char szDesFilePath[MAX_PATH];
	//if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH)) //svr.img
	//	return false;

	////����ϴ�������װ�Ƿ�ɹ�
	//CheckInstallState();
	//bool bNewConf = CheckUpdate(); //��ȡsvr.img�������ݷ���lhpcmd.img,lhplog.img�����ж��Ƿ�������
	//								//�м��wait 45��
	//SendMessage(g_hClientWnd,(WM_USER+14),2,0);
	////�ϴ��ͻ�����־��һ�촫һ��,�ϴ�ǰwait 10����
	//SendLogToServer();
	////��ȡ��ǰ����
	//unsigned int dwCmd = 0;
	//if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd))
	//{
	//	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "No Current Cmd Exist.");
	//	ChangeCurrentCmd(1);
	//	dwCmd = 1;
	//}
	//switch(dwCmd)
	//{
	//	if(DownloadFile())
	//	{
	//		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "DownloadFile Success.");
	//		//ɾ��breakpoint���
	//		YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT);
	//		ChangeCurrentCmd(4);
	//		dwCmd = 4;
	//		int n = SetUpdateTip(dwCmd);
	//		if(n <= 1)
	//			return (n == true);
	//	}else
	//	{
	//		YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "UpdaetCmd", "DownloadFile Fail.");
	//		return false;
	//	}
	//case 4:
	//	if(InstallSoft())
	//	{
	//		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "InstallSoft Success.");
	//		ChangeCurrentCmd(1);
	//	}else
	//	{
	//		return false;
	//	}
	//default:
	//	break;
	//}
	return true;
}
