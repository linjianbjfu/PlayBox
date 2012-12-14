
#ifndef _KWLOGSVR_H
#define _KWLOGSVR_H

#define LOG_DEBUG       0            //��¼����������־��������û�д���,���ڸ��ٳ����ִ��
#define LOG_NOTICE      1            //��¼����������־��������û�з�������,���ڼ�¼����Ĺؼ���
#define LOG_WARNING		2
#define LOG_FATAL       3            //��¼����������־��������������������,�������
#define LOG_KEYINFO     4            //��¼���״̬���û���Ϊ


#ifdef _NOLOG
#define YL_Log (void)0
#else

#ifdef KWLOGSVR_EXPORTS
#define KWLOGSVR_API __declspec(dllexport)
#else
#define KWLOGSVR_API __declspec(dllimport)
#endif

#include "windows.h"
#include <string>
#include <map>
using namespace std;
#pragma warning( disable: 4251 )

KWLOGSVR_API void	YL_Log( const string& strPath,  int level, const string& moduleName, const char* szFmt, ... );
KWLOGSVR_API void	LogInit(const string& strModule);
KWLOGSVR_API void	LogRealMsg( const string& strAct, const string& strmsg,bool bNeedRet = false,unsigned char** ppUnBuffer = NULL,unsigned int* pUnBufLen = NULL );
KWLOGSVR_API void	LogUserActMsg( const string& strAct, const string& strmsg);
KWLOGSVR_API void	LogFunctionalMsg( const string& strAct, const string& strmsg);
KWLOGSVR_API void	UpdateConfig();
KWLOGSVR_API void	SetStartupType(const string& strType);

struct RealTimeMsg
{
	unsigned int nDailyTimes;
	unsigned int nInterval;
	string strLogSvr;
};

// This class is exported from the KwLogSvr.dll


class YLMutex 
{
public:
	// YLMutex(const char* strName = NULL);
	// �ĳɽ��̼����,�޸���-6-07-13
	YLMutex(const char* strName = "LangHua_GAMEBOX_2010-04-25");
	virtual ~YLMutex();
	bool Lock();
	bool Unlock();
private:
	HANDLE m_hMutex;
};


class CKwLogSvr {

public:
	CKwLogSvr(void);
	~CKwLogSvr();
	// TODO: add your methods here.
private:
	char m_szLogFile[MAX_PATH];
	char m_szModuleName[64];

	map<string, RealTimeMsg> m_MsgMap;

public:
	string FormatMsg(int nMsgType, const string& msg);
	bool RealTimeMsgControl(const string& strMsgName);

	HRESULT Init(const string&  strModule);
	HRESULT UpdateConfig();
	HRESULT LogRealMsg(const string& strAct, const string& strmsg,bool bNeedRet = false,unsigned char** ppUnBuffer = NULL,unsigned int* pUnBufLen = NULL );
	HRESULT LogMsg(const string& strAct, const string& strmsg);	


	int				m_iLogVer;
	bool			m_bNeedLogUserAct;

private:

	HANDLE			m_hUploadThread;
	unsigned int	m_nLevel;	
	string			m_strID;
public:
	static string	FormatTime(const SYSTEMTIME& time);
	bool	LogFilter(int level, const string& moduleName);
	void	Log(const string& strPath,  int level, const string& moduleName,  const string& msg);
    void	Log(const string& strPath,  int level, const string& moduleName, const char* szFmt, ...);
};


#endif
#endif
