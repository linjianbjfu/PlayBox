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

//上传日志文件act.log
bool UploadFile(LPCTSTR lpszFile, LPCTSTR lpszURL)
{
//	//load file
//	DWORD dwFileSize = 0;
//	WIN32_FIND_DATA filedata;
//	char* szContent;
//	YL_CHTTPRequest client;
//	//如果日志文件不存在, 创建一个新文件
//	HANDLE hFind = FindFirstFile(lpszFile, &filedata);
//	if(hFind == INVALID_HANDLE_VALUE)
//	{
//		//创建日志文件目录
//		string strLogFilePath(lpszFile);
//		string::size_type nTok = strLogFilePath.find_last_of("\\/");
//		string strLogDir = strLogFilePath.substr(0, nTok);
//		YL_DirInfo::MakeDir(strLogDir);
//		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "Can't Find act.log File." );
//		//创建日志文件
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
//		//如果超过 300K, 只读取前 300K
//		if( dwFileSize > UPDATE_LOG_FILE_LENGTH_LIMIT)
//		{
//			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "UploadFile Get too Large log File, Size is %d.", dwFileSize );
//			dwFileSize = UPDATE_LOG_FILE_LENGTH_LIMIT;
//		}
//		szContent = new char[dwFileSize+1];
//		if( dwFileSize > 0)
//		{
//			FILE*	file;
//			//读出上传文件的内容
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
//	//发送给svr的日志，目前只是简单地只有日志内容
//	string strStat = string( szContent );
//
//	delete[] szContent;
//	
//	bool bRet = false;
//	char* szBuffer  = NULL;
//	unsigned char *pzip = NULL;
//	//修改日志上传协议 
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
//	//压缩读出的日志
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
	////检测日志上传时间戳,默认是一天上传一次
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

//检测配置文件中是否说有新的升级包
bool CheckUpdate()
{
	////设置升级配置文件存放路径
	//char szDesFilePath[MAX_PATH], szConfigPath[MAX_PATH];
	//if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH) || !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ))
	//{
	//	//Fatal Error!
	//	return false;
	//}

	////如果文件修改时间是当天的，则退出
	//struct __stat64 buf;
	//if(_stat64( szDesFilePath, &buf ) == 0)
	//{
	//	__time64_t time;
	//	_time64(&time);
	//	unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwLast = (unsigned int)(buf.st_mtime/(60 * 60 * 24));
	//	if(dwDate == dwLast)
	//		return true;
	//}

	////检测升级的时间间隔，默认为一天检查一次
	//int  nRet = CheckCmdLimit(1);
	//if(nRet == CMD_CHECK_FALSE)
	//{
	//	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate CheckCmdLimit FALSE." );
	//	return false;
	//}

	////更新配置，目前只有升级服务器和实时日志列表,中间会wait 45秒
	//bool bNewConf = UpdateUserConfig(szDesFilePath);
	//YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "UpdateUserConfig Finished, is success: %d.", bNewConf);
	//if(::IsWindow(g_hClientWnd))
	//{
	//	// 取配置结束(不管是否有新配置) 通知界面更新广告
	//	LRESULT l = SendMessage(g_hClientWnd, WM_USER+13, 0, 0);
	//	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "Send Conf Msg to GameBox, is success: %d.", l);
	//}

	//if(bNewConf == false)
	//	return false;

	//YL_Ini ini(szDesFilePath);
	//char szVersion[VERSION_LENGTH], szUpdateFile[VERSION_LENGTH];
	//memset(szVersion, 0, VERSION_LENGTH);
	//memset(szUpdateFile, 0, VERSION_LENGTH);
	////配置中有需要升级的版本号
	//if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	//{
	//	string strWaitVer;
	//	//处于 WAIT_FOR_NEXT 状态, 等待下一个版本
	//	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", strWaitVer))
	//	{
	//		if(strWaitVer == szVersion)
	//		{
	//			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate Should Wait for Next Version." );
	//			return false;
	//		}
	//	}
	//	// 更新升级提示delay天数
	//	UpdateSectionConfig("UpdateTips", szConfigPath, szDesFilePath);

	//	char szError[REG_STR_LENGTH];
	//	memset(szError, 0, REG_STR_LENGTH);
	//	_snprintf(szError, REG_STR_LENGTH-1, "TYPE:CHECK_UPDATE|NEW_V:%s", szVersion);
	//	LogFunctionalMsg(STR_UPDATE_MSG, szError);
	//	return true;
	//}
	return false;
}
//对升级服务器测速，选其中速度最快的，下载升级包
bool DownloadFile()
{
	//char szFileName[VERSION_LENGTH], szHomePath[MAX_PATH], szTempConfigPath[MAX_PATH];
	//string strFilePath;
	//memset(szFileName, 0, VERSION_LENGTH); 
	////设置目标路径
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
	////检测下载天数和次数
	//int  nRet = CheckCmdLimit(3);
	//if(nRet == CMD_CHECK_FAIL)
	//{
	//	//设置Wait_For_Next 
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
	//// http升级
	//// 测速选中最快的升级服务器
	//TestSpeed();
	//// 开始下载文件
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

//检查升级安装是否成功
//如果成功的话，会记录成功的日志，删除注册表有些项，发送本机一些软件信息
bool CheckInstallState()
{
	//string strInstall;
	//if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY, strInstall))
	//{
	//	//安装脚本写该注册表，如果是因为播放器正在运行而安装失败，则写入PLAYER_RUNNING
	//	//如果安装成功，则写入Success
	//	if(strInstall != "PLAYER_RUNNING")
	//	{
	//		//发送安装状态和安装之前的版本号
	//		string strLastVer;
	//		YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, strLastVer);
	//		strInstall = strInstall + "|" + strLastVer;
	//		//发送安装状态 如果发送成功 删除安装标记
	//		LogFunctionalMsg("INSTALL_STATE", strInstall.c_str());

	//		//删除原来的版本号
	//		YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION);
	//		//删除安装标记
	//		YL_EncFileReg::DeleteValue( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY );
	//		//本地软件信息记录在本地
	//		SendPlayerInfo();

	//		//如果安装成功 回到检测升级状态 
	//		ChangeCurrentCmd(1);
	//		return true;
	//	}
	//}
	return false;
}

/**
 * 功能: 安装下载到 update目录下的软件
 * 返回值: 安装成功返回 true;
 *         安装失败返回 false.
*/
bool InstallSoft()
{
	////如果上次安装成功
	//if(CheckInstallState())
	//	return true;

	////检测安装天数和次数
	//int  nRet = CheckCmdLimit(4);
	//if(nRet == CMD_CHECK_FAIL)
	//{
	//	//设置Wait_For_Next 
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

	//	//记录安装之前的版本号
	//	char szLastVersion[MAX_PATH];
	//	memset( szLastVersion, 0, MAX_PATH );
	//	CLhcImg::GetSoftwareVersion(szLastVersion, MAX_PATH);
	//	YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, szLastVersion);

	//	//运行安装文件
	//	YL_Ini ini(szConfFilePath);
	//	if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) > 0)
	//	{
	//		string strFileName = szFileName;
	//		string strBasePath = szHomePath;
	//		//如果文件不存在
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
	//// g_szModule为空串表示上传最后一次日志
	//if(g_szModule[0] == 0)
	//	return SendLogToServer();
	////设置升级配置文件存放路径
	//char szDesFilePath[MAX_PATH];
	//if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH)) //svr.img
	//	return false;

	////检查上次升级安装是否成功
	//CheckInstallState();
	//bool bNewConf = CheckUpdate(); //获取svr.img，将内容分至lhpcmd.img,lhplog.img，并判断是否有升级
	//								//中间会wait 45秒
	//SendMessage(g_hClientWnd,(WM_USER+14),2,0);
	////上传客户端日志，一天传一次,上传前wait 10分钟
	//SendLogToServer();
	////读取当前命令
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
	//		//删除breakpoint标记
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
