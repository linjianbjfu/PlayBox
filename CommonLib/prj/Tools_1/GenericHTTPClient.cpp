#include "GenericHTTPClient.h"
#include "windows.h"
#include "Wininet.h"

#include "../../common/YL_Base64.h"
#include "../../common/YL_DirInfo.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Wininet.lib")

#include "./../Log/KwLogSvr.h"
//static const char* LOG_FILE = "log_http.txt";
#define LOG_FILE "log_http.txt"

#define MEM_BUFFER_SIZE 100

void GenericHTTPClient::MemBufferCreate(MemBuffer *b)
{
	b->size = MEM_BUFFER_SIZE;
	b->buffer =(unsigned	char *) malloc( b->size );
	b->position = b->buffer;
}

void GenericHTTPClient::MemBufferFree(MemBuffer*b)
{
	if( b->buffer != NULL )
	{
		free(b->buffer);
		b->buffer = NULL;
	}
}

void GenericHTTPClient::MemBufferGrow(MemBuffer *b)
{
	size_t sz;
	sz = b->position - b->buffer;
	b->size = b->size *2;
	b->buffer =(unsigned	char *) realloc(b->buffer,b->size);
	b->position = b->buffer + sz;	// readjust current position
}

void GenericHTTPClient::MemBufferAddByte(MemBuffer *b,unsigned char byt)
{
	if( (size_t)(b->position-b->buffer) >= (b->size-1) )
		MemBufferGrow(b);

	*(b->position++) = byt;
}

void GenericHTTPClient::MemBufferAddBuffer(MemBuffer *b,
								 unsigned char *buffer, size_t size)
{
	while( ((size_t)(b->position-b->buffer)+size) >= (b->size-1) )
		MemBufferGrow(b);

	memcpy(b->position,buffer,size);
	b->position+=size;
}

GenericHTTPClient::GenericHTTPClient()
{
//	YL_Log( LOG_FILE,LOG_NOTICE,"GenericHTTPClient","IN" );
	m_sHttpProxy.eHttpProxyType = No_Proxy;
	m_sHttpProxy.iPort = 80;
	memset(m_sHttpProxy.szProxyIP,0,sizeof(m_sHttpProxy.szProxyIP));
	memset(m_sHttpProxy.szPassword,0,sizeof(m_sHttpProxy.szPassword));
	memset(m_sHttpProxy.szUserName,0,sizeof(m_sHttpProxy.szUserName));
	init();
	OIOSocketEx2::init();
	m_curHTTP = NULL;
	m_dwStatusCode = 0;
	m_iCurrentMethod = RequestGetMethod;
}

GenericHTTPClient::~GenericHTTPClient()
{	
//	YL_Log( LOG_FILE,LOG_NOTICE,"~GenericHTTPClient","IN" );
	if( WSACleanup() != 0 )
	{
		//YL_Log( LOG_FILE,LOG_NOTICE,"Close","WSACleanup,return code is %d! ",WSAGetLastError() );
	}
	else
	{
		//YL_Log( LOG_FILE,LOG_NOTICE,"Close","WSACleanup Succeeded!" );
	}
	destroy();
}


//==========================================================================================//
//	SetProxy
//	说明:	该方法用来设置连接的代理
//	参数:	eProxyType		代理类型，包括HTTPProxyType的四种情况
//			lpszProxyIP		代理的IP
//			lpszUserName	若有的话，为代理用户名，否则为NULL
//			lpszUserName	若有的话，为代理密码，否则为NULL
//			iPort			代理端口
//==========================================================================================//
VOID GenericHTTPClient::SetProxy( HTTPProxyType eProxyType,LPCSTR lpszProxyIP,
								  LPCSTR lpszUserName,LPCSTR lpszPassword,int iPort)
{
	//设置代理类型
	m_sHttpProxy.eHttpProxyType = eProxyType;

	if( eProxyType == IE_Auto_Proxy )
	{
		GetIEProxy();
		return;
	}

	//设置代理IP
	if( lpszProxyIP == NULL || strlen( lpszProxyIP) > SIZE_SMALL_BUFFER )
	{
		::ZeroMemory(m_sHttpProxy.szProxyIP,SIZE_SMALL_BUFFER) ;
	}	
	else
	{
		strcpy( m_sHttpProxy.szProxyIP,lpszProxyIP );		 
	}

	//设置代理用户名
	if( lpszUserName == NULL || strlen( lpszUserName) > SIZE_SMALL_BUFFER)
	{
		::ZeroMemory(m_sHttpProxy.szUserName,SIZE_SMALL_BUFFER) ;
	}	
	else
	{
		strcpy( m_sHttpProxy.szUserName,lpszUserName );		
	}

	//设置代理密码
	if( lpszPassword == NULL || strlen( lpszPassword ) > SIZE_SMALL_BUFFER )
	{
		::ZeroMemory(m_sHttpProxy.szPassword,SIZE_SMALL_BUFFER) ;
	}	
	else
	{
		strcpy( m_sHttpProxy.szPassword,lpszPassword );
	}

	//设置代理端口
	m_sHttpProxy.iPort = iPort;
	return;

}


BOOL GenericHTTPClient::GetIEProxy()
{
	unsigned long        nSize = 4096;
	char                 szBuf[4096] = { 0 };
	INTERNET_PROXY_INFO* pInfo = (INTERNET_PROXY_INFO*)szBuf;

	if(!InternetQueryOption(NULL, INTERNET_OPTION_PROXY, pInfo, &nSize))
	{
//		YL_Log(LOG_FILE,LOG_NOTICE,"GetIEProxy","Failed!");
		m_sHttpProxy.eHttpProxyType = No_Proxy;
		return FALSE;
	}
	const char *save = pInfo->lpszProxy;
	if( save == NULL )
	{
		m_sHttpProxy.eHttpProxyType = No_Proxy;
//		YL_Log(LOG_FILE,LOG_NOTICE,"GetIEProxy","NO Proxy!");
		return FALSE;
	}
	else
	{
		string tmpProxy(save);
		size_t Pos = tmpProxy.find(":");
		//if not input port , the default port is set to 80
		if( Pos == string::npos )
		{
			strcpy( m_sHttpProxy.szProxyIP,save);
			m_sHttpProxy.iPort = 80;
			::ZeroMemory(m_sHttpProxy.szUserName,SIZE_SMALL_BUFFER);
			::ZeroMemory(m_sHttpProxy.szPassword,SIZE_SMALL_BUFFER);
			m_sHttpProxy.eHttpProxyType = HTTP_Proxy;
			return TRUE;
		}
		else
		{
			strcpy( m_sHttpProxy.szProxyIP,tmpProxy.substr(0,Pos).c_str());
			m_sHttpProxy.iPort = atoi( tmpProxy.substr(Pos+1).c_str());
			::ZeroMemory(m_sHttpProxy.szUserName,SIZE_SMALL_BUFFER);
			::ZeroMemory(m_sHttpProxy.szPassword,SIZE_SMALL_BUFFER);
			m_sHttpProxy.eHttpProxyType = HTTP_Proxy;
			return TRUE;
		}
	}
}

//==========================================================================================//
//	Request
//	说明:	该方法为主要的HTTP请求类。
//	参数:	lpszURL				请求的URL
//			iMethod				请求的种类，包括GET和POST两种
//			iTimeOut			设定连接的超时时间，如果为-1，则没有超时时间
//			lpszPostArguments	为POST方法的参数，如果是GET，该参数被忽略掉
//			lpszHeader			HTTP请求返回的消息头
//			dwResHeader			lpHeader长度，返回为填入lpHeader长度，若消息头比dwResHeader长，则返回FALSE
//			iRetCode			HTTP返回的状态码
//			lpszContent			HTTP返回的内容
//			dwResContent		lpContent长度，返回为填入lpContent长度，若消息体比dwResContent长，则返回FALSE
//	返回值:	TRUE				HTTP请求成功
//			FALSE				HTTP请求失败(有两种可能，http连接失败，分配buffer太小，调用者须对
//								dwResHeader和dwResContent返回值进行判断
//==========================================================================================//

BOOL GenericHTTPClient::Request( LPCTSTR lpszURL, int iMethod,int iTimeOut,LPBYTE lpPostArguments,DWORD dwPostArg)
{
	m_bSendRequest = FALSE;
	//初始化参数
	init();

	if( lpszURL == NULL )
		//应先调用SetCandidateURLs来设置多URL
	{			
		return FALSE;
	}	

	m_strSrcURL			= lpszURL;
	m_iCurrentMethod	= iMethod;

	if( lpPostArguments != NULL )
	{
		m_lpszPostArguments = new BYTE[dwPostArg];
		memcpy( m_lpszPostArguments,lpPostArguments,dwPostArg );
		m_dwPostLen	= dwPostArg;
	}

	else
		m_dwPostLen = 0;
	if (iTimeOut > 5000)
		sock.setTimeout(iTimeOut - 5000, iTimeOut- 5000);

//	HANDLE  hThread;
	DWORD   dwThreadID;	
	DWORD	dwExitCode;
	BOOL	bRetValue = FALSE;

//	YL_Log( LOG_FILE,LOG_NOTICE,"Request","Request Start! url:%s,timerout:%d,%u",lpszURL,iTimeOut,this );
	m_curHTTP = CreateThread(
		NULL,            // Pointer to thread security attributes
		0,               // Initial thread stack size, in bytes
		RealRequest,  // Pointer to thread function
		this,     // The argument for the new thread
		0,               // Creation flags
		&dwThreadID      // Pointer to returned thread identifier
		);

	// Wait for the call to http request complete
	if( iTimeOut == -1 )
		//no time-out setting,so wait infinite
	{
		WaitForSingleObject( m_curHTTP,INFINITE );
	}
	else
	{
		DWORD dwTimeout = iTimeOut; // in milliseconds	
		if( WaitForSingleObject ( m_curHTTP, dwTimeout ) == WAIT_TIMEOUT )		
		{	
//			YL_Log( LOG_FILE,LOG_WARNING,"Request","Request:HTTP request is time out!");
			Close( TRUE );	
//			YL_Log( LOG_FILE,LOG_WARNING,"Request","After close");
			WaitForSingleObject ( m_curHTTP, 2000);
//			YL_Log( LOG_FILE,LOG_WARNING,"Request","After Wait for single object");
			CloseHandle( m_curHTTP );
			m_curHTTP = NULL;
			destroy();			
			return FALSE;
		}
	}
//	YL_Log( LOG_FILE,LOG_NOTICE,"Request","Request End! %u",this );
	// The state of the specified object (thread) is signaled
	dwExitCode = 0;
	if ( !GetExitCodeThread( m_curHTTP, &dwExitCode ) )
	{
//		YL_Log( LOG_FILE,LOG_WARNING,"Request","Request:Get exit code is error!" );		
		bRetValue = FALSE;
		goto clean;
	}
	CloseHandle (m_curHTTP);
	m_curHTTP = NULL;

	if ( dwExitCode )
	{
		bRetValue = FALSE;
//		YL_Log( LOG_FILE,LOG_WARNING,"Request","Request:thread exit code is %d",dwExitCode );
		goto clean;
	}	

	bRetValue = TRUE;
//	YL_Log( LOG_FILE,LOG_NOTICE,"Request","Request: Succeeded! %u",this );	

clean:
	Close();
	return bRetValue;
}


DWORD WINAPI RealSendRequest(VOID* pPar)
{
	GenericHTTPClient* pGenHTTPClient =(GenericHTTPClient*) pPar;	
//	YL_Log( LOG_FILE,LOG_NOTICE,"RealSendRequest","RealSendRequest: The server is %s,%u",pGenHTTPClient->m_strSrcURL.c_str(),GetCurrentThreadId() );


	if( !pGenHTTPClient->SendRequest() )
	{
//		YL_Log(LOG_FILE,LOG_NOTICE,"RealSendRequest","SendRequest Failed!");
		return 1;
	}	
//	YL_Log( LOG_FILE,LOG_NOTICE,"RealSendRequest","RealSendRequest: OUT" );
	return 0;
}

BOOL GenericHTTPClient::SendRequest( LPCTSTR lpszURL, int iMethod,int iTimeOut,LPBYTE lpPostArguments,DWORD dwPostArg)
{
	m_bSendRequest = TRUE;
	//初始化参数
	init();

	if( lpszURL == NULL )
		//应先调用SetCandidateURLs来设置多URL
	{	
		return FALSE;
	}

	m_strSrcURL			= lpszURL;
	m_iCurrentMethod	= iMethod;

	if( lpPostArguments != NULL )
	{
		m_lpszPostArguments = new BYTE[dwPostArg];
		memcpy( m_lpszPostArguments,lpPostArguments,dwPostArg );
		m_dwPostLen	= dwPostArg;
	}
	else
		m_dwPostLen = 0;
	if (iTimeOut > 5000)
		sock.setTimeout(iTimeOut - 5000, iTimeOut- 5000);

	//	HANDLE  hThread;
	DWORD   dwThreadID;	
	DWORD	dwExitCode;
	BOOL	bRetValue = FALSE;

//	YL_Log( LOG_FILE,LOG_NOTICE,"SendRequest","Request Start! %u",this );
	m_curHTTP = CreateThread(
		NULL,            // Pointer to thread security attributes
		0,               // Initial thread stack size, in bytes
		RealSendRequest,  // Pointer to thread function
		this,     // The argument for the new thread
		0,               // Creation flags
		&dwThreadID      // Pointer to returned thread identifier
		);

	// Wait for the call to http request complete
	if( iTimeOut == -1 )
		//no time-out setting,so wait infinite
	{
		WaitForSingleObject( m_curHTTP,INFINITE );
	}
	else
	{
		DWORD dwTimeout = iTimeOut; // in milliseconds	
		if( WaitForSingleObject ( m_curHTTP, dwTimeout ) == WAIT_TIMEOUT )		
		{	
//			YL_Log( LOG_FILE,LOG_WARNING,"SendRequest","Request:HTTP request is time out!");
			Close( TRUE );	
//			YL_Log( LOG_FILE,LOG_WARNING,"SendRequest","After close");
			WaitForSingleObject ( m_curHTTP, 2000);
//			YL_Log( LOG_FILE,LOG_WARNING,"SendRequest","After Wait for single object");
			CloseHandle( m_curHTTP );
			m_curHTTP = NULL;
			destroy();			
			return FALSE;
		}
	}
//	YL_Log( LOG_FILE,LOG_NOTICE,"SendRequest","Request End! %u",this );
	// The state of the specified object (thread) is signaled
	dwExitCode = 0;
	if ( !GetExitCodeThread( m_curHTTP, &dwExitCode ) )
	{
//		YL_Log( LOG_FILE,LOG_WARNING,"SendRequest","Request:Get exit code is error!" );		
		bRetValue = FALSE;
		goto clean;
	}
	CloseHandle (m_curHTTP);
	m_curHTTP = NULL;

	if ( dwExitCode )
	{
		bRetValue = FALSE;
//		YL_Log( LOG_FILE,LOG_WARNING,"SendRequest","Request:thread exit code is %d",dwExitCode );
		goto clean;
	}	

	bRetValue = TRUE;
//	YL_Log( LOG_FILE,LOG_NOTICE,"SendRequest","Request: Succeeded! %u",this );	

clean:	
	return bRetValue;
}

//真正的HTTP Request处理函数，为了实现超时监控
DWORD WINAPI GenericHTTPClient::RealRequest(VOID* pPar)
{
	try
	{
		GenericHTTPClient* pGenHTTPClient =(GenericHTTPClient*) pPar;	

//		YL_Log( LOG_FILE,LOG_NOTICE,"RealRequest","RealRequest: The server is %s,%u",pGenHTTPClient->m_strSrcURL.c_str(),GetCurrentThreadId() );

		
		if( !pGenHTTPClient->SendRequest() )
		{
//			YL_Log(LOG_FILE,LOG_NOTICE,"RealRequest","SendRequest Failed!");
			return 1;
		}
//		YL_Log( LOG_FILE,LOG_NOTICE,"RealRequest","after send request");
		//Recieve Header From Server
		if( !pGenHTTPClient->GetHeader( ) ||
			(pGenHTTPClient->m_dwStatusCode != 200 && pGenHTTPClient->m_dwStatusCode != 206 
			&& pGenHTTPClient->m_dwStatusCode != 304 ) )
		{	
			return 1;
		}

		//content==
		pGenHTTPClient->GetContent( );
//		YL_Log( LOG_FILE,LOG_NOTICE,"RealRequest","after get content");

		pGenHTTPClient->Close();

//		YL_Log( LOG_FILE,LOG_NOTICE,"RealRequest","RealRequest: OUT" );
	}
	catch (...)
	{
//		YL_Log( LOG_FILE,LOG_NOTICE,"RealRequest","Exception " );
		return 1;
	}
	return 0;
}

BOOL GenericHTTPClient::SendRequest(DWORD dwFileSize)
{
	int				iRetErr = 0;
	TCHAR	szProtocol[SIZE_SMALL_BUFFER] = "";
	TCHAR	szAddress[SIZE_SMALL_BUFFER] = "";

	TCHAR	szURI[SIZE_LARGE_BUFFER];
	memset(szURI,0,SIZE_LARGE_BUFFER);

	unsigned short	uPort = 80;
	//解析URL
	parseURL( szProtocol, szAddress, uPort, szURI );

//	YL_Log(LOG_FILE,LOG_NOTICE,"SendRequest","Before Connect!");
	try
	{
//		YL_Log(LOG_FILE,LOG_NOTICE,"RealRequest","Before Connect!");
		DWORD dwStart = GetTickCount();
		BOOL  bRet = Connect(szAddress, uPort);
		m_dwConnectTime = GetTickCount() - dwStart;
		if(!bRet)
		{
			m_dwStatusCode = -1;
			return FALSE;
		}
//		YL_Log(LOG_FILE,LOG_NOTICE,"RealRequest","Connect Succeeded!");

		string proStr = "";

		if( !*szURI )
		{
			lstrcpyn(szURI,"/",sizeof(szURI));
		}

		if( m_iCurrentMethod == GenericHTTPClient::RequestPostMethod )
		{
			//SendString("POST ");
			proStr += "POST ";
		}
		else 
		{
			//SendString("GET ");
			proStr += "GET ";
		}

		if( m_sHttpProxy.eHttpProxyType == GenericHTTPClient::HTTP_Proxy 
			|| m_sHttpProxy.eHttpProxyType == GenericHTTPClient::IE_Auto_Proxy )
		{
			//SendString(m_strSrcURL);
			proStr += m_strSrcURL;
		}
		else
		{
			//SendString(szURI);
			proStr += szURI;
		}
		proStr += " HTTP/1.0\r\n";
		//SendString(" HTTP/1.0\r\n");
		//SendString("Accept: */*\r\n");

		//SendString("Host: ");
		proStr += "Host: ";
		//SendString(szAddress);
		proStr += szAddress;
		//SendString("\r\n");
		proStr += "\r\n";
		//SendString("User-Agent: Mozilla/4.0\r\n");
		proStr += "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; GTB6; Maxthon ) \r\n";
		if( m_iCurrentMethod == GenericHTTPClient::RequestPostMethod)
		{
			char szProc[256];
			sprintf(szProc, "Content-Length: %d\r\n", m_dwPostLen);
			//SendString(szProc);
			proStr += szProc;
		}
	
		//SendString("Connection: close\r\n");
		proStr += "Connection: Close\r\n";
		proStr += "Accept: */*\r\n";

		if( dwFileSize > 0 )
		{
			char szProc[56];
			_snprintf(szProc,56,"Range:bytes=%d-\r\n",dwFileSize);
			szProc[55] = '\0';
			//SendString(szProc);
			proStr += szProc;
		}

		//SendString(sock,"Cookie: mycookie=blablabla\r\n");
		//	printf("Cookie: mycookie=blablabla\r\n");

		//send http username password
		if( m_sHttpProxy.eHttpProxyType == HTTP_Proxy &&
			strlen(m_sHttpProxy.szUserName) !=0 )
		{
			char msg[256];
			char msgAfterBase64[256];

			if ((strlen(m_sHttpProxy.szUserName) != 0) && 
				(strlen(m_sHttpProxy.szPassword) != 0))
			{
				_snprintf(msg,256,"%s:%s",m_sHttpProxy.szUserName,m_sHttpProxy.szPassword);
				msg[255] = '\0';				
			}
			else if (strlen(m_sHttpProxy.szUserName) != 0)
			{
				_snprintf(msg,256,"%s:",m_sHttpProxy.szUserName);
				msg[255] = '\0';				
			}
			else if (strlen(m_sHttpProxy.szPassword) != 0)
			{
				_snprintf(msg,256,":%s",m_sHttpProxy.szPassword);
				msg[255] = '\0';				
			}
			YL_Base64Encode(msgAfterBase64,msg,(int)strlen(msg));

			_snprintf(msg,256,"Proxy-Authorization: Basic %s\r\n",msgAfterBase64);
			msg[255] = '\0';
			proStr += msg;
			//sock.write(msg,(int)strlen(msg));
		}

		// Send a blank line to signal end of HTTP headerReceive
		//SendString("\r\n");	
		dwStart = GetTickCount();

		proStr += "\r\n";
		SendString(proStr.c_str());

//		YL_Log(LOG_FILE,LOG_NOTICE,"RealRequest","%s",proStr.c_str());

		if( m_iCurrentMethod == GenericHTTPClient::RequestPostMethod
			&& m_lpszPostArguments != NULL )
		{
			sock.write((const void*)m_lpszPostArguments,m_dwPostLen);			
		}

		m_dwReadTime += (GetTickCount() - dwStart );

	}catch (StreamException2 &e) 
	{
//		YL_Log(LOG_FILE, LOG_NOTICE, "GenericHTTPClient", "GenericHTTPClient::SendRequest error, errmsg:%s, errcode:%d", e.msg, e.err);
		return FALSE;
	}

//	YL_Log(LOG_FILE,LOG_NOTICE,"SendRequest","Out!");
	return TRUE;
}

BOOL GenericHTTPClient::GetHeader(  )
{
//	YL_Log(LOG_FILE,LOG_NOTICE,"GetHeader","IN!");
	if( m_bGetHeader )
	{
		if( m_bSendRequest )
		{
//			YL_Log(LOG_FILE,LOG_NOTICE,"GetHeader","m_bSendRequest");
			return FALSE;
		}
//		YL_Log(LOG_FILE,LOG_NOTICE,"GetHeader","OUT 1");
		return TRUE;
	}
//	if( !m_bSendRequest && !bRealRequest )
//	{
//		return TRUE;
//	}

	m_bGetHeader = TRUE;
	char	buffer[128];
	int		chars = 0;
	bool	done = FALSE;
	int		iRetValue = 0;

	//header==
	MemBufferFree(&m_memHeader);
	MemBufferCreate(&m_memHeader );
	try
	{
		DWORD dwStart = GetTickCount();
		while(!done)
		{
			iRetValue = sock.readUpto(buffer,1);
			if( iRetValue == 0 )
			{
				done = TRUE;
			}

			switch(*buffer)
			{
			case '\r':
				break;
			case '\n':
				if(chars==0)
				{
					done = TRUE;
				}
				chars=0;
				break;
			default:
				chars++;
				break;
			}

			MemBufferAddByte(&m_memHeader,*buffer);
		}
		*(m_memHeader.position) = 0;

//		YL_Log(LOG_FILE, LOG_NOTICE, "GenericHTTPClient", "GenericHTTPClient::GetHeader:%s", (char *)m_memHeader.buffer);

		//判断返回代码
		char* tmp = NULL;
		if( (tmp = strchr((char*)m_memHeader.buffer,' ')) == NULL )
		{
			m_dwStatusCode = 0;

			m_dwReadTime += ( GetTickCount() - dwStart );
			m_dwStatusCode = 2;

//			YL_Log(LOG_FILE,LOG_NOTICE,"GetHeader","OUt 2");
			return FALSE;
		}
		strncpy(buffer,tmp+1,3);
		buffer[3] = 0;
		m_dwStatusCode = atoi(buffer);

		if ((m_dwStatusCode != 200 && m_dwStatusCode != 206 
		&& m_dwStatusCode != 304 ))
		{
			// throw SockException2("HTTP reply code err", m_dwStatusCode);
		}

		m_dwReadTime += ( GetTickCount() - dwStart );
	}
	catch (StreamException2 &e)
	{
//		YL_Log(LOG_FILE, LOG_NOTICE, "GenericHTTPClient", "GenericHTTPClient::GetHeader, errormsg:%s, errcode:%d", e.msg, e.err);
		m_dwStatusCode = e.err;
		return FALSE;
	}

//	YL_Log(LOG_FILE,LOG_NOTICE,"GetHeader","OUT!!!");
	return TRUE;
}

BOOL GenericHTTPClient::GetContent(long dwContentSize )
{
//	YL_Log(LOG_FILE, LOG_NOTICE, "GenericHTTPClient", "In Getcontent");

	if( m_bGetContent )
	{
		if( m_bSendRequest )
		{
			return FALSE;
		}
		return TRUE;
	}

	MemBufferFree(&m_memContent);
	MemBufferCreate(&m_memContent);
	
	char buffer[1024];
	int iRead = 0;
	if( dwContentSize != -1 )
	{
		if( dwContentSize < 1024 )
		{
			iRead = dwContentSize;
		}
		else
		{
			iRead = 1024;		
		}
	}

	int	 iRetValue;
	// Now read the HTTP body
	DWORD dwReadData = 0;
	try
	{
		DWORD dwStart = GetTickCount();
		do
		{
			if( dwContentSize != -1 )
			{
				iRetValue = sock.readUpto(buffer,iRead);
			}
			else
			{
				iRetValue = sock.readUpto(buffer,1024);
			}

			if(iRetValue <= 0)
			{
				m_bGetContent = TRUE;
				break;
			}
			dwReadData += iRetValue;

//			*(buffer+iRetValue)=0;
			MemBufferAddBuffer(&m_memContent, (unsigned char*)&buffer, iRetValue);

			if( dwContentSize != -1 && dwReadData >= dwContentSize )
			{
				break;
			}
			if( dwContentSize != -1 )
			{
				iRead = dwContentSize - dwReadData;
				if( iRead > 1024 )
				{
					iRead = 1024;
				}
			}

		} while(iRetValue>0);

		m_dwReadTime += ( GetTickCount() - dwStart );
	}
	catch (StreamException2 &) 
	{
//		YL_Log(LOG_FILE, LOG_NOTICE, "GenericHTTPClient", "GenericHTTPClient::GetContent error %d", GetLastError());
		return FALSE;
	}
	*(m_memContent.position) = 0;
//	YL_Log(LOG_FILE, LOG_NOTICE, "GetContent", "OUT Getcontent,size is %d",dwContentSize);
	return TRUE;
}

//==========================================================================================//
//	SetCandidateURLs
//	说明:	该方法用来设置候选的URL,这些候选的URL会在Request中被检测,并选择最快的一个进行连接
//	参数:	lplpCandidateURLs		候选的URL数组
//			iURLCount				候选的URL个数
//==========================================================================================//
VOID GenericHTTPClient::SetCandidateURLs( LPTSTR* lplpszCandidateURLs,int iURLCount )
{
	if( lplpszCandidateURLs== NULL )
	{
		return;
	}
	m_vecCandidateURLs.clear();
	for( int i = 0;i < iURLCount;i++ )
	{
		if( lplpszCandidateURLs[i] != NULL )
		{
			m_vecCandidateURLs.push_back( lplpszCandidateURLs[i] );
		}
	}
	return;
}

//进行HTTP连接
BOOL GenericHTTPClient::Connect(const char* ipstr, unsigned short port)
{
	SOCKADDR_IN remoteAddr;
	memset(&remoteAddr, 0, sizeof(SOCKADDR_IN));
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	remoteAddr.sin_addr.S_un.S_addr = htonl(sock.getIP(ipstr));

	m_strIP = sock.strIP;

	SOCKADDR_IN des;
	memset(&des, 0, sizeof(SOCKADDR_IN));
	des.sin_family = AF_INET;										//Connect to web sever
	des.sin_port = htons( (unsigned short)m_sHttpProxy.iPort );
	des.sin_addr.S_un.S_addr = htonl(sock.getIP(m_sHttpProxy.szProxyIP));
	//des.sin_addr.s_addr = inet_addr(m_sHttpProxy.szProxyIP);
	//memset( &(des.sin_zero), '\0', 8 );

	try
	{
		switch( m_sHttpProxy.eHttpProxyType )
		{
		case GenericHTTPClient::No_Proxy:
		default:
			{
				sock.open(ipstr, port);
	//			sock.setTimeout();
				sock.connect();
				break;
			} 
		case GenericHTTPClient::HTTP_Proxy:
			{
				sock.open((struct sockaddr_in* )&des,sizeof(SOCKADDR_IN));
				try
				{
					sock.connect();
				}
				catch(StreamException2 &e)
				{
					char strErr[128];
					memset(strErr, 0, 128);
					_snprintf(strErr, 127, "connect HTTP_PROXY failed, %s", e.msg);
					throw SockException2(strErr, e.err);
				}

				break;
			}
		case GenericHTTPClient::Socks4_Proxy:
			{
				ConnectToSocks4((struct sockaddr *)&remoteAddr, (LPSOCKADDR)&des, sizeof(SOCKADDR_IN));
				break;
			}

		case GenericHTTPClient::Socks5_Proxy:
			{
				ConnectToSocks5((struct sockaddr *)&remoteAddr, (LPSOCKADDR)&des, sizeof(SOCKADDR_IN));
				break;
			}
		}
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

int	GenericHTTPClient::ConnectToSocks4(const struct sockaddr* remoteAddr, const struct sockaddr FAR *in_Socks,int socksLen)
{
	int lPacketLen	=	 9;								//Length of the Initialize packet
	if (!remoteAddr)
		return 0;

	try
	{
		if ( in_Socks == NULL )							//If no socks server got passed...
		{
			sock.open((struct sockaddr_in*)remoteAddr, sizeof(SOCKADDR_IN));
			sock.connect();
		}
		else
		{
			sock.open((struct sockaddr_in*)in_Socks, socksLen);
			sock.connect();

			if ( strlen(m_sHttpProxy.szUserName) > 0 ) 
			{
				lPacketLen += (int)strlen(m_sHttpProxy.szUserName);				//If there is an extra userid, add its length to the packet length
			}
			char packet[MAX_PATH];				//Allocate a packet
			memset(packet, 0, MAX_PATH);
			packet[0] = 4;						//Socks version
			packet[1] = 1;						//Connect code

			memcpy( packet+2,
				(char *)&(((sockaddr_in *)remoteAddr)->sin_port),
				2 );										//Copy the port
			memcpy( packet+4,
				(char *)&(((sockaddr_in *)remoteAddr)->sin_addr.S_un.S_addr),
				4 );										//Copy the IP

			if ( strlen(m_sHttpProxy.szUserName) > 0 )									//If there was a userid, copy it now
			{
				memcpy( packet+8, m_sHttpProxy.szUserName, strlen(m_sHttpProxy.szUserName)+1 );
			}
			else		
			{
				packet[8]=0;
			}

			//Send the packet
			sock.write(packet, lPacketLen);
			//delete[] packet;								//Unallocate the packet
			char reply[8];									//Allocate memory for the reply packet
			memset( reply, 0 , 8 );							//Set it to 0
			long bytesRecv = 0;
			bytesRecv = sock.readUpto(reply, 8);		//Get the reply packet

			if ( reply[0] != 0)								//Reply code should be 0
			{ 
				// SOCKS: Reply code is invalid
				throw SockException2("Sock4 reply[0] code not equal 0", (int)reply[0]);
//				return WSAENETDOWN;
			}
			if ( reply[1] != 90 )
			{
				throw SockException2("Sock4 reply[1] code not equal 90", (int)reply[1]);
			}
		}
	}
	catch(SockException2 &e)
	{
		char strErr[128];
		memset(strErr, 0, 128);
		_snprintf(strErr, 127, "connect SOCK4 failed, %s", e.msg);
		throw SockException2(strErr, e.err);
	}

	return 0;
}

int	GenericHTTPClient::ConnectToSocks5(const struct sockaddr* remoteAddr, const struct sockaddr FAR *in_Socks,int socksLen)
{
	int retval	=	-1;
	int lPacketLen	=	 6;								//Length of the Initialize packet
	try
	{
		if ( in_Socks == NULL )							//If no socks server got passed...
		{
			sock.open((struct sockaddr_in*)remoteAddr, sizeof(SOCKADDR_IN));
			sock.connect();
			return 0;
		}
		sock.open((struct sockaddr_in*)in_Socks, socksLen);
		sock.connect();

		char reply[255];								//Allocate memory for the reply packet
		memset( reply, 0 , 255 );					//Set it to 0
		char packet[6];							//Allocate a packet
		packet[0] = 5;							//Socks version
		packet[1] = 2;									//No of methods
		packet[2] = 0;									//No auth required
		packet[3] = 2;									//for UserName and Password
		packet[4] = 3;									//chap
		packet[5] = 0;									

		//Send the packet
		long bytesSend = 0;
		sock.write(packet, 6);

		long bytesRecv = 0;
		bytesRecv = sock.readUpto(reply, 255);	//Get the reply packet

		//MessageBox(0,"Connected...","",NULL);
		if ( reply[0] != 5)								//Reply code should be 0
		{ 
			throw SockException2("Sock5 reply[0] code not equal 5", (int)reply[0]);
			// SOCKS: Reply code is invalid
			//return WSAENETDOWN;
		}

		if( reply[1] == 2 )
		{
			int nlen = 0;
			char msg[256];
			msg[0] = 0x01;
			msg[1] = (byte)(int)strlen(m_sHttpProxy.szUserName);
			strcpy((char*)&msg[2],m_sHttpProxy.szUserName);
			nlen = 2+msg[1];
			msg[nlen++] = (byte)(int)strlen(m_sHttpProxy.szPassword);
			strcpy((char*)&msg[nlen],m_sHttpProxy.szPassword);
			nlen += (int)strlen(m_sHttpProxy.szPassword);

			sock.write((char*)msg,nlen);
			sock.readUpto(reply, 255);
		}

		if ( reply[1] == 0 )							//Request granted!
		{				
			char packet1[12]; //Allocate a packet
			memset(packet1 , 0, 12);
			packet1[0] = 5; //Socks version
			packet1[1] = 1; //connect
			packet1[2] = 0; //reserved
			packet1[3] = 1; //for ip v4 address

			memcpy( packet1+4, //Copy the IP
				(char *)&(((sockaddr_in *)remoteAddr)->sin_addr.S_un.S_addr),
				4 );
			memcpy( packet1+8, //Copy the port
				(char *)&(((sockaddr_in *)remoteAddr)->sin_port),
				2 );

			//Send the packet
			bytesSend = 0;
			sock.write(packet1, 12 );

			memset( reply, 0 , 255 );						//Set it to 0
			sock.readUpto(reply, 255);				//Get the reply packet

			if ( reply[0] != 5)							//Reply code should be 0
			{ 
				throw SockException2("Sock5 reply[0] code not equal 5", (int)reply[0]);
				// SOCKS: Reply code is invalid
				//return WSAENETDOWN;
			}
			if ( reply[1] != 0 )						//Request granted!
			{
				throw SockException2("Sock5 reply[1] code not equal 0", (int)reply[1]);
				//return 0;
			}
		}
	}
	catch(SockException2 &e)
	{
		char strErr[128];
		memset(strErr, 0, 128);
		_snprintf(strErr, 127, "connect SOCK5 failed, %s", e.msg);
		throw SockException2(strErr, e.err);
	}
	return 0;
}
//关闭HTTP连接
BOOL GenericHTTPClient::Close( BOOL bTerm )
{
	string strTmp = m_strSrcURL;	
	BOOL	bRetValue = TRUE;
//	YL_Log( LOG_FILE,LOG_NOTICE,"Close","Enter close! %s,%u",strTmp.c_str() ,GetCurrentThreadId());

	sock.close();
	m_bGetHeader = FALSE;
	
	if( bTerm )
	{
		DWORD dwExitCode=0;
		if( m_curHTTP!= NULL && GetExitCodeThread(m_curHTTP,&dwExitCode) && dwExitCode == STILL_ACTIVE)
		{
//			YL_Log( LOG_FILE,LOG_NOTICE,"Close","In Terminate,%u" ,GetCurrentThreadId());
			if( WaitForSingleObject( m_curHTTP,1000) == WAIT_TIMEOUT )
			{
//				YL_Log( LOG_FILE,LOG_NOTICE,"HTTP Close","in" );
				TerminateThread(m_curHTTP,1);
//				YL_Log( LOG_FILE,LOG_NOTICE,"HTTP Close","out" );
				WaitForSingleObject(m_curHTTP,1000);
//				YL_Log( LOG_FILE,LOG_NOTICE,"HTTP Close","out2" );
			}
		}		
	}

//	YL_Log( LOG_FILE,LOG_NOTICE,"Close","OUT close%u" ,GetCurrentThreadId());
	return TRUE;
}

//解析HTTP请求
VOID GenericHTTPClient::parseURL( LPTSTR pszProtocol, LPTSTR pszAddress, unsigned short &uPort, LPTSTR pszURI )
{
	LPSTR	pszURL = NULL;	
	TCHAR	szPort[SIZE_SMALL_BUFFER]="";
	DWORD	dwPosition=0;
	BOOL	bFlag=FALSE;

	pszURL = new TCHAR[m_strSrcURL.size()+1];
	strcpy( pszURL,m_strSrcURL.c_str() );

	//get protocol
	while( strlen(pszURL)>0 && dwPosition< strlen(pszURL) 
		&& strncmp((pszURL+dwPosition), ":", 1))
	{
		++dwPosition;
	}

	if(!strncmp((pszURL+dwPosition+1), "/", 1))
	{	// is PROTOCOL
		if(pszProtocol)
		{
			strncpy(pszProtocol, pszURL, dwPosition);
			pszProtocol[dwPosition]=0;
		}
		bFlag=TRUE;
	}
	else
	{	// is HOST 
		if(pszProtocol)
		{
			strncpy(pszProtocol, "http", 4);
			pszProtocol[5]=0;
		}
	}

	//get Server Address
	DWORD dwStartPosition=0;
	if(bFlag)
	{
		dwStartPosition=dwPosition+=3;				
	}else
	{
		dwStartPosition=dwPosition=0;
	}

	bFlag=FALSE;
	while( strlen(pszURL)>0 && dwPosition<strlen(pszURL) 
		&& strncmp((pszURL+dwPosition), "/", 1))
	{
		++dwPosition;
	}

	DWORD dwFind=dwStartPosition;
	for(;dwFind<=dwPosition;dwFind++)
	{
		if(!strncmp((pszURL+dwFind), ":", 1))
		{ // find PORT
			bFlag=TRUE;
			break;
		}
	}

	if(bFlag)
	{
		TCHAR sztmp[SIZE_SMALL_BUFFER]="";
		strncpy(sztmp, (pszURL+dwFind+1), dwPosition-dwFind);
		uPort = atoi(sztmp);
		strncpy(pszAddress, (pszURL+dwStartPosition), dwFind-dwStartPosition);
	}
	else if(!strcmp(pszProtocol,"https"))
	{
		uPort = INTERNET_DEFAULT_HTTPS_PORT;
		strncpy(pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
	}
	else
	{
		uPort = INTERNET_DEFAULT_HTTP_PORT;
		strncpy(pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
	}

	if(dwPosition<strlen(pszURL))
	{ // find URI
		strncpy(pszURI, (pszURL+dwPosition), strlen(pszURL)-dwPosition);
	}
	else
	{
		pszURI[0]=0;
	}
	delete[] pszURL;
	return;
}

//从候选的服务器中选择最快的一个进行下载
BOOL GenericHTTPClient::SelectFastestServer( string& strURL )
{
	DWORD	dwThreadCount = (DWORD)m_vecCandidateURLs.size();
	HANDLE*  hThread = new HANDLE[dwThreadCount];
	HANDLE*  hThreadRemain = NULL;
	DWORD*   dwThreadID = new DWORD[dwThreadCount];		
	GenericHTTPClient* genHTTP = new GenericHTTPClient[dwThreadCount];

	//创建下载线程
	for( size_t idx = 0;idx < dwThreadCount;idx++ )
	{		
		genHTTP[idx].init();
		genHTTP[idx].m_sHttpProxy = this->m_sHttpProxy;	
		genHTTP[idx].m_strSrcURL  = m_vecCandidateURLs[idx];
		hThread[idx] = CreateThread(
			NULL,				// Pointer to thread security attributes
			0,					// Initial thread stack size, in bytes
			ServerTesting,	// Pointer to thread function
			&genHTTP[idx],				// The argument for the new thread
			0,					// Creation flags
			&dwThreadID[idx]    // Pointer to returned thread identifier
			);	
	}

	DWORD		dwRetValue = FALSE;
	DWORD		dwRes;
	DWORD		dwExitCode = -1;	
	DWORD		dwRetThread = 0;
	DWORD		dwRemainThread = dwThreadCount;
	vector<int> vecFailedThread;	

	while( true )
	{
		if( dwRetThread == dwThreadCount )
			//所有线程都已经返回,但没有合适的下载线程,返回
		{
//			YL_Log( LOG_FILE,LOG_NOTICE,"SelectFastestServer","SelectFastestServer: dwRetThread == dwThreadCount ");		
			goto clean;
		}
		dwRes = WaitForMultipleObjects( dwRemainThread,hThread,FALSE,30*1000 );
		if( dwRes == WAIT_FAILED || dwRes == WAIT_TIMEOUT )
		{
			goto clean;
		}
		vecFailedThread.clear();
		for( DWORD dwi = 0;dwi < dwRemainThread;dwi++ )
		{
			if( dwRes == WAIT_OBJECT_0 + dwi )
			{				
				dwRetThread++;
				vecFailedThread.push_back(dwi);
				if( GetExitCodeThread( hThread[dwi],&dwExitCode) && dwExitCode == 0 )
					//成功返回
				{					
					strURL = m_vecCandidateURLs[dwi];					
//					YL_Log( LOG_FILE,LOG_NOTICE,"SelectFastestServer","SelectFastestServer: %d Server is %s!",dwi,m_vecCandidateURLs[dwi].c_str());		
					dwRetValue = TRUE;
					genHTTP[dwi].Close();		
					goto clean;					
				}
				CloseHandle(hThread[dwi]);
			}
		}	
		//删除错误的http请求线程
		if( hThreadRemain != NULL )
		{
			delete[] hThreadRemain,hThreadRemain=NULL;
		}

		hThreadRemain = new HANDLE[dwRemainThread-vecFailedThread.size()];
		DWORD  dwIndex = 0;
		for( DWORD dwi = 0;dwi < dwRemainThread;dwi++ )
		{
			size_t idx = 0;
			for(;idx < vecFailedThread.size();idx++)
			{
				if( dwi == vecFailedThread[idx] )
				{
//					YL_Log( LOG_FILE,LOG_NOTICE,"SelectFastestServer","SelectFastestServer: %d Server is failed!",dwi);		
					break;
				}
			}
			if( idx == vecFailedThread.size() )
			{
				hThreadRemain[dwIndex] = hThread[dwi];
				dwIndex++;
			}
		}
		dwRemainThread -= (DWORD)vecFailedThread.size();
		if( hThread != NULL )
		{
			delete[] hThread,hThread=NULL;
		}
		hThread = new HANDLE[dwRemainThread];
		for(size_t idx = 0;idx < dwRemainThread;idx++ )
		{
			hThread[idx] = hThreadRemain[idx];
		}
	}
clean:
	//terminate all thread;	
	for( size_t idx = 0;idx < dwRemainThread;idx++ )
	{	
		DWORD dwStatus = 0;
		if( GetExitCodeThread( hThread[idx],&dwStatus) && dwStatus == STILL_ACTIVE )
		{				
			TerminateThread( hThread[idx],1 );
			WaitForSingleObject( hThread[idx],2000 );
		}
		CloseHandle( hThread[idx] );
		genHTTP[idx].Close();
	}
	delete[] hThread,hThread = NULL;
	delete[] dwThreadID,dwThreadID = NULL;
	delete[] genHTTP,genHTTP = NULL;
	if( hThreadRemain != NULL )
	{
		delete[] hThreadRemain,hThreadRemain = NULL;
	}
	return dwRetValue;
}

DWORD WINAPI GenericHTTPClient::ServerTesting( LPVOID pPar )
{	
	GenericHTTPClient* pGenHTTPClient =(GenericHTTPClient*) pPar;
	BOOL	bReturn = FALSE;
	if( bReturn = pGenHTTPClient->SendRequest() )
	{
//		YL_Log( LOG_FILE,LOG_NOTICE,"ServerTesting","ServerTesting: %s Connect is succeeded!",pGenHTTPClient->m_strSrcURL.c_str() );
		
		bReturn=pGenHTTPClient->GetHeader( );
		if(!bReturn)
		{			
//			YL_Log( LOG_FILE,LOG_WARNING,"ServerTesting","ServerTesting:%s GetHeader is failed!",pGenHTTPClient->m_strSrcURL.c_str() );			
			goto clean;
		}
//		YL_Log( LOG_FILE,LOG_NOTICE,"ServerTesting","ServerTesting: %s GetHeader is succeeded!",pGenHTTPClient->m_strSrcURL.c_str());		
	}
	else
	{
//		YL_Log( LOG_FILE,LOG_WARNING,"ServerTesting","ServerTesting:%s Connect is failed!", pGenHTTPClient->m_strSrcURL.c_str());		
		goto clean;
	}	
	//获得消息体
	//10k data to test

	pGenHTTPClient->GetContent(100000);
//	YL_Log( LOG_FILE,LOG_WARNING,"ServerTesting","ServerTesting:%s GetContentReturn!", pGenHTTPClient->m_strSrcURL.c_str());		

clean:	
	return bReturn?0:1;
}

//初始化成员变量
VOID GenericHTTPClient::init()
{
//	YL_Log( LOG_FILE,LOG_NOTICE,"init","int IN ");

	m_lpszPostArguments = NULL;
	m_memContent.buffer = NULL;
	m_memHeader.buffer  = NULL;
	m_strSrcURL			= "";	
	m_bGetHeader		= FALSE;
	m_bGetContent		= FALSE;

	m_dwConnectTime		= 0;
	m_dwReadTime		= 0;
}

VOID GenericHTTPClient::destroy()
{
//	YL_Log( LOG_FILE,LOG_NOTICE,"destroy","Enter destory!" );
	MemBufferFree(&m_memContent);
	MemBufferFree(&m_memHeader);
	if( m_lpszPostArguments )
	{
		delete[] m_lpszPostArguments,m_lpszPostArguments = NULL;
	}
}
void GenericHTTPClient::SendString(LPCSTR str)
{
	sock.write(str, (int)strlen(str));
}

void GenericHTTPClient::GetRequestTime(int* pUnConnectTime,int* pUnReadTime )
{
	if( pUnConnectTime != NULL )
	{
		*pUnConnectTime = (int)m_dwConnectTime;
	}
	if( pUnReadTime != NULL )
	{
		*pUnReadTime = (int)m_dwReadTime;
	}
}

void GenericHTTPClient::GetRequestIP(string& str)
{
	str = m_strIP;
}
