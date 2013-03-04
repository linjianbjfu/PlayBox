#ifndef _YL_HTTPREQUEST_H
#define _YL_HTTPREQUEST_H

#ifdef	YL_HTTPREQUEST_EXPORT
#define YL_HTTPREQUEST_DLL __declspec(dllexport)
#else
#define YL_HTTPREQUEST_DLL __declspec(dllimport)
#endif

#include <string>
#include <vector>
using namespace std;

class GenericHTTPClient;

class YL_HTTPREQUEST_DLL YL_CHTTPRequest
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

	enum REQUEST_TYPE
	{
		REQUEST_GET		= 0,
		REQUEST_POST	= 1
	};

public:
	YL_CHTTPRequest();
	~YL_CHTTPRequest();

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
	bool	SetProxy( YL_CHTTPRequest::PROXY_TYPE enumProxyType,const string* pStrProxyIP = 0,const string* pStrUserName = 0,const string* pStrPassword = 0,int iPort = 80 );

	//**************************************************************************//
	//																			//
	//	��������ӿ�,�ú����ڷ���ʱ���Ѿ����������HTTP������̣���HTTP			//
	//	��Ϣͷ��HTTP��Ϣ�嶼�Ѿ��õ��ˣ�										//
	//	�����ֲ����HTTP������Ҫ����SendRequest,Ȼ���ٷֱ����GetHeader��	//
	//	GetContent																//
	//																			//
	//  ����:																	//
	//		strURL		: HTTP�����URL											//						
	//		enumRequest	: HTTP���������										//
	//		lTimeout	: �ô�����ĳ�ʱʱ��,-1��ʾ�����Ƴ�ʱʱ��				//
	//		pStrPosArgu	: �������ΪPOST����,POST����							//
	//																			//
	// ����ֵ��																	//
	//		true ��	����ɹ������ӳɹ�����HTTP ������Ϊ200,206����306			//		
	//		false��	����ʧ��													//
	//																			//
	//**************************************************************************//
	bool	Request( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout = -1,unsigned char* pPosArgu = 0,unsigned int unPosLen = 0);

	//**************************************************************************//
	//																			//
	//	��������ӿ�,��ֻ�������󣬲�������ȡ��HTTPͷ��HTTP��					//															//
	//  ����:																	//
	//		strURL		: HTTP�����URL											//						
	//		enumRequest	: HTTP���������										//
	//		lTimeout	: �ô�����ĳ�ʱʱ��,-1��ʾ�����Ƴ�ʱʱ��				//
	//		pStrPosArgu	: �������ΪPOST����,POST����							//
	//																			//
	// ����ֵ��																	//
	//		true ��	����ɹ������ӳɹ�����HTTP ������Ϊ200,206����306			//		
	//		false��	����ʧ��													//
	//																			//
	//**************************************************************************//
	bool	SendRequest( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout = -1,unsigned char* pPosArgu = 0,unsigned int unPosLen = 0);

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
	//	 *	���֮ǰ���õ���Request,���ʱHTTPͷ�Ѿ���ȡ��ɣ��ýӿڽ�������	//
	//		��õ�����															//
	//	 *	���֮ǰ����SendRequest����ýӿڻ�ִ�������Ļ�ȡHTTPͷ�Ĺ���		//
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
	//	���HTTP���󷵻ص�HTTPͷ��ĳһItem��������								//
	//																			//
	//  ����:																	//
	//		pszItemName	:	Item����											//
	//		strItemName	:	Item����											//
	//	  szItemContent	:	Item����											//
	//		unBuffLen	:	Buffer����											//
	//		strContent	:	Item����											//
	//																			//
	// ����ֵ��																	//
	//			  true	:	��óɹ�											//
	//			  false :	���ʧ��											//
	//																			//
	//**************************************************************************//
	bool	GetHeaderItem( const char* pszItemName,char szItemContent[],unsigned int unBuffLen );
	bool	GetHeaderItem( const string& strItemName,string& strContent );

	//**************************************************************************//
	//																			//
	//	���HTTP���󷵻ص�HTTP��												//
	//	 *	���֮ǰ���õ���Request,���ʱHTTP���Ѿ���ȡ��ɣ��ýӿڽ�������	//
	//		��õ�����															//
	//	 *	���֮ǰ����SendRequest����ýӿڻ�ִ�������Ļ�ȡHTTP��Ĺ���		//		
	//																			//
	//  ����:																	//
	//		lContenLen	:	HTTP��ĳ��ȣ�										//
	//						����ֵ������Ҫ��ȡ��HTTP��ĳ��ȣ�					//
	//						����ֵ������ȡ��HTTP��ĳ���						//
	//																			//
	// ����ֵ��																	//
	//		* �����������Ч�����᷵�ؿ�ָ�룬���򷵻�����ָ�롣				//
	//		* ������ص�lContentLenС�ڴ����ֵ�����ʾHTTP���Ѿ���ȡ���		//
	//		ע�⣺��Ҫ��HTTP�������ָ���ͷţ���DLL��Release���ͷš�			//
	//																			//
	//**************************************************************************//
	unsigned char*	GetContent( long& lContentLen );

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
	//	����һ��Server��URL���ҳ��ٶ����ķ�����								//
	//	Ҫ��ÿ�������������и�����10k��index.html�ļ�����Ϊ������				//
	//																			//
	//																			//
	//  ����:																	//
	//		vecStrServers : �������б�											//
	//		strSelectedServer : ���ص����ķ�����								//
	// ����ֵ��																	//
	//		true	: ���ٳɹ�													//
	//		false	: ����ʧ��													//
	//																			//
	//**************************************************************************//

	bool	SelectFastServer( const vector<string>& vecStrSevers,string& strSelectedServer );

	//**************************************************************************//
	//																			//
	//	�õ�һ��HTTP��������ӺͶ�����ʱ��										//
	//																			//
	//																			//
	//  ����:																	//
	//		pUnConnectTime	: ����ʱ��											//
	//		pUnReadTime		: ���ص����ķ�����								//
	//																			//
	//**************************************************************************//
	void	GetRequestTime( int* pnConnectTime,int* pnReadTime );
	void	GetRequestIP(string&);
private:

	GenericHTTPClient* m_pHTTPClient;

};

#endif