#include "GenericHTTPClient.h"
#include "../../include/YL_HTTPDownFile.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/MacroInt.h"
#include "./../Log/KwLogSvr.h"


static const char* LOG_FILE = "log_downfile.txt";

YL_CHTTPDownFile::YL_CHTTPDownFile()
{
	m_pHTTPClient		= new GenericHTTPClient();
	m_enumFailReason	= HTTP_FAILED_UNKNOWN;
	m_unLocalFileSize	= 0;
	m_unNetFileSize		= 0;
	m_unDownFileSize	= 0;

	m_unCurrentSpeed	= 0;

	m_hDownloadThread = NULL;
	m_bDownloadFailed = false;
}

YL_CHTTPDownFile::~YL_CHTTPDownFile()
{
	delete m_pHTTPClient;
	if (m_hDownloadThread != NULL)
	{
		CloseHandle(m_hDownloadThread);
	}
}

bool YL_CHTTPDownFile::SetProxy( YL_CHTTPDownFile::PROXY_TYPE enumProxyType,const string* pStrProxyIP,const string* pStrUserName,const string* pStrPassword ,int iPort)
{
	GenericHTTPClient::HTTPProxyType httpProxy;
	switch( enumProxyType )
	{
	case YL_CHTTPDownFile::HTTP_PROXY:
		httpProxy = GenericHTTPClient::HTTP_Proxy;
		break;
	case YL_CHTTPDownFile::NO_PROXY:
		httpProxy = GenericHTTPClient::No_Proxy;
		break;
	case YL_CHTTPDownFile::IE_AUTO_PROXY:
		httpProxy = GenericHTTPClient::IE_Auto_Proxy;
		break;
	case YL_CHTTPDownFile::SOCKS4_PROXY:
		httpProxy = GenericHTTPClient::Socks4_Proxy;
		break;
	case YL_CHTTPDownFile::SOCKS5_PROXY:
		httpProxy = GenericHTTPClient::Socks5_Proxy;
		break;
	default:
		httpProxy = GenericHTTPClient::No_Proxy;
		break;
	}

	m_pHTTPClient->SetProxy( httpProxy,
		pStrProxyIP != NULL ? pStrProxyIP->c_str() : NULL,
		pStrUserName != NULL ? pStrUserName->c_str() : NULL,
		pStrPassword != NULL ? pStrPassword->c_str() : NULL,
		iPort);
	
	return true;
}

bool YL_CHTTPDownFile::GetReturnCode( long& lCode )
{
	lCode = m_pHTTPClient->m_dwStatusCode;
	return true;
}


unsigned char* YL_CHTTPDownFile::GetHeader( long& lHeadLen )
{
	lHeadLen = (long)m_pHTTPClient->m_memHeader.size;
	return m_pHTTPClient->m_memHeader.buffer;
}


bool YL_CHTTPDownFile::Disconnect()
{
	return m_pHTTPClient->Close( TRUE ) == TRUE;	
}

void YL_CHTTPDownFile::CreateDir()
{
	CHAR lpszDesFile[512];
	strcpy(lpszDesFile,m_strLocalFilePath.c_str());

	CHAR* p1 = strchr( lpszDesFile,'\\');
	CHAR* p2 = strchr( lpszDesFile,'/');
	if( p1 != NULL || p2 != NULL )
	{
		CHAR szdir[256];
		if( p1 == NULL && p2 != NULL )
		{
			strncpy( szdir,lpszDesFile,p2-lpszDesFile );
			szdir[p2-lpszDesFile] = '\0';
		}
		else if( p1 != NULL && p2 == NULL )
		{
			strncpy( szdir,lpszDesFile,p1-lpszDesFile );
			szdir[p1-lpszDesFile] = '\0';
		}
		else if( p1 != NULL && p2 != NULL )
		{
			if( p1 < p2 )
			{
				strncpy( szdir,lpszDesFile,p2-lpszDesFile );
				szdir[p2-lpszDesFile] = '\0';
			}
			else
			{
				strncpy( szdir,lpszDesFile,p1-lpszDesFile );
				szdir[p1-lpszDesFile] = '\0';
			}
		}
		YL_DirInfo::MakeDir(string(szdir));
	}
}

bool YL_CHTTPDownFile::GetNetFileSize_old()
{
	//连接网络代码段--BEGIN
//	YL_Log(LOG_FILE,LOG_NOTICE, "GetNetFileSize", "Init network connection" );
	//先进行HTTP连接

	m_pHTTPClient->m_strSrcURL = m_strNetFileURL;

	if( m_pHTTPClient->SendRequest( m_unLocalFileSize ) && m_pHTTPClient->GetHeader() )
	{		
		if( m_pHTTPClient->m_dwStatusCode == 416 )
		//range 头设置大于服务器端文件大小
		{
//			YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "return code is 416!");
			//断掉上次网络连接
			m_pHTTPClient->Close();
			m_pHTTPClient->destroy();

			//重新创建文件
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			if( (hHandle = ::CreateFile( m_strLocalFilePath.c_str(),
				GENERIC_READ|GENERIC_WRITE,
				NULL,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL)) == INVALID_HANDLE_VALUE )
				//FILE CREATE failed
			{					
//				YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "GetNetFileSize:Create File is failed!");
//				YL_LogMsgFromCode(LOG_FILE,::GetLastError());				
				return  false;
			}
			CloseHandle( hHandle );
			m_unLocalFileSize = 0;//重新创建了文件，必须修改文件大小
			
			//再次进行网络连接-BEGIN
			if( !m_pHTTPClient->SendRequest() || !m_pHTTPClient->GetHeader() )
			{
				m_pHTTPClient->Close();
				m_pHTTPClient->destroy();
//				YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "GetNetFileSize:Connect is failed!" );
				return FALSE;
			}
			//再次进行网络连接-END
		}
		else if( m_pHTTPClient->m_dwStatusCode == 200 && m_unLocalFileSize > 0)
		{
			//server不支持断点续传，需重新下载文件
//			YL_Log( LOG_FILE,LOG_NOTICE, "GetNetFileSize", "GetNetFileSize:server not support duan_dian_xu_chuan!");		
			//重新创建文件
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			if( (hHandle = ::CreateFile( m_strLocalFilePath.c_str(),
				GENERIC_READ|GENERIC_WRITE,
				NULL,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL)) == INVALID_HANDLE_VALUE )
				//FILE CREATE failed
			{					
//				YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "GetNetFileSize:Create File is failed!");
//				YL_LogMsgFromCode(LOG_FILE,::GetLastError());				
				return  false;
			}
			CloseHandle( hHandle );
			m_unLocalFileSize = 0;//重新创建了文件，必须修改文件大小
		}	
		else if( m_pHTTPClient->m_dwStatusCode != 200 && m_pHTTPClient->m_dwStatusCode != 206 )
		{
			m_pHTTPClient->Close();
			m_pHTTPClient->destroy();
			return false;
		}
	}
	else
	//连接失败
	{
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();
//		YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "connect is failed!");
		return false;
	}
//	YL_Log( LOG_FILE,LOG_NOTICE, "GetNetFileSize",   "Download:Request is succeeded!");		

	//连接网络代码段--END

	char *	tmp = NULL;
	char*	tmp2 = NULL;
	if( m_pHTTPClient->m_memHeader.buffer != NULL && strlen((char*)m_pHTTPClient->m_memHeader.buffer) != 0 &&
		(tmp = strstr((char*)m_pHTTPClient->m_memHeader.buffer,"Content-Length: ")) != NULL )
	{
		tmp += strlen("Content-Length: ");
		tmp2 = strchr(tmp,'\r');
		if( tmp2 != NULL )
		{
			char szBuf[12];
			strncpy(szBuf,tmp,tmp2-tmp);
			szBuf[tmp2-tmp] = 0;

			m_unNetFileSize = atol( szBuf );
		}
		else
		{
			tmp2 = strchr(tmp,'\n');
			if( tmp2 != NULL )
			{
				char szBuf[12];
				strncpy(szBuf,tmp,tmp2-tmp);
				szBuf[tmp2-tmp] = 0;

				m_unNetFileSize = atol( szBuf );
			}
			else
			{			
				m_unNetFileSize = 0;
			}
		}
		//		
	}
	else
		//can not get file size
	{
		m_unNetFileSize = 0;
	}

	return true;
}
bool YL_CHTTPDownFile::GetNetFileSize()
{
	m_pHTTPClient->m_strSrcURL = m_strNetFileURL;

	// 如果返回416，那么有两种可能：
	// 1是本地文件不对，大小比远程文件还大；
	// 2是已经下载完成
	// 需要多请求一次来确认这个情况
	unsigned int unRequestSize = m_unLocalFileSize;
	bool bConnected = ( m_pHTTPClient->SendRequest( unRequestSize ) && m_pHTTPClient->GetHeader() );
	if ((bConnected) && ( m_pHTTPClient->m_dwStatusCode == 416 ))
	{
		// 连接成功，但是range 头设置大于服务器端文件大小，此时无法获取文件大小，需要重新连接
		//断掉上次网络连接
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();

		unRequestSize = 0;
		//再次进行网络连接-BEGIN
		bConnected = m_pHTTPClient->SendRequest( unRequestSize );
		if (bConnected)
		{
			bConnected = m_pHTTPClient->GetHeader();
		}
		//再次进行网络连接-END
	}
	if ((!bConnected) || ((m_pHTTPClient->m_dwStatusCode != 200) && (m_pHTTPClient->m_dwStatusCode != 206)))
	{
		// 连接失败，或者无法获取Header
		// 或者是状态码既不是200，也不是206
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();
		return false;
	}

	// 此时连接成功，并且已经获取到Header
	// 计算出服务器端文件大小：
	char *	tmp = NULL;
	char*	tmp2 = NULL;
	if( m_pHTTPClient->m_memHeader.buffer != NULL && strlen((char*)m_pHTTPClient->m_memHeader.buffer) != 0 &&
		(tmp = strstr((char*)m_pHTTPClient->m_memHeader.buffer,"Content-Length: ")) != NULL )
	{
		tmp += strlen("Content-Length: ");
		tmp2 = strchr(tmp,'\r');
		if( tmp2 != NULL )
		{
			char szBuf[12];
			strncpy(szBuf,tmp,tmp2-tmp);
			szBuf[tmp2-tmp] = 0;

			m_unNetFileSize = atol( szBuf );
		}
		else
		{
			tmp2 = strchr(tmp,'\n');
			if( tmp2 != NULL )
			{
				char szBuf[12];
				strncpy(szBuf,tmp,tmp2-tmp);
				szBuf[tmp2-tmp] = 0;

				m_unNetFileSize = atol( szBuf );
			}
			else
			{			
				m_unNetFileSize = 0;
			}
		}
		//		
	}
	else
		//can not get file size
	{
		m_unNetFileSize = 0;
	}

	if (m_unNetFileSize <= 0)
	{
		// 无法获取到远程文件大小
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();
		return false;
	}

	if (m_pHTTPClient->m_dwStatusCode == 206)
	{
		// 如果返回的是部分内容的长度，则需要加上请求的长度
		m_unNetFileSize += unRequestSize;
	}

	// 如果返回200，说明服务器将从头开始发送数据，该连接可以继续使用。
	// 在这种情况下，本地文件有四种可能：
	// 1：部分内容；2：下载完成（服务器端忽略了range）；3：本地是一个错误的文件，大小比远程文件大；4：本地为空文件
	// 其中，1和3需要清空本地文件

	// 如果返回206（请求大小肯定是大于0的），说明服务器支持断点续传，并且文件大小合适，可以获取远程文件大小，并继续使用该连接
	if (m_pHTTPClient->m_dwStatusCode == 200)
	{
		if ((m_unLocalFileSize > 0) && (m_unLocalFileSize != m_unNetFileSize))
		{
			// 对应上面1和3的可能
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			if( (hHandle = ::CreateFile( m_strLocalFilePath.c_str(),
				GENERIC_READ|GENERIC_WRITE,
				NULL,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL)) == INVALID_HANDLE_VALUE )
				//FILE CREATE failed
			{					
				//				YL_Log( LOG_FILE,LOG_WARNING, "GetNetFileSize", "GetNetFileSize:Create File is failed!");
				//				YL_LogMsgFromCode(LOG_FILE,::GetLastError());				
				return  false;
			}
			CloseHandle( hHandle );
			m_unLocalFileSize = 0;//重新创建了文件，必须修改文件大小
		}
	}

	return true;
}

bool YL_CHTTPDownFile::DownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize)
{
	//YL_Log(LOG_FILE,LOG_NOTICE, "YL_CHTTPDownFile", "DownloadFile URL is %s",strURLFile.c_str() );
//	YL_Log(LOG_FILE,LOG_NOTICE, "YL_CHTTPDownFile", "DownloadFile des is %s",strDesFile.c_str() );

	m_enumFailReason	= HTTP_FAILED_UNKNOWN;
	m_strNetFileURL		= strURLFile;
	m_strLocalFilePath	= strDesFile;

	//创建文件路径中的目录
	CreateDir();
	//先获得文件大小-BEGIN
	 YL_FileInfo::GetFileSize( strDesFile.c_str(),&m_unLocalFileSize);	
	//先获得文件大小-END

	//获得content length-网络文件大小--BEGIN
	if( !GetNetFileSize() )
	{
		//YL_Log(LOG_FILE,LOG_NOTICE, "YL_CHTTPDownFile", "FAILED!!! GetNetFileSize() return false.");
		m_enumFailReason = HTTP_FAILED_NOTCONNECT;
		return false;
	}

	//YL_Log(LOG_FILE,LOG_NOTICE, "YL_CHTTPDownFile", "Net size is %d,File size is %d",m_unNetFileSize,m_unLocalFileSize);
	//获得content length-网络文件大小--END

	if( pFileSize != NULL && (*pFileSize) > 0 )
	{
		if( *pFileSize != m_unLocalFileSize + m_unNetFileSize )
		{
			//重新创建文件
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			if( (hHandle = ::CreateFile( m_strLocalFilePath.c_str(),
				GENERIC_READ|GENERIC_WRITE,
				NULL,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL)) == INVALID_HANDLE_VALUE )
				//FILE CREATE failed
			{					
//				YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", ":Create File is failed!");
//				YL_LogMsgFromCode(LOG_FILE,::GetLastError());	
				m_enumFailReason = HTTP_FAILED_CANT_CREATE_LOCAL_FILE;
				return  false;
			}
			CloseHandle( hHandle );
			m_unLocalFileSize = 0;//重新创建了文件，必须修改文件大小
			//断掉上次网络连接
			m_pHTTPClient->Close();
			m_pHTTPClient->destroy();

			//再次进行网络连接-BEGIN
			if( m_pHTTPClient->SendRequest() && m_pHTTPClient->GetHeader() )
			{
				if( m_pHTTPClient->m_dwStatusCode != 200 && 
					m_pHTTPClient->m_dwStatusCode != 206 )
				{
//					YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "Download:Request is failed!And the status code is %d",m_pHTTPClient->m_dwStatusCode);			
					m_pHTTPClient->Close();
					m_pHTTPClient->destroy();
					m_enumFailReason = HTTP_FAILED_NOTCONNECT;
					return false;
				}
			}
			else
			{
//				YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "DownlaodFile:Connect is failed!" );
				m_enumFailReason = HTTP_FAILED_NOTCONNECT;
				return false;
			}
			//再次进行网络连接-END
		}
		*pFileSize = m_unLocalFileSize + m_unNetFileSize;
	}

	// modified by chengzw on 2009-12-01
	m_unDownFileSize = m_unLocalFileSize;

	if (m_unDownFileSize == m_unNetFileSize)
	{
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();
        //YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "Already downloaded. return TRUE" );
		m_enumFailReason = HTTP_FAILED_UNKNOWN;
		return true;
	}

	HANDLE m_hDownFile = INVALID_HANDLE_VALUE;
	if( (m_hDownFile = ::CreateFile( m_strLocalFilePath.c_str(),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE )
		//FILE CREATE failed
	{					
//		YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "DownloadFile:Create File is failed!");
//		YL_LogMsgFromCode(LOG_FILE,::GetLastError());				
		m_pHTTPClient->Close();
		m_pHTTPClient->destroy();
		m_enumFailReason = HTTP_FAILED_CANT_CREATE_LOCAL_FILE;
		return  false;
	}
	

	//set file poiiter,支持断点续传	
	if( m_unLocalFileSize >0 )
		//need set file poiter 
	{	
		if( SetFilePointer( m_hDownFile,m_unLocalFileSize,NULL,FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
			//文件指针设置失败
		{			
//			YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "DownloadFile:SetFilePointer is failed!" );
//			YL_LogMsgFromCode(LOG_FILE,::GetLastError() );
			m_pHTTPClient->Close();
			m_pHTTPClient->destroy();
			m_enumFailReason = HTTP_FAILED_CANT_CREATE_LOCAL_FILE;
			return false;
		}		
	}

	//开始下载
	// m_unDownFileSize = 0;//设置已经下载的大小为0
    //YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "Start downloading..." );

	DWORD	dwResultSize = 0;
	bool	bDownSucceeded = TRUE;
	DWORD	dwWriteLength = 0;
	LPBYTE	pszBuffer = new BYTE[SIZE_HTTP_RESPONSE_BUFFER];
	DWORD   dwPeriodStartTime = GetTickCount();
	DWORD   dwPeriodEndTime;
	DWORD	dwPeriodSize = 0;
	try
	{
		while( (dwResultSize = m_pHTTPClient->sock.readUpto((char*)pszBuffer,SIZE_HTTP_RESPONSE_BUFFER)) > 0)
		{
			if( ::WriteFile( m_hDownFile,pszBuffer,dwResultSize,&dwWriteLength,NULL ) == 0 )
			{				
//				YL_Log( LOG_FILE,LOG_WARNING, "YL_CHTTPDownFile", "DownloadFile: WriteFile is failed!");
//				YL_LogMsgFromCode( LOG_FILE,::GetLastError() );
				CloseHandle( m_hDownFile );
				m_pHTTPClient->Close();
				m_pHTTPClient->destroy();
				delete[] pszBuffer,pszBuffer = NULL;
				m_enumFailReason = HTTP_FAILED_CANT_WRITE_LOCAL_FILE;
				return false;
			}
			dwPeriodEndTime = GetTickCount();
			dwPeriodSize += dwResultSize;
			if (dwPeriodEndTime - dwPeriodStartTime > 1000)
			{
				m_unCurrentSpeed = (dwPeriodSize * 1000 / (dwPeriodEndTime - dwPeriodStartTime)) / 1024;
				dwPeriodStartTime = dwPeriodEndTime;
				dwPeriodSize = 0;
			}
			m_unDownFileSize += dwResultSize;			
		}

	}
	catch (SockException2) 
	{
		//YL_Log(LOG_FILE, LOG_NOTICE, "YL_CHTTPDownFile", "YL_CHTTPDownFile::_downloadFileImpl error %d", GetLastError());
		m_enumFailReason = HTTP_FAILED_SOCKET_ERROR;
		bDownSucceeded =false;
	}
	catch (TimeoutException2)
	{
		//YL_Log(LOG_FILE, LOG_NOTICE, "YL_CHTTPDownFile", "YL_CHTTPDownFile::SendRequest error %d", GetLastError());
		m_enumFailReason = HTTP_FAILED_TIMEOUT;
		bDownSucceeded =false;
	}

	if( dwResultSize == SOCKET_ERROR )
	{
		m_enumFailReason = HTTP_FAILED_SOCKET_ERROR;
		bDownSucceeded =false;
	}

	CloseHandle(m_hDownFile );
	m_pHTTPClient->Close();
	m_pHTTPClient->destroy();
	delete[] pszBuffer,pszBuffer = NULL;
	if( pFileSize != NULL && *pFileSize > 0 && m_unDownFileSize + m_unLocalFileSize != *pFileSize )
	{
		if( !bDownSucceeded)
		{
			m_enumFailReason = HTTP_FAILED_SOCKET_ERROR;
//			YL_Log(LOG_FILE, LOG_FATAL, "YL_CHTTPDownFile", "Download File Error!. response Error.");
		}
		return false;
	}
	
	return bDownSucceeded;
}

DWORD WINAPI HttpDownloadThread(VOID* pPar)
{
	YL_CHTTPDownFile *pDownloader = (YL_CHTTPDownFile *)pPar;
	if (pDownloader == NULL)
	{
		return 0;
	}
	if (!pDownloader->DownloadFile(pDownloader->GetParamUrl(), pDownloader->GetParamLocalFile(), pDownloader->GetParamFileSize()))
	{
		// MessageBox(NULL, "DownloadFile return false!", "", 0);
		pDownloader->SetDownloadFailed(true);
	}
	else
	{
		// MessageBox(NULL, "DownloadFile return true!", "", 0);
	}
	return 0;
}

bool YL_CHTTPDownFile::AsyncDownloadFile( const string& strURLFile,const string& strDesFile,unsigned int* pFileSize /* = NULL */)
{
	m_strParamUrl = strURLFile;
	m_strParamLocalFile = strDesFile;
	m_punParamFileSize = pFileSize;

	DWORD   dwThreadID;	

	//	YL_Log( LOG_FILE,LOG_NOTICE,"Request","Request Start! url:%s,timerout:%d,%u",lpszURL,iTimeOut,this );
	m_hDownloadThread = CreateThread(
		NULL,            // Pointer to thread security attributes
		0,               // Initial thread stack size, in bytes
		HttpDownloadThread,  // Pointer to thread function
		this,     // The argument for the new thread
		0,               // Creation flags
		&dwThreadID      // Pointer to returned thread identifier
		);
	return (m_hDownloadThread != NULL);
}

double YL_CHTTPDownFile::GetCurrentDownloadStatus()
{
	if( m_unNetFileSize != 0 )
	{
		// return (double)m_unDownFileSize / m_unNetFileSize;
		if (m_unDownFileSize == m_unNetFileSize)
		{
			return 1.0;
		}
		return (double)(m_unDownFileSize / BLOCK_SIZE) / (m_unNetFileSize / BLOCK_SIZE + 1);
	}
	return -1;
}
