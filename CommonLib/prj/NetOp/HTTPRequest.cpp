#include "../../include/YL_HTTPRequest.h"
#include "GenericHTTPClient.h"
#include "../../include/YL_HTTPRequestExport.h"


YL_CHTTPRequest::YL_CHTTPRequest()
{
	m_pHTTPClient = new GenericHTTPClient();
}

YL_CHTTPRequest::~YL_CHTTPRequest()
{
	if( m_pHTTPClient != NULL )
	{
		delete m_pHTTPClient;
	}
}

bool YL_CHTTPRequest::SetProxy( YL_CHTTPRequest::PROXY_TYPE enumProxyType,const string* pStrProxyIP ,const string* pStrUserName ,const string* pStrPassword ,int iPort )
{
	GenericHTTPClient::HTTPProxyType httpProxy;
	switch( enumProxyType )
	{
	case YL_CHTTPRequest::HTTP_PROXY:
		httpProxy = GenericHTTPClient::HTTP_Proxy;
		break;
	case YL_CHTTPRequest::NO_PROXY:
		httpProxy = GenericHTTPClient::No_Proxy;
		break;
	case YL_CHTTPRequest::IE_AUTO_PROXY:
		httpProxy = GenericHTTPClient::IE_Auto_Proxy;
		break;
	case YL_CHTTPRequest::SOCKS4_PROXY:
		httpProxy = GenericHTTPClient::Socks4_Proxy;
		break;
	case YL_CHTTPRequest::SOCKS5_PROXY:
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


bool YL_CHTTPRequest::Request( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout,unsigned char* pPosArgu,unsigned int unPosLen )
{
	try
	{	
		return m_pHTTPClient->Request( strURL.c_str(),(int)enumRequest,lTimeout,pPosArgu,unPosLen) == TRUE;	
	}
	catch(...)
	{
		//YL_Log( "log_http.txt",LOG_NOTICE,"Request","YL_Request,exception");
		return false;
	}
}

bool YL_CHTTPRequest::SendRequest( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout,unsigned char* pPosArgu,unsigned int unPosLen)
{
	return m_pHTTPClient->SendRequest( strURL.c_str(),(int)enumRequest,lTimeout,pPosArgu,unPosLen) == TRUE;
}

bool YL_CHTTPRequest::GetReturnCode( long& lCode )
{
	lCode = m_pHTTPClient->m_dwStatusCode;
	return true;
}

unsigned char* YL_CHTTPRequest::GetHeader( long& lHeadLen )
{
	if( m_pHTTPClient->GetHeader() )
	{
		lHeadLen = (long)( m_pHTTPClient->m_memHeader.position - m_pHTTPClient->m_memHeader.buffer);
		return m_pHTTPClient->m_memHeader.buffer;
	}
	return NULL;
}

bool YL_CHTTPRequest::GetHeaderItem( const char* pszItemName,char szItemContent[],unsigned int unBuffLen )
{
	if( pszItemName == NULL )
	{
		return false;
	}
	string strcotent;
	if( GetHeaderItem( string(pszItemName),strcotent) )
	{
		if( strcotent.size() >= unBuffLen )
		{
			return false;
		}
		strcpy(szItemContent,strcotent.c_str());
		return true;
	}
	return false;	
}

bool YL_CHTTPRequest::GetHeaderItem( const string& strItemName,string& strContent )
{
	long lHeadLen = 0;
	unsigned char* pszHead = GetHeader( lHeadLen );
	if( pszHead != NULL && lHeadLen > 0)
	{
		char* pTmpBuf = new char[lHeadLen+1];
		memcpy(pTmpBuf,pszHead,lHeadLen);
		pTmpBuf[lHeadLen] = '\0';

		char* pPos = strstr( pTmpBuf,strItemName.c_str());
		if( pPos == NULL )
		{
			delete pTmpBuf;
			return false;
		}
		pPos = strstr( pPos+1,":");
		if( pPos == NULL )
		{
			delete pTmpBuf;
			return false;
		}
		while (*++pPos)
		{
			if (*pPos != ' ')
				break;
		}
		char* pPos2 = strstr( pPos,"\r\n");
		*pPos2 = '\0';
		strContent = pPos;
		delete[] pTmpBuf;
		return true;
	}
	return false;
}

unsigned char* YL_CHTTPRequest::GetContent( long& lContentLen )
{
	if( m_pHTTPClient->GetContent( lContentLen ) )
	{
		lContentLen = (long)( m_pHTTPClient->m_memContent.position-m_pHTTPClient->m_memContent.buffer);
		return m_pHTTPClient->m_memContent.buffer;
	}
	return NULL;
}

bool YL_CHTTPRequest::Disconnect()
{
	return m_pHTTPClient->Close( TRUE ) == TRUE;
}

bool YL_CHTTPRequest::SelectFastServer( const vector<string>& vecStrSevers,string& strSelectedServer )
{	
	
	LPTSTR* b = new LPTSTR[vecStrSevers.size()];
	for( size_t idx = 0;idx < vecStrSevers.size();idx++ )
	{
		b[idx] = new TCHAR[vecStrSevers[idx].size()+1];
		strcpy(b[idx],vecStrSevers[idx].c_str());
	}

	m_pHTTPClient->SetCandidateURLs(b,(int)(vecStrSevers.size()));

	BOOL	bRet = FALSE;
	bRet = m_pHTTPClient->SelectFastestServer( strSelectedServer );
	for( size_t idx = 0;idx < vecStrSevers.size();idx++ )
	{
		delete[] b[idx];
	}
	delete[] b;
	
	return bRet == TRUE ;
	
	return true;
}


void YL_CHTTPRequest::GetRequestTime( int* pnConnectTime,int* pnReadTime )
{
	m_pHTTPClient->GetRequestTime( pnConnectTime,pnReadTime );
}
void YL_CHTTPRequest::GetRequestIP(string& str)
{
	m_pHTTPClient->GetRequestIP( str );
}
