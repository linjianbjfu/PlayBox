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

//�ϴ���־�ļ�act.log
bool UploadFile(LPCTSTR lpszFile, LPCTSTR lpszURL)
{
	//load file
	DWORD dwFileSize = 0;
	WIN32_FIND_DATA filedata;
	char* szContent;
	YL_CHTTPRequest client;
	//�����־�ļ�������, ����һ�����ļ�
	HANDLE hFind = FindFirstFile(lpszFile, &filedata);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		//������־�ļ�Ŀ¼
		string strLogFilePath(lpszFile);
		string::size_type nTok = strLogFilePath.find_last_of("\\/");
		string strLogDir = strLogFilePath.substr(0, nTok);
		YL_DirInfo::MakeDir(strLogDir);
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "Can't Find act.log File." );
		//������־�ļ�
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
		//������� 300K, ֻ��ȡǰ 300K
		if( dwFileSize > UPDATE_LOG_FILE_LENGTH_LIMIT)
		{
			YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "upload log", "UploadFile Get too Large log File, Size is %d.", dwFileSize );
			dwFileSize = UPDATE_LOG_FILE_LENGTH_LIMIT;
		}
		szContent = new char[dwFileSize+1];
		if( dwFileSize > 0)
		{
			FILE*	file;
			//�����ϴ��ļ�������
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

	//���͸�svr����־��Ŀǰֻ�Ǽ򵥵�ֻ����־����
	string strStat = string( szContent );

	delete[] szContent;
	
	bool bRet = false;
	char* szBuffer  = NULL;
	unsigned char *pzip = NULL;
	//�޸���־�ϴ�Э�� 
	char len[NUM_LENGTH], szTransfer[1024];
	memset(len, 0, NUM_LENGTH);
	memset( szTransfer, 0, 1024 );

	string strURL;
	YL_StringUtil::Format( strURL, "%s/ulog?type=clog&ver=%s&id=%s&source=%s",
		lpszURL,
		szVersion,
		szID,
		szInstallSRC );

	//ѹ����������־
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
	//�����־�ϴ�ʱ���,Ĭ����һ���ϴ�һ��
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

//ȥ��������ȡ�����ļ����м��wait 45��
bool UpdateUserConfig(const char* szDesFilePath)
{
	//��ȡ�������ϵ������ļ�
	bool bret = false;
	YL_CHTTPRequest client;
	char len[NUM_LENGTH], szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH], szTransfer[1024];
	CLhcImg::GetUserID( szID, INT_LENGTH );
	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	char szsvr[SRC_LENGTH];
	memset(szsvr, 0, SRC_LENGTH);
	CLhcImg::GetConfigServer(szsvr, SRC_LENGTH);
	string strURL = string(szsvr) + "/uc/s?m=";    //����ѹ������־
	char* pEncodeTransfer = NULL;
	memset(len, 0, NUM_LENGTH);
	memset( szTransfer, 0, 1024 );
	_snprintf(szTransfer, 1023, "%s;%s,%s", szID, szVersion, szInstallSRC);
	
	//ʹ��Base64�����ַ���
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

	//�ò��������ļ�
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
	//�����ļ����ٵ��г���10�����ַ�����Ϊsvr��û�����øð������ļ�ʱ�����ܷ���\n
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

	UpdateConfigFile(szDesFilePath); //����lhpcmd.img��lhplog.img
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "update 5.");
	return true;
}

void ChangeCurrentCmd(DWORD dwCmd)
{
	//dwCmd��1 �������״̬
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, 0);
	YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_TIME_KEY, 0);
}

/**
 * ����: ���õȴ���һ���������
 * Download_File��Install_Soft�������ִ��ʧ�ܵ������,���ô˺���
 * TestSpeed����ʧ��ʱ,���Խ���������ΪCheck_Update,�����صȴ���һ������
*/
void SetWaitForNext()
{
	//��ǰ������Ϊ�������
	ChangeCurrentCmd(1);
	//���������ļ����·��
	char szVersion[VERSION_LENGTH], szDesFilePath[MAX_PATH];
	memset(szVersion, 0, VERSION_LENGTH);
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH))
	{
		return;
	}
	//����Wait_For_Next 
	YL_Ini ini(szDesFilePath);
	if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	{
		YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", szVersion);
	}
}

//��������ļ����Ƿ�˵���µ�������
bool CheckUpdate()
{
	//�������������ļ����·��
	char szDesFilePath[MAX_PATH], szConfigPath[MAX_PATH];
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH) || !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ))
	{
		//Fatal Error!
		return false;
	}

	//����ļ��޸�ʱ���ǵ���ģ����˳�
	struct __stat64 buf;
	if(_stat64( szDesFilePath, &buf ) == 0)
	{
		__time64_t time;
		_time64(&time);
		unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwLast = (unsigned int)(buf.st_mtime/(60 * 60 * 24));
		if(dwDate == dwLast)
			return true;
	}

	//���������ʱ������Ĭ��Ϊһ����һ��
	int  nRet = CheckCmdLimit(1);
	if(nRet == CMD_CHECK_FALSE)
	{
		YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate CheckCmdLimit FALSE." );
		return false;
	}

	//�������ã�Ŀǰֻ��������������ʵʱ��־�б�,�м��wait 45��
	bool bNewConf = UpdateUserConfig(szDesFilePath);
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "UpdateUserConfig Finished, is success: %d.", bNewConf);
	if(::IsWindow(g_hClientWnd))
	{
		// ȡ���ý���(�����Ƿ���������) ֪ͨ������¹��
		LRESULT l = SendMessage(g_hClientWnd, WM_USER+13, 0, 0);
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "Send Conf Msg to GameBox, is success: %d.", l);
	}

	if(bNewConf == false)
		return false;

	YL_Ini ini(szDesFilePath);
	char szVersion[VERSION_LENGTH], szUpdateFile[VERSION_LENGTH];
	memset(szVersion, 0, VERSION_LENGTH);
	memset(szUpdateFile, 0, VERSION_LENGTH);
	//����������Ҫ�����İ汾��
	if(ini.GetString("Update", "Version", szVersion, VERSION_LENGTH-1) > 0)
	{
		string strWaitVer;
		//���� WAIT_FOR_NEXT ״̬, �ȴ���һ���汾
		if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "WAIT_FOR_NEXT", strWaitVer))
		{
			if(strWaitVer == szVersion)
			{
				YL_Log( UPDATE_LOG_FILE, LOG_WARNING, "UpdateCmd", "CheckUpdate Should Wait for Next Version." );
				return false;
			}
		}
		// ����������ʾdelay����
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
//TestSpeed�е��õĲ����߳�
void TestServer(void *param)
{
	vector<string> *pVec = (vector<string> *)param;
	string url;
	YL_CHTTPRequest client;
	client.SelectFastServer((*pVec), url);
	(*pVec).clear();
	(*pVec).push_back(url);
}

//���������������ԣ����ٶ�����svrд�������ļ�
bool TestSpeed()
{
	//��ȡ�������б�
	char szConfigPath[MAX_PATH];
	if( !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ) )
		return false;

	vector<string> vecUpdateServer;
	char szServer[SRC_LENGTH];
	YL_Ini ini(szConfigPath);

	vecUpdateServer.clear();
	//��ȡ�����������б�
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

	//���Է������ٶ�
	HANDLE hUpdateTest = NULL;
	if(vecUpdateServer.size() > 1)
		hUpdateTest = (HANDLE)_beginthread(TestServer, 0, &vecUpdateServer);

	if(hUpdateTest != NULL)
		WaitForSingleObject(hUpdateTest, INFINITE);

	//���Խ��д�������ļ�
	//���� update servers
	if(vecUpdateServer.size() > 0 && vecUpdateServer[0].length() > 0)
	{
		char szConfigPath[MAX_PATH];
		if( !CLhcImg::GetConfigPath( szConfigPath, MAX_PATH ) )
			return false;
		WritePrivateProfileString("UpdateServer", "updatesvr", vecUpdateServer[0].c_str(), szConfigPath);
	}
	return true;
}

//���������������٣�ѡ�����ٶ����ģ�����������
bool DownloadFile()
{
	char szFileName[VERSION_LENGTH], szHomePath[MAX_PATH], szTempConfigPath[MAX_PATH];
	string strFilePath;
	memset(szFileName, 0, VERSION_LENGTH); 
	//����Ŀ��·��
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
	//������������ʹ���
	int  nRet = CheckCmdLimit(3);
	if(nRet == CMD_CHECK_FAIL)
	{
		//����Wait_For_Next 
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
	// http����
	// ����ѡ����������������
	TestSpeed();
	// ��ʼ�����ļ�
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
* ������������û��ı���һЩ�����Ϣ
*/
BOOL SendPlayerInfo()
{
	//�õ�version
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

	//�õ�����ϵͳ��������
	//��ʼ��������Դ
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
	//�õ��û�ʹ�õ�ɱ������б�
	strStat += "|" + GetAntiVursSofts();
	//�õ�userid
	char szID[INT_LENGTH];
	memset(szID, 0, INT_LENGTH);
	if(!CLhcImg::GetUserID(szID, INT_LENGTH))
	{
		_snprintf(szID, INT_LENGTH-1, "%s", "0");
	}
	strStat = strStat+ "}|U:"+szID+">";
	//��־��¼�ڱ���
	LogFunctionalMsg("PlayerInfo", strStat.c_str());
	
	return TRUE;
}

//���������װ�Ƿ�ɹ�
//����ɹ��Ļ������¼�ɹ�����־��ɾ��ע�����Щ����ͱ���һЩ�����Ϣ
bool CheckInstallState()
{
	string strInstall;
	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY, strInstall))
	{
		//��װ�ű�д��ע����������Ϊ�������������ж���װʧ�ܣ���д��PLAYER_RUNNING
		//�����װ�ɹ�����д��Success
		if(strInstall != "PLAYER_RUNNING")
		{
			//���Ͱ�װ״̬�Ͱ�װ֮ǰ�İ汾��
			string strLastVer;
			YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, strLastVer);
			strInstall = strInstall + "|" + strLastVer;
			//���Ͱ�װ״̬ ������ͳɹ� ɾ����װ���
			LogFunctionalMsg("INSTALL_STATE", strInstall.c_str());

			//ɾ��ԭ���İ汾��
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION);
			//ɾ����װ���
			YL_EncFileReg::DeleteValue( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_INSTALL_KEY );
			//���������Ϣ��¼�ڱ���
			SendPlayerInfo();

			//�����װ�ɹ� �ص��������״̬ 
			ChangeCurrentCmd(1);
			return true;
		}
	}
	return false;
}

/**
 * ����: ��װ���ص� updateĿ¼�µ����
 * ����ֵ: ��װ�ɹ����� true;
 *         ��װʧ�ܷ��� false.
*/
bool InstallSoft()
{
	//����ϴΰ�װ�ɹ�
	if(CheckInstallState())
		return true;

	//��ⰲװ�����ʹ���
	int  nRet = CheckCmdLimit(4);
	if(nRet == CMD_CHECK_FAIL)
	{
		//����Wait_For_Next 
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

		//��¼��װ֮ǰ�İ汾��
		char szLastVersion[MAX_PATH];
		memset( szLastVersion, 0, MAX_PATH );
		CLhcImg::GetSoftwareVersion(szLastVersion, MAX_PATH);
		YL_EncFileReg::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_LAST_VERSION, szLastVersion);

		//���а�װ�ļ�
		YL_Ini ini(szConfFilePath);
		if(ini.GetString("Update", "filename", szFileName, VERSION_LENGTH-1) > 0)
		{
			string strFileName = szFileName;
			string strBasePath = szHomePath;
			//����ļ�������
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
	//������Ǿ�Ĭ��װ, ��װ�ɲ�����ִ��, bho�ȴ���һ��
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
			//���ñ�� �ò��������а�װ��
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_MB_NEW_LYRIC_VER);
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_NEW_LYRIC_VERSION);
			if(ini.GetUInt("Update", "mbupdate", 0) == 1)
			{
				YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_MB_NEW_LYRIC_VER, szFileName );
			}else
			{
				YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_NEW_LYRIC_VERSION, szFileName );
			}
			// �����°汾������Ϣ
			char szImgPath[MAX_PATH], szInfo[INI_SECTION_BUFFER_LENGTH];
			CLhcImg::GetHomePath(szImgPath, MAX_PATH);
			strcat(szImgPath, "\\res\\pic\\lhupd.img");

			YL_Ini upini(szImgPath);
			ini.GetString("NewestSoft", "intro", szInfo, INI_SECTION_BUFFER_LENGTH-1);
			upini.WriteString("NewestSoft", "intro", szInfo);
			//bho�ȴ���һ��
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
	// g_szModuleΪ�մ���ʾ�ϴ����һ����־
	if(g_szModule[0] == 0)
	{
		return SendLogToServer();
	}
	//�������������ļ����·��
	char szDesFilePath[MAX_PATH];
	if(!CLhcImg::GetTempConfigPath(szDesFilePath, MAX_PATH)) //svr.img
	{
		return false;
	}

	//����ϴ�������װ�Ƿ�ɹ�
	CheckInstallState();
	bool bNewConf = CheckUpdate(); //��ȡsvr.img�������ݷ���lhpcmd.img,lhplog.img�����ж��Ƿ�������
									//�м��wait 45��
	SendMessage(g_hClientWnd,(WM_USER+14),2,0);

	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "GetConfigFile--DONE");

	//�ϴ��ͻ�����־��һ�촫һ��,�ϴ�ǰwait 10����
	SendLogToServer();

	SendMessage(g_hClientWnd,(WM_USER+14),3,0);
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "SendLogToServer Finished.");

	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "SendLogToServer Finished.");
	//��ȡ��ǰ����
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
		if(bNewConf)//CheckUpdate()) //check update ��ǰ, ÿ�����ִ��, ���� dwCmd ����
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
		//TestSpeed()) //���ٺ����غ�һ,�����е�������
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "TestSpeed Success.");
			ChangeCurrentCmd(3);
			dwCmd = 3;
		}
	case 3:
		if(DownloadFile())
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UpdaetCmd", "DownloadFile Success.");
			//ɾ��breakpoint���
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
