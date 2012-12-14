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
	bool	SetProxy( YL_CHTTPRequest::PROXY_TYPE enumProxyType,const string* pStrProxyIP = 0,const string* pStrUserName = 0,const string* pStrPassword = 0,int iPort = 80 );

	//**************************************************************************//
	//																			//
	//	内容请求接口,该函数在返回时，已经完成了整个HTTP请求过程，即HTTP			//
	//	消息头和HTTP消息体都已经拿到了．										//
	//	如果想分步完成HTTP请求，需要调用SendRequest,然后再分别调用GetHeader和	//
	//	GetContent																//
	//																			//
	//  参数:																	//
	//		strURL		: HTTP请求的URL											//						
	//		enumRequest	: HTTP请求的类型										//
	//		lTimeout	: 该次请求的超时时间,-1表示不限制超时时间				//
	//		pStrPosArgu	: 如果请求为POST类型,POST内容							//
	//																			//
	// 返回值：																	//
	//		true ：	请求成功，连接成功，且HTTP 返回码为200,206或者306			//		
	//		false：	请求失败													//
	//																			//
	//**************************************************************************//
	bool	Request( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout = -1,unsigned char* pPosArgu = 0,unsigned int unPosLen = 0);

	//**************************************************************************//
	//																			//
	//	内容请求接口,但只发送请求，并不包含取回HTTP头和HTTP体					//															//
	//  参数:																	//
	//		strURL		: HTTP请求的URL											//						
	//		enumRequest	: HTTP请求的类型										//
	//		lTimeout	: 该次请求的超时时间,-1表示不限制超时时间				//
	//		pStrPosArgu	: 如果请求为POST类型,POST内容							//
	//																			//
	// 返回值：																	//
	//		true ：	请求成功，连接成功，且HTTP 返回码为200,206或者306			//		
	//		false：	请求失败													//
	//																			//
	//**************************************************************************//
	bool	SendRequest( const string& strURL,YL_CHTTPRequest::REQUEST_TYPE enumRequest ,long lTimeout = -1,unsigned char* pPosArgu = 0,unsigned int unPosLen = 0);

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
	//	 *	如果之前调用的是Request,则此时HTTP头已经获取完成，该接口仅仅返回	//
	//		获得的内容															//
	//	 *	如果之前调用SendRequest，则该接口会执行真正的获取HTTP头的过程		//
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
	//	获得HTTP请求返回的HTTP头中某一Item具体内容								//
	//																			//
	//  参数:																	//
	//		pszItemName	:	Item名称											//
	//		strItemName	:	Item名称											//
	//	  szItemContent	:	Item内容											//
	//		unBuffLen	:	Buffer长度											//
	//		strContent	:	Item内容											//
	//																			//
	// 返回值：																	//
	//			  true	:	获得成功											//
	//			  false :	获得失败											//
	//																			//
	//**************************************************************************//
	bool	GetHeaderItem( const char* pszItemName,char szItemContent[],unsigned int unBuffLen );
	bool	GetHeaderItem( const string& strItemName,string& strContent );

	//**************************************************************************//
	//																			//
	//	获得HTTP请求返回的HTTP体												//
	//	 *	如果之前调用的是Request,则此时HTTP体已经获取完成，该接口仅仅返回	//
	//		获得的内容															//
	//	 *	如果之前调用SendRequest，则该接口会执行真正的获取HTTP体的过程		//		
	//																			//
	//  参数:																	//
	//		lContenLen	:	HTTP体的长度，										//
	//						传入值表明需要获取的HTTP体的长度，					//
	//						传出值表明获取的HTTP体的长度						//
	//																			//
	// 返回值：																	//
	//		* 如果体内容无效，将会返回空指针，否则返回内容指针。				//
	//		* 如果返回的lContentLen小于传入的值，则表示HTTP体已经获取完毕		//
	//		注意：不要将HTTP体的内容指针释放，由DLL在Release中释放。			//
	//																			//
	//**************************************************************************//
	unsigned char*	GetContent( long& lContentLen );

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
	//	给定一组Server的URL，找出速度最快的服务器								//
	//	要求每个服务器至少有个大于10k的index.html文件，做为测速用				//
	//																			//
	//																			//
	//  参数:																	//
	//		vecStrServers : 服务器列表											//
	//		strSelectedServer : 返回的最快的服务器								//
	// 返回值：																	//
	//		true	: 测速成功													//
	//		false	: 测速失败													//
	//																			//
	//**************************************************************************//

	bool	SelectFastServer( const vector<string>& vecStrSevers,string& strSelectedServer );

	//**************************************************************************//
	//																			//
	//	得到一次HTTP请求的连接和读数据时间										//
	//																			//
	//																			//
	//  参数:																	//
	//		pUnConnectTime	: 连接时间											//
	//		pUnReadTime		: 返回的最快的服务器								//
	//																			//
	//**************************************************************************//
	void	GetRequestTime( int* pnConnectTime,int* pnReadTime );
	void	GetRequestIP(string&);
private:

	GenericHTTPClient* m_pHTTPClient;

};

#endif