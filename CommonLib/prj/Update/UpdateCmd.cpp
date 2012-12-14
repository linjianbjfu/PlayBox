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

extern HANDLE		   g_EventGetConfig;
extern HANDLE		   g_EventUpConfigLog;
extern void GenUserID();
extern bool P2PDownloadFile( const Sign& sign );

//上传日志文件act.log
bool UploadFile(LPCTSTR lpszFile, LPCTSTR lpszURL)
{
	//load file
	DWORD dwFileSize = 0;
	WIN32_FIND_DATA filedata;
	char* szContent;
	YL_CHTTPRequest client;
	//如果日志文件不存在, 创建一个新文件
	HANDLE hFind = FindFirstFile(lpszFile, &filedata);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		//创建日志文件目录
		string strLogFilePath(lpszFile);
		string::size_type nTok = strLogFilePath.find_last_of("\\/");
		string strLogDir = strLogFilePath.substr(0, nTok);
		YL_DirInfo::MakeDir(strLogDir);
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "Can't Find act.log File." );
		//创建日志文件
		HANDLE  hFile = CreateFile(lpszFile, GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			YL_Log(UPDATE_LOG_FILE, LOG_FATAL, "upload log", "Create act.log Error.");
		}
		CloseHandle( hFile );
		szContent = new char[1];
		szContent[0] = '\0';
	}
	else
	{
		FindClose(hFind);
		dwFileSize = filedata.nFileSizeLow;
		//如果超过 300K, 只读取前 300K
		if( dwFileSize > UPDATE_LOG_FILE_LENGTH_LIMIT)
		{
			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "UploadFile Get too Large log File, Size is %d.", dwFileSize );
			dwFileSize = UPDATE_LOG_FILE_LENGTH_LIMIT;
		}
		szContent = new char[dwFileSize+1];
		if( dwFileSize > 0)
		{
			FILE*	file;
			//读出上传文件的内容
			file = fopen( lpszFile,"rb" );
			if( file == NULL )
			{
				return false;
			}
			BYTE*   byContent = new BYTE[dwFileSize];
			if( fread( byContent, dwFileSize, 1, file ) == 0 )
			{
				delete[] byContent;		
				fclose(file);
				return false;
			}
			fclose(file);

			memcpy(szContent, byContent, dwFileSize);
			delete[] byContent;
		}

		szContent[dwFileSize] = '\0';
	}

	char szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	CLhcImg::GetUserID( szID, INT_LENGTH );
	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	//发送给svr的日志，目前只是简单地只有日志内容
	string strStat = string( szContent );

	delete[] szContent;
	
	bool bRet = false;
	char* szBuffer  = NULL;
	unsigned char *pzip = NULL;
	//修改日志上传协议 
	char len[NUM_LENGTH], szTransfer[1024];
	memset(len, 0, NUM_LENGTH);
	memset( szTransfer, 0, 1024 );

	string strURL;
	YL_StringUtil::Format( strURL, "%s/ulog?type=clog&ver=%s&id=%s&source=%s",
		lpszURL,
		szVersion,
		szID,
		szInstallSRC );

	//压缩读出的日志
	uLong ziplen = compressBound((uLong)strStat.length());
	pzip = new unsigned char[ziplen];
	if(pzip == NULL)
	{
		goto cleanup;
	}

	if(compress(pzip, &ziplen, (byte*)strStat.c_str(), (uLong)strStat.length()) != Z_OK)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "Upload", "Compress LogFile error!" );
		goto cleanup;
	}

	strStat.clear();

	YL_Log("Uplog.txt",LOG_DEBUG,"Updata","Wait upConfigLog Event");
	WaitForSingleObject( g_EventUpConfigLog,600*1000 );
	YL_Log("Uplog.txt",LOG_DEBUG,"Updata","Wait upConfigLog Event---DONE");

	if(!client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_POST, 10*60*1000, pzip, (unsigned int)ziplen))
	{
		LogFunctionalMsg(STR_ACT_FATAL_ERROR, "TYPE:NET_ERR|DES:UPLOAD_LOG_FAIL");
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "Upload", "Upload Log:HTTP Request is error!" );
		goto cleanup;
	}

cleanup:
	if(pzip != NULL)
		delete[] pzip;

	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Upload", "Upload Log Complete.");
	return bRet;
}

bool SendLogToServer()
{
	//检测日志上传时间戳,默认是一天上传一次
	if(!CheckTimeStamp(STR_UPLOAD_LOG_KEY, DEFAULT_CHECK_UPDATE_INTERVAL))
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "SendLogToServer CheckTimeStamp Fail." );
		return false;
	}

	YL_Log( UPDATE_LOG_FILE, LOG_NOTICE, "UpdateCmd", "SendLogToServer CheckTimeStamp Success." );
	char szFilePath[MAX_PATH], svr[MAX_PATH];
	memset(szFilePath, 0, MAX_PATH);
	memset(svr, 0, MAX_PATH);
	if(!CLhcImg::GetUpLogServer(svr, MAX_PATH))
	{
		_snprintf(svr, MAX_PATH-1, "%s", "http://115.182.59.18");
	}
	CLhcImg::GetLogFilePath(szFilePath, MAX_PATH);

	string out = string(szFilePath) + ".out";
	::_unlink(out.c_str());
	rename(szFilePath, out.c_str());

	bool bUpSuc = UploadFile(out.c_str(), svr);
	return bUpSuc;
}

void RetryToGetConfig(string& strURL, long lCode, long& reslen, unsigned char **ppContent)
{
	static int nWaitTime = RETRY_CONFIG_WAIT_TIME;
	static int nTotalTime = 0;
	if(lCode < 0)
	{
		nWaitTime = RETRY_CONFIG_WAIT_TIME;
	}
	else
	{
		nWaitTime = nWaitTime * 2;
	}

	nTotalTime += nWaitTime;
	if(nTotalTime > 25*60*1000)
	{
		LogFunctionalMsg("DOWNCONF","FAILED:-1");
		return;
	}
	Sleep( nWaitTime );

	YL_CHTTPRequest client;
	bool bret = client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_GET, 10*1000, NULL, 0);
	if(bret)
	{
		LogFunctionalMsg("DOWNCONF","FAILED:1");
		unsigned char *p = NULL;
		p = client.GetContent(reslen);
		if(p != NULL && reslen > 0)
		{
			*ppContent = new unsigned char[reslen + 1];
			memcpy(*ppContent, p, reslen);
			(*ppContent)[reslen] = 0;
		}
	}
	else
	{
		long lCode = 0;
		client.GetReturnCode(lCode);
		LogFunctionalMsg("DOWNCONF","FAILED:0");
		RetryToGetConfig(strURL, lCode, reslen, ppContent);
	}
}

//去服务器获取配置文件，中间会wait 45秒
bool UpdateUserConfig(const char* szDesFilePath)
{
	//获取服务器上的配置文件
	bool bret = false;
	YL_CHTTPRequest client;
	char len[NUM_LENGTH], szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH], szTransfer[1024];
	CLhcImg::GetUserID( szID, INT_LENGTH );
	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	char szsvr[SRC_LENGTH];
	memset(szsvr, 0, SRC_LENGTH);
	CLhcImg::GetConfigServer(szsvr, SRC_LENGTH);
	string strURL = string(szsvr) + "/uc/s?m=";    //经过压缩的日志
	char* pEncodeTransfer = NULL;
	memset(len, 0, NUM_LENGTH);
	memset( szTransfer, 0, 1024 );
	_snprintf(szTransfer, 1023, "%s;%s,%s", szID, szVersion, szInstallSRC);
	
	//使用Base64加密字符串
	unsigned int length = (unsigned int)strlen(szTransfer);
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdateUserConfig", "Encrypt string in. szTransfer is %s, length is %u.", szTransfer, length);
	if(!MyMusic::Encryption(szTransfer, length, &pEncodeTransfer))
	{
		YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "UpdateUserConfig", "Encrypt string Error! string is %s, length is %u.", szTransfer, length);
	}
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdateUserConfig", "Encrypt string Success! string is %s, length is %u.", szTransfer, length);

	strURL = strURL + itoa((int)strlen(szTransfer), len, 10) + ";" + pEncodeTransfer;

	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "Wait GetConfig Event");
	WaitForSingleObject( g_EventGetConfig,45*1000 );
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "Wait GetConfig Event---DONE");

	bret = client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_GET, 10*1000, NULL, 0);
	if(pEncodeTransfer != NULL)
		delete[] pEncodeTransfer;

	long reslen = 0;
	unsigned char *pContent = NULL;
	if(!bret)
	{
		LogFunctionalMsg("DOWNCONF","FAILED:0");
		LogFunctionalMsg(STR_ACT_FATAL_ERROR, "TYPE:NET_ERR|DES:UPLOAD_LOG_FAIL");
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateUserConfig", "Get Config Request: HTTP Request is error!" );
		//return false;
		long lCode = 0;
		client.GetReturnCode(lCode);
		RetryToGetConfig(strURL, lCode, reslen, &pContent);
	}
	else
	{
		LogFunctionalMsg("DOWNCONF","FAILED:1");
		pContent = client.GetContent(reslen);
	}
	YL_Log( UPDATE_LOG_FILE, LOG_NOTICE, "UpdateUserConfig", "Get Config Request Success." );

	//得不到配置文件
	if(pContent == NULL || reslen <= 0)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateUserConfig", "Get Config Error: No Return Content!" );
		return false;
	}
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdateUserConfig", "result config content is %s", pContent);

	char *szContent = new char[reslen+1], *szConfig = new char[BASE64_LENGTH(reslen)+1];
	memset( szContent, 0, reslen+1 );
	memset( szConfig, 0, BASE64_LENGTH(reslen)+1 );

	memcpy(szContent, pContent, reslen);
	YL_Base64Decode(szConfig, szContent, "langhua ");
	if( strlen( szConfig ) > 10 ) 
	//配置文件至少得有超过10个的字符，因为svr端没有配置该版升级文件时，可能返回\n
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdateUserConfig", "Config file: content is %s", (char*)szConfig);
		FILE* f = fopen(szDesFilePath, "wb");
		if(f != NULL)
		{
			fprintf(f, "%s", (char*)szConfig);
			fflush(f);
			fclose(f);
			bret = true;
		}
	}
	if(szConfig != NULL)
		delete[] szConfig;
	if(szContent != NULL)
		delete[] szContent;
	if(bret == false)
		return false;

	UpdateConfigFile(szDesFilePath); //更新lhpcmd.img和lhplog.img
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "update 5.");
	return true;
}

void ChangeCurrentCmd(DWORD dwCmd)
{
	//dwCmd：1 检测升级状态
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_TIME_KEY, 0);
}

/**
 * 功能: 设置等待下一版升级标记
 * Download_File和Install_Soft命令多天执行失败的情况下,调用此函数
 * TestSpeed多天失败时,可以将命令设置为Check_Update,但不必等待下一版升级
*/
void SetWaitForNext()
{
	//当前命令设为检测升级
	ChangeCurrentCmd(1);
	//升级配置文件存放路径
	char szVersion[VERSION_LENGTH], szDesFilePath[MAX_PATH];
	memset(szVersion, 0, VERSION_LENGTH);
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH))
	{
		return;
	}
	//设置Wait_For_Next 
	YL_Ini ini(szDesFilePath);
	if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	{
		YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", szVersion);
	}
}

//检测配置文件中是否说有新的升级包
bool CheckUpdate()
{
	//设置升级配置文件存放路径
	char szDesFilePath[MAX_PATH], szConfigPath[MAX_PATH];
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH) || !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ))
	{
		//Fatal Error!
		return false;
	}

	//如果文件修改时间是当天的，则退出
	struct __stat64 buf;
	if(_stat64( szDesFilePath, &buf ) == 0)
	{
		__time64_t time;
		_time64(&time);
		unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwLast = (unsigned int)(buf.st_mtime/(60 * 60 * 24));
		if(dwDate == dwLast)
			return true;
	}

	//检测升级的时间间隔，默认为一天检查一次
	int  nRet = CheckCmdLimit(1);
	if(nRet == CMD_CHECK_FALSE)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate CheckCmdLimit FALSE." );
		return false;
	}

	//更新配置，目前只有升级服务器和实时日志列表,中间会wait 45秒
	bool bNewConf = UpdateUserConfig(szDesFilePath);
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "UpdateUserConfig Finished, is success: %d.", bNewConf);
	if(::IsWindow(g_hClientWnd))
	{
		// 取配置结束(不管是否有新配置) 通知界面更新广告
		LRESULT l = SendMessage(g_hClientWnd, WM_USER+13, 0, 0);
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "Send Conf Msg to GameBox, is success: %d.", l);
	}

	if(bNewConf == false)
		return false;

	YL_Ini ini(szDesFilePath);
	char szVersion[VERSION_LENGTH], szUpdateFile[VERSION_LENGTH];
	memset(szVersion, 0, VERSION_LENGTH);
	memset(szUpdateFile, 0, VERSION_LENGTH);
	//配置中有需要升级的版本号
	if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	{
		string strWaitVer;
		//处于 WAIT_FOR_NEXT 状态, 等待下一个版本
		if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", strWaitVer))
		{
			if(strWaitVer == szVersion)
			{
				YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate Should Wait for Next Version." );
				return false;
			}
		}
		// 更新升级提示delay天数
		UpdateSectionConfig("UpdateTips", szConfigPath, szDesFilePath);

		char szError[REG_STR_LENGTH];
		memset(szError, 0, REG_STR_LENGTH);
		_snprintf(szError, REG_STR_LENGTH-1, "TYPE:CHECK_UPDATE|NEW_V:%s", szVersion);
		LogFunctionalMsg(STR_UPDATE_MSG, szError);
		return true;
	}

	YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate Can't Get Update Version from DesFile." );
	return false;
}
//TestSpeed中调用的测试线程
void TestServer(void *param)
{
	vector<string> *pVec = (vector<string> *)param;
	string url;
	YL_CHTTPRequest client;
	client.SelectFastServer((*pVec), url);
	(*pVec).clear();
	(*pVec).push_back(url);
}

//对升级服务器测试，把速度最快的svr写入配置文件
bool TestSpeed()
{
	//读取服务器列表
	char szConfigPath[MAX_PATH];
	if( !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ) )
		return false;

	vector<string> vecUpdateServer;
	char szServer[SRC_LENGTH];
	YL_Ini ini(szConfigPath);

	vecUpdateServer.clear();
	//读取升级服务器列表
	unsigned int num = ini.GetUInt("UpdateServerList", "svrnum", 0);
	for(unsigned int i=0; i<num; i++)
	{
		char index[NUM_LENGTH];
		itoa(i, index, 10);
		string strKey = string("updatesvr") + index;
		memset(szServer, 0, SRC_LENGTH);
		ini.GetString("UpdateServerList", strKey.c_str(), szServer, SRC_LENGTH-1);
		if(strlen(szServer) > 0)
			vecUpdateServer.push_back(string(szServer));
	}

	//测试服务器速度
	HANDLE hUpdateTest = NULL;
	if(vecUpdateServer.size() > 1)
		hUpdateTest = (HANDLE)_beginthread(TestServer, 0, &vecUpdateServer);

	if(hUpdateTest != NULL)
		WaitForSingleObject(hUpdateTest, INFINITE);

	//测试结果写入配置文件
	//测试 update servers
	if(vecUpdateServer.size() > 0 && vecUpdateServer[0].length() > 0)
	{
		char szConfigPath[MAX_PATH];
		if( !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ) )
			return false;
		WritePrivateProfileString("UpdateServer", "updatesvr", vecUpdateServer[0].c_str(), szConfigPath);
	}
	return true;
}

//对升级服务器测速，选其中速度最快的，下载升级包
bool DownloadFile()
{
	char szFileName[VERSION_LENGTH], szHomePath[MAX_PATH], szTempConfigPath[MAX_PATH];
	string strFilePath;
	memset(szFileName, 0, VERSION_LENGTH); 
	//设置目标路径
	if(!CLhcImg::GetTempConfigPath(szTempConfigPath, MAX_PATH))
	{
		return false;
	}
	if(!CLhcImg::GetHomePath(szHomePath, MAX_PATH))
	{
		return false;
	}

	YL_Ini ini(szTempConfigPath);
	if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) <= 0)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile Can't Get Update File Name from Temp Config." );
		return false;
	}
	//检测下载天数和次数
	int  nRet = CheckCmdLimit(3);
	if(nRet == CMD_CHECK_FAIL)
	{
		//设置Wait_For_Next 
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile CheckCmdLimit Fail." );
		SetWaitForNext();
		return false;
	}
	else if(nRet == CMD_CHECK_FALSE)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "DownloadFile CheckCmdLimit FALSE." );
		return false;
	}

	string strDir = string(szHomePath) + "\\update";
	YL_DirInfo::MakeDir( strDir );
	strFilePath = string(szHomePath) + "\\update\\" + szFileName;
	// http升级
	// 测速选中最快的升级服务器
	TestSpeed();
	// 开始下载文件
	LogFunctionalMsg("DOWNLOAD_START", "");
	unsigned int nFileSize = 0;
	YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT, nFileSize);
	YL_CHTTPDownFile downfile;
	if(!CLhcImg::GetUpdateServer(szTempConfigPath, MAX_PATH))
	{
		return false;
	}
	string url = string(szTempConfigPath) + "/" + szFileName;
	if(downfile.DownloadFile(url, strFilePath, &nFileSize))
	{
		LogFunctionalMsg("DOWNLOAD_SUCCESS", "");
		return true;
	}
	else
	{
		char szError[REG_STR_LENGTH];
		memset(szError, 0, REG_STR_LENGTH);
		_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:DOWNN_FAIL|%s", szFileName);
		LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT, nFileSize);
	}
	return false;
}

/**
* 向服务器发送用户的本地一些软件信息
*/
BOOL SendPlayerInfo()
{
	//得到version
	char szVersion[VERSION_LENGTH];
	memset(szVersion, 0, VERSION_LENGTH);	
	if(!CLhcImg::GetSoftwareVersion(szVersion, VERSION_LENGTH))
	{
		_snprintf(szVersion, VERSION_LENGTH-1, "%s", "0");
	}
	char szInstallSRC[SRC_LENGTH];
	memset(szInstallSRC, 0, SRC_LENGTH);
	if(!CLhcImg::GetInstallSRC(szInstallSRC, SRC_LENGTH))
	{
		_snprintf(szInstallSRC, SRC_LENGTH-1, "%s", "0");
	}
	string strStat = string("2%09<SRC:") + szVersion + "|ACT:PLAYER{";

	//得到操作系统语言种类
	//初始化文字资源
	string languageName;
	LCID ulcid = GetUserDefaultLCID();
	//0x0804 Chinese PRC 
	//0x0404 Chinese  Taiwan 
	//0x0409 English United States 
	//0x1004 Chinese Singapore 
	switch(ulcid)
	{
	case 0x0804:
		languageName = "L:Ch.PRC";
		break;
	case 0x0404:
		languageName = "L:Ch.TW";
		break;
	case 0x0409:
		languageName = "L:En.US";
		break;
	case 0x1004:
		languageName = "L:Ch.Sp";
		break;
	default:
		languageName = "L:Other";
		break;
	}
	char chulcid[16];
	memset(chulcid, 0, 16);
	_snprintf(chulcid, 15, "%x", ulcid);
	strStat = strStat+ "|"+languageName+":"+string(chulcid);
	//得到用户使用的杀毒软件列表
	strStat += "|" + GetAntiVursSofts();
	//得到userid
	char szID[INT_LENGTH];
	memset(szID, 0, INT_LENGTH);
	if(!CLhcImg::GetUserID(szID, INT_LENGTH))
	{
		_snprintf(szID, INT_LENGTH-1, "%s", "0");
	}
	strStat = strStat+ "}|U:"+szID+">";
	//日志记录在本地
	LogFunctionalMsg("PlayerInfo", strStat.c_str());
	
	return TRUE;
}

//检查升级安装是否成功
//如果成功的话，会记录成功的日志，删除注册表有些项，发送本机一些软件信息
bool CheckInstallState()
{
	string strInstall;
	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY, strInstall))
	{
		//安装脚本写该注册表，如果是因为播放器正在运行而安装失败，则写入PLAYER_RUNNING
		//如果安装成功，则写入Success
		if(strInstall != "PLAYER_RUNNING")
		{
			//发送安装状态和安装之前的版本号
			string strLastVer;
			YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, strLastVer);
			strInstall = strInstall + "|" + strLastVer;
			//发送安装状态 如果发送成功 删除安装标记
			LogFunctionalMsg("INSTALL_STATE", strInstall.c_str());

			//删除原来的版本号
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION);
			//删除安装标记
			YL_EncFileReg::DeleteValue( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY );
			//本地软件信息记录在本地
			SendPlayerInfo();

			//如果安装成功 回到检测升级状态 
			ChangeCurrentCmd(1);
			return true;
		}
	}
	return false;
}

/**
 * 功能: 安装下载到 update目录下的软件
 * 返回值: 安装成功返回 true;
 *         安装失败返回 false.
*/
bool InstallSoft()
{
	//如果上次安装成功
	if(CheckInstallState())
		return true;

	//检测安装天数和次数
	int  nRet = CheckCmdLimit(4);
	if(nRet == CMD_CHECK_FAIL)
	{
		//设置Wait_For_Next 
		SetWaitForNext();
		return false;
	}
	else if(nRet == CMD_CHECK_FALSE)
	{
		return false;
	}

	char szHomePath[MAX_PATH];
	if(CLhcImg::GetHomePath(szHomePath, MAX_PATH))
	{
		char szFileName[VERSION_LENGTH], szConfFilePath[MAX_PATH];
		memset(szFileName, 0, VERSION_LENGTH); 
		memset(szConfFilePath, 0, MAX_PATH);
		if(!CLhcImg::GetTempConfigPath(szConfFilePath, MAX_PATH))
		{
			return false;
		}

		//记录安装之前的版本号
		char szLastVersion[MAX_PATH];
		memset( szLastVersion, 0, MAX_PATH );
		CLhcImg::GetSoftwareVersion(szLastVersion, MAX_PATH);
		YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, szLastVersion);

		//运行安装文件
		YL_Ini ini(szConfFilePath);
		if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) > 0)
		{
			string strFileName = szFileName;
			string strBasePath = szHomePath;
			//如果文件不存在
			if( _access( (strBasePath + "\\update\\" + szFileName).c_str(), 0) == ENOENT )
			{
				char szError[REG_STR_LENGTH];
				memset(szError, 0, REG_STR_LENGTH);
				_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:NO_FILE|%s", szFileName);
				LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
				return false;
			}
			if(win32exec((strFileName + " /S").c_str(), (strBasePath + "\\update").c_str(), false) == NULL)
			{
				char szError[REG_STR_LENGTH];
				memset(szError, 0, REG_STR_LENGTH);
				_snprintf(szError, REG_STR_LENGTH-1, "TYPE:UPDATE_ERR|DES:RUN_FAIL|%s", szFileName);
				LogFunctionalMsg(STR_ACT_FATAL_ERROR, szError);
				return false;
			}
			return true;
		}
	}
	return false;
}

int SetUpdateTip(unsigned int &dwCmd)
{
	//如果不是静默安装, 安装由播放器执行, bho等待下一版
	char szDesFilePath[MAX_PATH], szFileName[MAX_PATH];
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH))
	{
		return 0;
	}

	YL_Ini ini(szDesFilePath);
	if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) > 0)
	{
		if(strstr(strupr(szFileName), "_SILENT") == NULL)
		{
			char szError[REG_STR_LENGTH];
			memset(szError, 0, REG_STR_LENGTH);
			_snprintf(szError, REG_STR_LENGTH-1, "TYPE:SET_INSTALL|F:%s", szFileName);
			LogFunctionalMsg(STR_UPDATE_MSG, szError);
			//设置标记 让播放器运行安装包
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_MB_NEW_LYRIC_VER);
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_NEW_LYRIC_VERSION);
			if(ini.GetUInt("Update", "mbupdate", 0) == 1)
			{
				YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_MB_NEW_LYRIC_VER, szFileName );
			}else
			{
				YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_NEW_LYRIC_VERSION, szFileName );
			}
			// 处理新版本介绍信息
			char szImgPath[MAX_PATH], szInfo[INI_SECTION_BUFFER_LENGTH];
			CLhcImg::GetHomePath(szImgPath, MAX_PATH);
			strcat(szImgPath, "\\res\\pic\\lhupd.img");

			YL_Ini upini(szImgPath);
			ini.GetString("NewestSoft", "intro", szInfo, INI_SECTION_BUFFER_LENGTH-1);
			upini.WriteString("NewestSoft", "intro", szInfo);
			//bho等待下一版
			ChangeCurrentCmd(1);
			dwCmd = 1;
			SetWaitForNext();
			return 1;
		}
		return 2;
	}else
	{
		return 0;
	}
}

bool ExecCurrentCmd()
{
	// g_szModule为空串表示上传最后一次日志
	if(g_szModule[0] == 0)
	{
		return SendLogToServer();
	}
	//设置升级配置文件存放路径
	char szDesFilePath[MAX_PATH];
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH)) //svr.img
	{
		return false;
	}

	//检查上次升级安装是否成功
	CheckInstallState();
	bool bNewConf = CheckUpdate(); //获取svr.img，将内容分至lhpcmd.img,lhplog.img，并判断是否有升级
									//中间会wait 45秒
	SendMessage(g_hClientWnd,(WM_USER+14),2,0);

	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "GetConfigFile--DONE");

	//上传客户端日志，一天传一次,上传前wait 10分钟
	SendLogToServer();

	SendMessage(g_hClientWnd,(WM_USER+14),3,0);
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "SendLogToServer Finished.");

	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "SendLogToServer Finished.");
	//读取当前命令
	unsigned int dwCmd = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd))
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "No Current Cmd Exist.");
		ChangeCurrentCmd(1);
		dwCmd = 1;
	}

	switch(dwCmd)
	{
	case 1:
		if(bNewConf)//CheckUpdate()) //check update 提前, 每天必须执行, 不受 dwCmd 控制
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "CheckUpdate Success.");
			ChangeCurrentCmd(2);
			dwCmd = 2;
		}else
		{
			YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "UpdaetCmd", "CheckUpdate Fail!");
			return false;
		}
	case 2:
		//TestSpeed()) //测速和下载合一,不再有单独测速
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "TestSpeed Success.");
			ChangeCurrentCmd(3);
			dwCmd = 3;
		}
	case 3:
		if(DownloadFile())
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "DownloadFile Success.");
			//删除breakpoint标记
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_DOWNLOAD_BREAKPOINT);
			ChangeCurrentCmd(4);
			dwCmd = 4;
			int n = SetUpdateTip(dwCmd);
			if(n <= 1)
				return (n == true);
		}else
		{
			YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "UpdaetCmd", "DownloadFile Fail.");
			return false;
		}
	case 4:
		if(InstallSoft())
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "InstallSoft Success.");
			ChangeCurrentCmd(1);
		}else
		{
			return false;
		}
	default:
		break;
	}
	return true;
}
