#ifndef _YL_HTTPDOWNFILE_H
#define _YL_HTTPDOWNFILE_H

#ifdef	YL_HTTPREQUEST_EXPORT
#define YL_HTTPREQUEST_DLL __declspec(dllexport)
#else
#define YL_HTTPREQUEST_DLL __declspec(dllimport)
#endif

#include "yl_httpint.h"

#include <string>
using namespace std;

class GenericHTTPClient;

class YL_HTTPREQUEST_DLL YL_CHTTPDownFile
{
public:
	enum PROXY_TYPE
	{
		NO_PROXY		= 0,
		IE_AUTO_PROXY	= 1,
		HTTP_PROXY		= 2,
		SOCKS4_PROXY	= 3,
		SOCKS5_PROXY	= 4
	};

public:
	YL_CHTTPDownFile();
	~YL_CHTTPDownFile();

public:

	//**************************************************************************//
	//																			//
	//	���ô����������֧�ֵĴ������HTTP,SOCKS4��SOCKS5��						//
	//	�����ָ��ʹ��IE�����ô���												//
	//	��������øú�������Ĭ������Ϊ��ʹ�ô���								//
	//																			//
	// ������																	//
	//		enumuProxyType	: ��������											//
	//		pStrProxyIP		: ���������IP										//
	//		pStrUserName	: �����û���										//
	//		pStrPassword	: ��������											//
	//		iPort			: ����˿�											//
	//																			//
	// ����ֵ��																	//
	//		true ��	���óɹ���������֤����Ҳ�ɹ�								//		
	//		false��	����ʧ��													//
	//																			//
	//**************************************************************************//
	bool	SetProxy( YL_CHTTPDownFile::PROXY_TYPE enumProxyType,const string* pStrProxyIP = 0,const string* pStrUserName = 0,const string* pStrPassword = 0,int iPort = 80 );

	//**************************************************************************//
	//																			//
	//	���HTTP���󷵻���,��Request��ɺ��ͨ���ýӿڻ�÷�����				//
	//																			//
	//  ����:																	//
	//		lCode	: ������													//
	//																			//
	// ����ֵ��																	//
	//		true ��	��������Ч													//		
	//		false��	��������Ч													//
	//																			//
	//**************************************************************************//
	bool	GetReturnCode( long& lCode );

	//**************************************************************************//
	//																			//
	//	���HTTP���󷵻ص�HTTPͷ												//
	//																			//
	//  ����:																	//
	//		lHeadLen	: ͷ�ĳ���												//
	//																			//
	// ����ֵ��																	//
	//		���ͷ������Ч�����᷵�ؿ�ָ�룬���򷵻�����ָ�롣					//
	//		ע�⣺��Ҫ��HTTP ͷ������ָ���ͷţ���DLL��Release���ͷš�			//
	//																			//
	//**************************************************************************//
	unsigned char*	GetHeader( long& lHeadLen );

	//**************************************************************************//
	//																			//
	//	�Ͽ��˴�HTTP����,������Request�У���Ϊ����ʱ���ɵ��øú�����			//
	//	��Request����������														//
	//																			//
	// ����ֵ��																	//
	//		true	: �Ͽ��ɹ�													//
	//		false	: �Ͽ�ʧ��													//
	//																			//
	//**************************************************************************//
	bool	Disconnect();	

	//**************************************************************************//
	//																			//
	//  �����ļ���֧�ֶϵ�����													//
	//																			//
	//  ����:																	//
	//		strURLFile : Ҫ�����ļ���URL										//
	//		strDesFile : ����ڱ��ص�·��										//
	//		unFileSize : Ҫ���ص��ļ���С,���Ϊ0,��ָ���ļ���С.				//
	//					 ���ָ�����ļ���С����ʵ�����ع����У����ַ��������ļ�	//
	//					 ��С��ָ����С��һ�£���������ؽ���ʱ������unFileSize	//
	//					 Ϊ���������ļ���С										//
	// ����ֵ��																	//
	//		true	: ���سɹ�													//
	//		false	: ����ʧ��													//
	//																			//
	//**************************************************************************//
	bool	DownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize = NULL);	

	//**************************************************************************//
	//																			//
	//  �첽��ʽ�����ļ���֧�ֶϵ�����													//
	//																			//
	//  ����:																	//
	//		strURLFile : Ҫ�����ļ���URL										//
	//		strDesFile : ����ڱ��ص�·��										//
	//		unFileSize : Ҫ���ص��ļ���С,���Ϊ0,��ָ���ļ���С.				//
	//					 ���ָ�����ļ���С����ʵ�����ع����У����ַ��������ļ�	//
	//					 ��С��ָ����С��һ�£���������ؽ���ʱ������unFileSize	//
	//					 Ϊ���������ļ���С										//
	// ����ֵ��																	//
	//		true	: ���������̳߳ɹ�													//
	//		false	: ���������߳�ʧ��													//
	//																			//
	//**************************************************************************//
	bool	AsyncDownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize = NULL);	

	//**************************************************************************//
	//																			//
	//  ��õ�ǰ���ر���														//
	//																			//
	//  ����:																	//
	// ����ֵ��																	//
	//		double	: ���ذٷ���												//
	//																			//
	//**************************************************************************//
	double	GetCurrentDownloadStatus();

	unsigned int GetFileSize() { return m_unNetFileSize; };
	void SetDownloadFailed(bool bFailed=true) { m_bDownloadFailed = bFailed; };
	bool IsDownloadFailed() { return m_bDownloadFailed; };
	string GetParamUrl() { return m_strParamUrl; };
	string GetParamLocalFile() { return m_strParamLocalFile; };
	unsigned int *GetParamFileSize() { return m_punParamFileSize; };
	HTTP_DOWN_FAILED_REASON GetFailedReason() { return m_enumFailReason; };
	unsigned int GetSpeed() { return m_unCurrentSpeed; };

private:

	GenericHTTPClient*	m_pHTTPClient;
	unsigned int		m_unNetFileSize;
	unsigned int		m_unLocalFileSize;
	unsigned int		m_unDownFileSize;
	string				m_strNetFileURL;
	string				m_strLocalFilePath;
	HTTP_DOWN_FAILED_REASON	m_enumFailReason;

	unsigned int		m_unCurrentSpeed;

	void		CreateDir();
	bool		GetNetFileSize();
	bool		GetNetFileSize_old();

	HANDLE				m_hDownloadThread;
	bool				m_bDownloadFailed;

public:
	string				m_strParamUrl;
	string				m_strParamLocalFile;
	unsigned int       *m_punParamFileSize;
};

#endif