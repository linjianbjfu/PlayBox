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
	//	设置代理服务器，支持的代理包括HTTP,SOCKS4和SOCKS5，						//
	//	另外可指定使用IE所配置代理												//
	//	如果不调用该函数，则默认设置为不使用代理								//
	//																			//
	// 参数：																	//
	//		enumuProxyType	: 代理类型											//
	//		pStrProxyIP		: 代理服务器IP										//
	//		pStrUserName	: 代理用户名										//
	//		pStrPassword	: 代理密码											//
	//		iPort			: 代理端口											//
	//																			//
	// 返回值：																	//
	//		true ：	设置成功，但不保证连接也成功								//		
	//		false：	设置失败													//
	//																			//
	//**************************************************************************//
	bool	SetProxy( YL_CHTTPDownFile::PROXY_TYPE enumProxyType,const string* pStrProxyIP = 0,const string* pStrUserName = 0,const string* pStrPassword = 0,int iPort = 80 );

	//**************************************************************************//
	//																			//
	//	获得HTTP请求返回码,在Request完成后可通过该接口获得返回码				//
	//																			//
	//  参数:																	//
	//		lCode	: 返回码													//
	//																			//
	// 返回值：																	//
	//		true ：	返回码有效													//		
	//		false：	返回码无效													//
	//																			//
	//**************************************************************************//
	bool	GetReturnCode( long& lCode );

	//**************************************************************************//
	//																			//
	//	获得HTTP请求返回的HTTP头												//
	//																			//
	//  参数:																	//
	//		lHeadLen	: 头的长度												//
	//																			//
	// 返回值：																	//
	//		如果头内容无效，将会返回空指针，否则返回内容指针。					//
	//		注意：不要将HTTP 头的内容指针释放，由DLL在Release中释放。			//
	//																			//
	//**************************************************************************//
	unsigned char*	GetHeader( long& lHeadLen );

	//**************************************************************************//
	//																			//
	//	断开此次HTTP连接,但调用Request中，还为返回时，可调用该函数，			//
	//	则Request会立即返回														//
	//																			//
	// 返回值：																	//
	//		true	: 断开成功													//
	//		false	: 断开失败													//
	//																			//
	//**************************************************************************//
	bool	Disconnect();	

	//**************************************************************************//
	//																			//
	//  下载文件，支持断点续传													//
	//																			//
	//  参数:																	//
	//		strURLFile : 要下载文件的URL										//
	//		strDesFile : 存放在本地的路径										//
	//		unFileSize : 要下载的文件大小,如果为0,则不指定文件大小.				//
	//					 如果指定了文件大小，但实际下载过程中，发现服务器端文件	//
	//					 大小和指定大小不一致，则会在下载结束时，更改unFileSize	//
	//					 为服务器端文件大小										//
	// 返回值：																	//
	//		true	: 下载成功													//
	//		false	: 下载失败													//
	//																			//
	//**************************************************************************//
	bool	DownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize = NULL);	

	//**************************************************************************//
	//																			//
	//  异步方式下载文件，支持断点续传													//
	//																			//
	//  参数:																	//
	//		strURLFile : 要下载文件的URL										//
	//		strDesFile : 存放在本地的路径										//
	//		unFileSize : 要下载的文件大小,如果为0,则不指定文件大小.				//
	//					 如果指定了文件大小，但实际下载过程中，发现服务器端文件	//
	//					 大小和指定大小不一致，则会在下载结束时，更改unFileSize	//
	//					 为服务器端文件大小										//
	// 返回值：																	//
	//		true	: 启动下载线程成功													//
	//		false	: 启动下载线程失败													//
	//																			//
	//**************************************************************************//
	bool	AsyncDownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize = NULL);	

	//**************************************************************************//
	//																			//
	//  获得当前下载比例														//
	//																			//
	//  参数:																	//
	// 返回值：																	//
	//		double	: 下载百分率												//
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