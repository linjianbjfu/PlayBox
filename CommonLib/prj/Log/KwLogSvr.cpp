#include "stdafx.h"
#include "KwLogSvr.h"
#include "../../common/globalparas.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/YL_Ini.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_Base64.h"
#include "../../common/YL_StringUtil.h"
#include "../../common/tools.h"
#include "../include/YL_HTTPRequest.h"
#include <time.h>
#include <fstream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <iostream>      
#include "../include/YL_ZlibZip.h"

#pragma   data_seg("ConfigVer")   
int		  g_iVersion = 0;
#pragma   data_seg()   
#pragma   comment(linker,   "/section:ConfigVer,RWS")   

using namespace std;

CKwLogSvr	g_Log;
YLMutex		m_Mutex;
string		g_strStartupType = "LH";


CKwLogSvr::CKwLogSvr()
{ 
#ifdef _YLLOG
	m_nLevel     = LOG_DEBUG;
#else
	m_nLevel	= LOG_KEYINFO;
#endif

	memset(m_szLogFile, 0, MAX_PATH);
	_snprintf(m_szLogFile, MAX_PATH-1, "%s", "act.log");

	char szID[128];
	memset(szID,0,128);
	//CLhcImg::GetUserID(szID,128);
	m_strID = szID;	

	m_iLogVer	= g_iVersion;
	UpdateConfig();

	return; 
}

CKwLogSvr::~CKwLogSvr()
{
}

#define STR_REG_MSG_DATE      "_LAST_DATE"
#define STR_REG_MSG_TIMES     "_TIMES"
#define STR_REG_MSG_TIME      "_LAST_TIME"
#define FORMAT_2              2
#define FORMAT_9              9
#define FORMAT_0              0
#define LOG_FILE_SIZE_LIMIT   (3000*1024)
#define LEFT_LOG_SIZE         (2800*1024)


struct SENDMSG
{
	string	svr;
	string	msg;
	bool	bNeedRet;
	unsigned char** ppUnBuffer;
	unsigned int*	pUnBufLen;
};

void SendMsg( SENDMSG* pSendMsg )
{
	//Base64 加密
	char* chInput = new char[BASE64_LENGTH(pSendMsg->msg.length())+1];
	YL_Base64Encode(chInput, pSendMsg->msg.c_str(), (int)pSendMsg->msg.length(), "langhua ");
	//send to log server
	YL_CHTTPRequest client;
	char szLogServer[SRC_LENGTH];
	memset(szLogServer, 0, SRC_LENGTH);

	if( pSendMsg->svr != "")
	{
		_snprintf(szLogServer, SRC_LENGTH-1, "%s", pSendMsg->svr.c_str());
	}
	else if(!CLhcImg::GetLogServer(szLogServer, SRC_LENGTH))
	{
		_snprintf(szLogServer, SRC_LENGTH-1, "%s", "http://115.182.59.18");
	}

	char szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	//CLhcImg::GetUserID( szID, INT_LENGTH );
	//CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	//CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	string strURL;
	YL_StringUtil::Format( strURL, "%s/ulog?type=ilog&ver=%s&id=%s&source=%s",
		szLogServer,
		szVersion,
		szID,
		szInstallSRC );

	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "Set Log Server End.");

	if(!client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_POST, 10*1000, (unsigned char*)chInput, strlen(chInput)))
	{
		YL_Log(LOGSVR_LOG_FILE, LOG_WARNING, "LogSvr", "Send Msg to Server Request Error!");
	}

	delete[] chInput;
	if ( pSendMsg->bNeedRet )
	{
		long reslen = 0;
		unsigned char *pContent = client.GetContent(reslen);
		//得不到配置文件
		if(pContent != NULL && reslen > 0)
		{
			*(pSendMsg->ppUnBuffer) = new unsigned char[ reslen ];
			memcpy( *(pSendMsg->ppUnBuffer),pContent,reslen );
			*(pSendMsg->pUnBufLen)	= reslen;
		}	
	}
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "Send Msg to Server Out");
	return ;
}

DWORD WINAPI SendMsg_Thread(void*ppar)
{
	SENDMSG* pSendMsg = (SENDMSG*)ppar;
	SendMsg( pSendMsg );

	delete pSendMsg;
	return 0;
}

bool SendMsgToServer(const string& svr, const string& msg,bool bNeedRet,unsigned char** ppUnBuffer,unsigned int* pUnBufLen )
{	
	SENDMSG* pSendMsg = new SENDMSG;

	pSendMsg->svr		= svr;
	pSendMsg->msg		= msg;
	pSendMsg->bNeedRet	= bNeedRet;
	pSendMsg->ppUnBuffer= ppUnBuffer;
	pSendMsg->pUnBufLen	= pUnBufLen;

	if( bNeedRet )
	{
		SendMsg( pSendMsg );
		delete pSendMsg;
	}
	else
	{
		CloseHandle( CreateThread( NULL,NULL,SendMsg_Thread,pSendMsg,0,NULL) );
	}
	return true;
}

HRESULT CKwLogSvr::Init(const string& strModule)
{
	//初始化注册表路径失败
#if defined _TEST
	_snprintf(LOGSVR_LOG_FILE, MAX_PATH-1, "%s", "log.log");
#else 
	memset(LOGSVR_LOG_FILE, 0, MAX_PATH);
#endif
	memset(m_szModuleName, 0, VERSION_LENGTH);
	_snprintf(m_szModuleName, VERSION_LENGTH-1, "%s", strModule.c_str());
	//创建Log Service的日志文件
	CHAR szPath[MAX_PATH];
	if(CLhcImg::GetHomePath(szPath, MAX_PATH))
	{
		//创建注册表子键
		//改为文件存储，无需创建
		//YL_RegInfo::CreateKey(HKEY_PLAYBOX_ROOT, string(STR_REG_SOFT) + "\\LOGMSG");
	}else
	{
		return E_FAIL;
	}
	//初始化Msg文件路径	
	YL_DirInfo::MakeDir(string(szPath) + "\\log");

	//如果本地日志文件太大 进行裁减
	unsigned int dwSizeHigh = 0, dwSizeLow = 0;
	if( YL_FileInfo::IsValid(m_szLogFile) && YL_FileInfo::GetFileSize(m_szLogFile, &dwSizeLow, &dwSizeHigh) )
	{
		if( dwSizeHigh > 0 || dwSizeLow > LOG_FILE_SIZE_LIMIT )
		{
			CutLogFile(m_szLogFile, LEFT_LOG_SIZE);
		}
	}

	UpdateConfig();

	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "Init LogSvr Success.");
	return S_OK;
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


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	
	string strDirName = sSourceFolder;
	hFind = FindFirstFile((strDirName + "\\*").c_str(), &FindFileData);	

	string strSrc,strDes;

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	
	do
	{
		if( strcmp(FindFileData.cFileName,".") == 0 || strcmp(FindFileData.cFileName,"..") == 0 )
		{
			continue;
		}

		if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{		

		}
		else
		{
			strSrc = sSourceFolder;
			strSrc += FindFileData.cFileName;

			strDes = sDestFolder;
			strDes += FindFileData.cFileName;

			DoFileCopy( strSrc.c_str(),strDes.c_str() );
		}
	}
	while( FindNextFile(hFind, &FindFileData) == TRUE);


	FindClose( hFind );	
}


#define MAX_LOG_FILE_CONTENT 1024*1024*10


bool Encryption( char* szSrc, unsigned int iSrclength, char** szDest )
{
	bool bRet = false;
	try{
		int i = BASE64_LENGTH(iSrclength)+1;
		*szDest = new char[BASE64_LENGTH(iSrclength)+1];//Base64::Encode((unsigned char*) pTemp,iSrclength);
		if(*szDest != NULL)
		{
			memset(*szDest, 0, BASE64_LENGTH(iSrclength)+1);
			if(YL_Base64Encode(*szDest, szSrc, iSrclength, "langhua ")>0)
				bRet = true;
		}
		int  iLen = strlen( *szDest );
	}
	catch(...)
	{
		return false;
	}
	return bRet;
}

HRESULT CKwLogSvr::UpdateConfig()
{
	//读取实时消息配置
	char szTemp[512];
	memset( szTemp, 0, 512 );
	unsigned int num = YL_EncFileReg::GetInt( "LogMsg", "MsgNum", 0 );
	if(num > 0)
	{
		m_MsgMap.clear();
	}	

	string strLog, strName, strKey;
	char index[NUM_LENGTH], szValue[SRC_LENGTH];
	for(unsigned int i=0; i<num; i++)
	{
		struct RealTimeMsg RealMsg;
		memset(index, 0, NUM_LENGTH);
		memset(szValue, 0, SRC_LENGTH);
		strKey = string("MsgName") + itoa(i, index, 10);

	
		memset( szValue, 0, SRC_LENGTH );
		YL_EncFileReg::GetString("LogMsg", strKey.c_str(), szValue, SRC_LENGTH-1);
		strName = szValue;

		strKey = string("MsgInterval") + itoa(i, index, 10);
		//获取消息发送时间间隔,缺省为1天
		RealMsg.nInterval = YL_EncFileReg::GetInt("LogMsg", strKey.c_str(), 24*60*60);

		strKey = string("MsgDailyTimes") + itoa(i, index, 10);
		//获取消息每天发送次数限制,缺省为1
		RealMsg.nDailyTimes = YL_EncFileReg::GetInt("LogMsg", strKey.c_str(), 1);

		strKey = string("MsgSvr") + itoa(i, index, 10);
		YL_EncFileReg::GetString("LogMsg", strKey.c_str(), szValue, SRC_LENGTH-1);
		RealMsg.strLogSvr = szValue;

		pair<string, RealTimeMsg> pr(strName, RealMsg);
		m_MsgMap.insert(pr);
	}

	m_bNeedLogUserAct = (YL_EncFileReg::GetInt("LogMsg", "userlog", 0) == 1);
	//update log level
#ifndef _YLLOG
	char szbuffer[32];
	memset( szbuffer,32,0 );
	YL_EncFileReg::GetString( "LogMsg","realloglevel",szbuffer,32 );	
	szbuffer[31] = '\0';

	if( strcmp(szbuffer,"debug") == 0 )
	{
		m_nLevel = LOG_DEBUG;
	}
	else if( strcmp( szbuffer,"notice") == 0 )
	{
		m_nLevel = LOG_NOTICE;
	}
	else
	{
		m_nLevel = LOG_KEYINFO;		
	}

#endif
	//update id
	char szID[128];
	memset(szID,0,128);
	//CLhcImg::GetUserID(szID,128);
	m_strID = szID;	

	return S_OK;
}

bool CKwLogSvr::RealTimeMsgControl(const string& strMsgName)
{
	//如果不是实时消息 卸载消息必然是实时消息
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl In.");
	if(m_MsgMap.find(strMsgName) == m_MsgMap.end())
	{
		YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl Out1.");
		return false;
	}
	//检测发送时间间隔
	__time64_t time;
	unsigned int lastdate = 0, date = 0, dwTimes = 0, dwLastTime = 0, dwCurrentTime = 0;
	dwCurrentTime = (DWORD)_time64(&time);
	date = (DWORD)(time/(60 * 60 * 24));

	if( !YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIME, dwLastTime) )
	{
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIME, dwCurrentTime);
	}
	else	
	{
		unsigned int interval = m_MsgMap.find(strMsgName)->second.nInterval * 60;
		if(dwCurrentTime - dwLastTime < interval)
		{
			if(dwCurrentTime < dwLastTime)
			{
				YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIME, dwCurrentTime);
			}
			YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl Out2.");
			return false;
		}
		else
		{
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIME, dwCurrentTime);
		}
	}
	//检测发送次数,更新发送日期
	if( !YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_DATE, lastdate) )
	{
		lastdate = 0;
	}

	//消息记录不是今天,重新开始计数
	if(date != lastdate)
	{
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_DATE, date);
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIMES, 0);
	}

	if( !YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIMES, dwTimes) )
	{
		dwTimes = 0;
	}

	dwTimes++;
	if(dwTimes > m_MsgMap.find(strMsgName)->second.nDailyTimes)
	{
		YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl Out3.");
		return false;
	}
	else
	{
		if( !YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_SOFT_MSG, strMsgName + STR_REG_MSG_TIMES, dwTimes) )
		{
			YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl Out4.");
			return false;
		}
		YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "RealTimeMsgControl Out5.");
		return true;
	}
}

string CKwLogSvr::FormatMsg(int nMsgType, const string& msg)
{
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "FormatMsg In.");
	//format <SRC:$ver|ACT:msg{$src}|U:$id>
	string strFMsg, str;
	char szID[VERSION_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	//CLhcImg::GetUserID(szID, VERSION_LENGTH);
	//CLhcImg::GetSoftwareVersion(szVersion, VERSION_LENGTH);
	//CLhcImg::GetInstallSRC(szInstallSRC, SRC_LENGTH);
	str = string("SRC:") + szVersion + "|ACT:" + msg + "{" + szInstallSRC + "}" + "|U:" + szID;
	//str = string("|ACT:") + msg + "{" + szInstallSRC + "}" + "|U:" + szID;
	switch(nMsgType)
	{
	case FORMAT_0:
		strFMsg = "|ACT:" + msg;// + "|SRC:" + szVersion;
		break;
	case FORMAT_2:
		strFMsg = string("2%09<") + str + ">";
		break;
	case FORMAT_9:
		strFMsg = string("9%09<") + str + ">";
		break;
	}
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "FormatMsg Out.");
	return strFMsg;
}

HRESULT CKwLogSvr::LogMsg(const string& strAct, const string& strmsg)
{
	string msg, strMsg;

	if(strmsg == "")
		msg = strAct + "|S:" + m_szModuleName + "|STARTTYPE:" + g_strStartupType;
	else
		msg = strAct + "|S:" + m_szModuleName + "|STARTTYPE:" + g_strStartupType + "|" + strmsg;


	strMsg = FormatMsg(FORMAT_0, msg);

	//记录本地日志
	YL_Log(m_szLogFile, LOG_KEYINFO, "KNF", "%s", strMsg.c_str() );

	return S_OK;
}

HRESULT CKwLogSvr::LogRealMsg(const string& strAct, const string& strmsg,bool bNeedRet,unsigned char** ppUnBuffer,unsigned int* pUnBufLen)
{
	HRESULT hRet = S_OK;
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "LogMsg In.");
	string msg, strMsg;
	if(strmsg == "")
		msg = strAct + "|S:" + m_szModuleName + "|STARTTYPE:" + g_strStartupType;
	else
		msg = strAct + "|S:" + m_szModuleName + "|STARTTYPE:" + g_strStartupType + "|" + strmsg;

	if(RealTimeMsgControl(strAct))
	{
		strMsg = FormatMsg(FORMAT_2, msg);
		string svr;
		if(m_MsgMap.find(strAct) == m_MsgMap.end())
		{
			svr = "";
		}
		else
		{
			svr = m_MsgMap.find(strAct)->second.strLogSvr;
		}
		//发送实时消息
		if( !SendMsgToServer(svr, strMsg,bNeedRet,ppUnBuffer,pUnBufLen ) )
		{
			hRet = S_FALSE;
		}
	}
	else
	{
		strMsg = FormatMsg(FORMAT_0, msg);
	}

	//记录本地日志
	YL_Log(m_szLogFile, LOG_KEYINFO, "KNF", "%s", strMsg.c_str());
	YL_Log(LOGSVR_LOG_FILE, LOG_KEYINFO, "LogSvr", "LogMsg Out.");
	return hRet;
}

bool CKwLogSvr::LogFilter(int level, const string & moduleName)
{
	if((unsigned int)level < m_nLevel)
	{
		return false;
	}
	return true;
}

void CKwLogSvr::Log(const string& strPath, int level, const string& moduleName, const string& msg)
{
#ifdef _DEBUG
	std::ofstream stream;
	SYSTEMTIME time;
	GetLocalTime(&time);
	string t = FormatTime(time);
	string strlevel;
	switch(level){
		case 0:
			strlevel = string("DEBUG   ");
			break;
		case 1: 
			strlevel = string("NOTICE  ");
			break;
		case 2:
			strlevel = string("WARNING ");
			break;
		case 3:
			strlevel = string("FATAL   ");
			break;
		case 4:
			strlevel = string("");
			break;
	}

	m_Mutex.Lock();
	stream.open(strPath.c_str(), ios::app | ios::out);

	string	strBlockName = "";
	YL_FileInfo::GetFileNamePrefix( strPath,strBlockName );

	stream	<< strlevel 
		<< "[" << m_strID << "]" 
		<< "[" << strBlockName <<  "]" 
		<<"[" << t << "]"
		<<"["<< moduleName << "]"
		<< ": " << msg << "\n";

	stream.close();

	m_Mutex.Unlock();
#endif
}

void CKwLogSvr::Log(const string& strPath, int level, const string& moduleName, const char* szFmt, ...)
{
	va_list argList;
	int len;
	char* buf;
	string strTemp;
	va_start( argList, szFmt );
	len = _vscprintf(szFmt, argList) + 1;
	buf = new char[len];
	vsprintf(buf, szFmt, argList);
	strTemp = string(buf, len - 1);
	delete[] buf;
	va_end( argList );
	Log(strPath,level,  moduleName ,strTemp);
}

string CKwLogSvr::FormatTime(const SYSTEMTIME& time)
{
	string strDay, strTime ;
	stringstream stream;
	char fillc = stream.fill('0');

	stream<< setw(2) << time.wMonth << "-" << setw(2) << time.wDay;
	stream>> strDay;
	stream.clear();
	// use two strings (strDay and strTime)here , because of the " " problem
	// when >> a stringtream to a string, >> operation will stop when barge up against " "
	stream<< setw(2) << time.wHour << ":" 
		<< setw(2) << time.wMinute << ":" 
		<< setw(2) << time.wSecond << "."
		<< setw(3) << time.wMilliseconds;
	stream>> strTime;
	strTime = strDay + " " + strTime;
	return strTime;
}


YLMutex::YLMutex(const char* strName)
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };

	m_hMutex = CreateMutex(&sa, FALSE, strName);
	if(m_hMutex == NULL){
		printf("Create Mutex error: %d \n", GetLastError());
	}
}

YLMutex::~YLMutex()
{
	CloseHandle(m_hMutex);
}

bool YLMutex::Lock()
{
	int nRet = WaitForSingleObject(m_hMutex, INFINITE);
	return nRet == WAIT_OBJECT_0 ? true : false;
}

bool YLMutex::Unlock()
{
	return ReleaseMutex(m_hMutex) ? true : false;
}

string GetLogDirPath()
{
	char  szBuffer[512];
	if( GetModuleFileName(NULL,szBuffer,512) == 0 )
	{
		return "";
	}

	string strDir(szBuffer);
	size_t nPos = strDir.find_last_of("\\");
	strDir = strDir.substr(0,nPos);
	strDir += "\\log";
	CreateDirectory(strDir.c_str(),NULL);
	return strDir;
}

void YL_Log(const string& strPath, int level, const string& moduleName, const char* szFmt, ...)
{
	if(strPath == "")
	{
		return;
	}

	if( g_Log.m_iLogVer != g_iVersion )
	{
		g_Log.UpdateConfig();
		g_Log.m_iLogVer	 = g_iVersion;
	}

	if( !g_Log.LogFilter( level,moduleName) )
	{
		return;
	}

	string strFilePath = strPath;
	if( strPath.find(":\\") != 1 )
	{	
		strFilePath = GetLogDirPath() + "\\" + strPath;
	}

	va_list argList;
	int len;
	char* buf;
	string strTemp;

	try
	{
		va_start( argList, szFmt );
		len = _vscprintf(szFmt, argList) + 1;
		buf = new char[len];
		vsprintf(buf, szFmt, argList);
		strTemp = string(buf, len - 1);
		delete[] buf;
		va_end( argList );
		g_Log.Log(strFilePath, level, moduleName, strTemp);
	}
	catch(...)
	{
		return;
	}
}

void	LogRealMsg(const string& strAct, const string& strmsg,bool bNeedRet,unsigned char** ppUnBuffer,unsigned int* pUnBufLen )
{		
	if( g_Log.m_iLogVer != g_iVersion )
	{
		g_Log.UpdateConfig();
		g_Log.m_iLogVer	 = g_iVersion;
	}

	g_Log.LogRealMsg(strAct,strmsg,bNeedRet,ppUnBuffer,pUnBufLen );
}

void	LogUserActMsg( const string& strAct, const string& strmsg)
{
	if( g_Log.m_iLogVer != g_iVersion )
	{
		g_Log.UpdateConfig();
		g_Log.m_iLogVer	 = g_iVersion;
	}

	if( !g_Log.m_bNeedLogUserAct )
	{
		return;
	}

	g_Log.LogRealMsg(strAct,strmsg);
}

void SetStartupType(const string& strType)
{
	g_strStartupType = strType;
}

void	LogFunctionalMsg( const string& strAct, const string& strmsg)
{
	LogRealMsg( strAct,strmsg );
}

void	UpdateConfig()
{
	g_iVersion++;
}

void	LogInit(const string& strModule)
{
	g_Log.Init(strModule);
}