#ifndef __GENERIC_HTTP_CLIENT
#define __GENERIC_HTTP_CLIENT

#include "oiosocket2.h"
//#include "../../common/YL_Log.h"

#include <vector>
#include <string>
using namespace std;


#define SIZE_HTTP_RESPONSE_BUFFER	100000
#define SIZE_BUFFER					10240
#define SIZE_SMALL_BUFFER			256
#define SIZE_LARGE_BUFFER			1024

struct MemBuffer
{
	unsigned	char *buffer;
	unsigned	char *position;
	size_t		size;

	MemBuffer()
	{
		buffer = NULL;
		position = NULL;
		size	= 0;
	}
};



//class CLASSINDLL_CLASS_DECL GenericHTTPClient 
class GenericHTTPClient
{
public:						
	enum RequestMethod
		//request method type
	{														
		RequestGetMethod	= 0,
		RequestPostMethod	= 1	
	};

	enum HTTPProxyType
	{
		No_Proxy			= 0,
		IE_Auto_Proxy		= 1,
		HTTP_Proxy			= 2,
		Socks4_Proxy		= 3,
		Socks5_Proxy		= 4
	};

	struct HTTP_PROXY
		//store http proxy info
	{							
		char			szUserName[SIZE_SMALL_BUFFER];
		char			szPassword[SIZE_SMALL_BUFFER];
		int				iPort;		
		char			szProxyIP[SIZE_SMALL_BUFFER];
		HTTPProxyType	eHttpProxyType;			
	};

	GenericHTTPClient();
	virtual ~GenericHTTPClient();

	//Proxy Handler
	VOID			SetProxy( HTTPProxyType eProxyType,LPCSTR lpszProxyIP = NULL,
							  LPCSTR lpszUserName=NULL,LPCSTR lpszPassword=NULL,int iPort=80 );
	// HTTP Method handler 
	BOOL			Request( LPCTSTR lpszURL, int iMethod,int iTimeOut, LPBYTE lpPostArguments,DWORD dwPostArg);
	BOOL			SendRequest(LPCTSTR lpszURL, int iMethod,int iTimeOut, LPBYTE lpPostArguments,DWORD dwPostArg);

	//Set Candidate URLs
	VOID			SetCandidateURLs( LPTSTR* lplpszCandidateURLs,int iURLCount );
	BOOL			SelectFastestServer( string& strURL );
	
	//disconnect the connection
	BOOL			Close( BOOL bTerm = FALSE );

	static DWORD WINAPI  RealRequest( LPVOID pPar );
	static DWORD WINAPI	 ServerTesting( LPVOID pPar );
public:	
	VOID		init();
	VOID		destroy();
	VOID		parseURL(LPTSTR lpszProtocol, LPTSTR lpszAddress, unsigned short &uPort, LPTSTR lpszURI);

	void		MemBufferFree(MemBuffer*b);
	void		MemBufferCreate(MemBuffer *b);
	void		MemBufferGrow(MemBuffer *b);
	void		MemBufferAddByte(MemBuffer *b, unsigned char byt);
	void		MemBufferAddBuffer(MemBuffer *b, unsigned char *buffer, size_t size);
	void		SendString(LPCSTR str);

	//Main	Step
	BOOL		SendRequest(DWORD dwFileSize = 0);
	BOOL		GetHeader();
	BOOL		GetContent(long dwContentSize = -1);
	// Connection handler	
	BOOL		Connect(const char* ipstr, unsigned short port);
	int			ConnectToSocks4(const struct sockaddr* remoteAddr,	const struct sockaddr FAR *in_Socks,int socksLen);
	int			ConnectToSocks5(const struct sockaddr* remoteAddr,	const struct sockaddr FAR *in_Socks,int socksLen);
	BOOL		GetIEProxy();

	void		GetRequestTime(int* pnConnectTime,int* pnReadTime );
	void		GetRequestIP(string& str);
		
public:				

	HTTP_PROXY		m_sHttpProxy;			//proxy struct
	OIOSocketEx2	sock;					//sock object
	int				m_iCurrentMethod;		//GET or POST
	string			m_strSrcURL;			//HTTP URL
	MemBuffer		m_memHeader;			//HTTP Header
	MemBuffer		m_memContent;			//HTTP Conten
	vector<string> m_vecCandidateURLs;		//candidate URL vector
	BYTE*			m_lpszPostArguments;	//post arguments	
	DWORD			m_dwPostLen;
	DWORD			m_dwStatusCode;			//HTTP return code	
	HANDLE			m_curHTTP;				//thread handler
	BOOL			m_bGetHeader;			//是否已经取过HTTP头　
	BOOL			m_bGetContent;			//是否已经取过HTTp体
	BOOL			m_bSendRequest;			//是否采用SendRequest方式

	DWORD			m_dwConnectTime;
	DWORD			m_dwReadTime;
	string			m_strIP;
};

#endif	// #ifndef __GENERIC_HTTP_CLIENT
