
#include "oiosocket2.h"
#include "stdio.h"

OIOSocketEx2::~OIOSocketEx2()
{
	if (exitEvent != INVALID_HANDLE_VALUE)
	{
		::WSACloseEvent(exitEvent);
		exitEvent = INVALID_HANDLE_VALUE;
	}

	if (m_WriteEventArray[0] != INVALID_HANDLE_VALUE )
	{	
		::WSACloseEvent(m_WriteEventArray[0]);
		m_WriteEventArray[0] = INVALID_HANDLE_VALUE;
	}

	if (m_ReadEventArray[0] != INVALID_HANDLE_VALUE)
	{
		::WSACloseEvent(m_ReadEventArray[0]);
		m_ReadEventArray[0] = INVALID_HANDLE_VALUE;
	}

	close();
	
}

void OIOSocketEx2::init()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
    
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
		throw SockException2("Init winsock dll failed", err);
}

void OIOSocketEx2::setBlocking(bool yes)
{
	unsigned long op = yes ? 0 : 1;
	//设置缓存区大小
	int bufsize = MAXBUFFERSIZE;
	if (setsockopt(sockNum, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize)) == SOCKET_ERROR )
		throw SockException2("Set sock recvbuf failed", ::WSAGetLastError());
	if ( setsockopt(sockNum, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize)) == SOCKET_ERROR)
		throw SockException2("Send sock sndbuf failed", ::WSAGetLastError());
	if (ioctlsocket(sockNum, FIONBIO, &op) == SOCKET_ERROR)
		throw SockException2("Set sock blocking failed", ::WSAGetLastError());
}

void OIOSocketEx2::setReuse(bool yes)
{
	unsigned long op = yes ? 1 : 0;
	if (::setsockopt(sockNum, SOL_SOCKET,SO_REUSEADDR,(char *)&op,sizeof(unsigned long)) == SOCKET_ERROR) 
		throw SockException2("Set sockaddr reuse failed", ::WSAGetLastError());
}


void OIOSocketEx2::setNagle(bool on)
{
	int nodelay = (on==false);
	if (setsockopt(sockNum, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(nodelay)) == SOCKET_ERROR) 
		throw SockException2("Set sock NODELAY failed", ::WSAGetLastError());
}


void	OIOSocketEx2::setLinger(int sec)
{
	linger linger;
	linger.l_onoff = (sec>0)?1:0;
	linger.l_linger = sec;

	if (setsockopt(sockNum, SOL_SOCKET, SO_LINGER, (const char *)&linger, sizeof (linger)) == -1) 
		throw SockException2("Set LINGER failed", ::WSAGetLastError());
}


void	OIOSocketEx2::open(unsigned long Ip, unsigned short port)
{
	sockNum = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (sockNum == INVALID_SOCKET)
	{
		throw SockException2("Create socket failed", ::WSAGetLastError());
	}

	setBlocking(false);
#ifdef DISABLE_NAGLE
	setNagle(false);	//设置成立即发送
#endif

	::ResetEvent(exitEvent);

	memset(&remoteAddr,0,sizeof(remoteAddr));
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	remoteAddr.sin_addr.S_un.S_addr = htonl(Ip);
}

/**
* 根据主机名获取IP
*/
unsigned int OIOSocketEx2::getIP(const char *name)
{
	if (!name)
	{
		return 0;
	}

	unsigned int ip = inet_addr(name);
	if (ip == INADDR_NONE)
	{
		HOSTENT* he = ::gethostbyname(name);
		if (he)
		{
			LPSTR lpAddr = he->h_addr_list[0];
			if (lpAddr)
			{
				struct in_addr  inAddr;
				memmove (&inAddr, lpAddr, 4);

				ip =  inAddr.S_un.S_un_b.s_b1<<24 |
					inAddr.S_un.S_un_b.s_b2<<16 |
					inAddr.S_un.S_un_b.s_b3<<8 |
					inAddr.S_un.S_un_b.s_b4;

				char sz[128];
				sprintf(sz,"%d.%d.%d.%d",inAddr.S_un.S_un_b.s_b1,inAddr.S_un.S_un_b.s_b2,inAddr.S_un.S_un_b.s_b3,inAddr.S_un.S_un_b.s_b4);
				strIP = sz;
				
			}else
				ip = 0;
		} else
			ip = 0;
	}
	else
		ip = htonl(ip);

	return ip;
}

void	OIOSocketEx2::open(const struct sockaddr_in *in_Socks, int socksLen)
{
	if (!in_Socks)
		return;
	sockNum = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (sockNum == INVALID_SOCKET)
	{
		throw SockException2("Create socket failed", ::WSAGetLastError());
	}

	setBlocking(false);
#ifdef DISABLE_NAGLE
	setNagle(false);	//设置成立即发送
#endif

	::ResetEvent(exitEvent);

	memcpy(&remoteAddr, in_Socks, socksLen);
}

void OIOSocketEx2::open(const char* lpszIp, unsigned short port)
{
	sockNum = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (sockNum == INVALID_SOCKET)
	{
		throw SockException2("Create socket failed", ::WSAGetLastError());
	}

	setBlocking(false);
#ifdef DISABLE_NAGLE
	setNagle(false);	//设置成立即发送
#endif

	::ResetEvent(exitEvent);

	memset(&remoteAddr,0,sizeof(remoteAddr));

	if (!strlen(lpszIp))
	{
		throw SockException2("lpszIp is NULL");
	}

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	remoteAddr.sin_addr.S_un.S_addr = htonl(OIOSocketEx2::getIP(lpszIp));
}

void OIOSocketEx2::bind(const unsigned short port)
{
	struct sockaddr_in localAddr;
	sockNum = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (sockNum == INVALID_SOCKET)
	{
		throw SockException2("Create socket failed", ::WSAGetLastError());
	}

	setBlocking(false);
	setReuse(true);
	memset(&localAddr,0,sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(port);
	localAddr.sin_addr.s_addr = INADDR_ANY;

	if( ::bind (sockNum, (sockaddr *)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
		throw SockException2("bind failed", WSAGetLastError());

	if (::listen(sockNum, SOMAXCONN) == SOCKET_ERROR)
		throw SockException2("listen failed", WSAGetLastError());
}


// --------------------------------------------------
/**
 * 检测是否超时
 * \param r 是否为读
 * \param w 是否为写
 */
void OIOSocketEx2::checkTimeout(bool r, bool w)
{
	int err = WSAGetLastError();
	if (err == WSAEWOULDBLOCK)
	{
		timeval timeout;
		fd_set read_fds;
		fd_set write_fds;

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		FD_ZERO (&write_fds);
		if (w)
		{
			timeout.tv_sec = (int)this->writeTimeout/1000;
			FD_SET (sockNum, &write_fds);
		}

		FD_ZERO (&read_fds);
		if (r)
		{
			timeout.tv_sec = (int)this->readTimeout/1000;
			FD_SET (sockNum, &read_fds);
		}

		timeval *tp;
		if (timeout.tv_sec)
			tp = &timeout;
		else
			tp = NULL;

		int r = select (NULL, &read_fds, &write_fds, NULL, tp);

		if (r == 0)
			throw TimeoutException2("Connect timeout");
		 if (r == SOCKET_ERROR)
			throw SockException2("select failed", ::WSAGetLastError());

	}else{
		throw SockException2("Connect failed", err);
	}
}


void	OIOSocketEx2::connect()
{
	if (::connect(sockNum,(struct sockaddr *)&remoteAddr,sizeof(remoteAddr)) == SOCKET_ERROR)
	{
		checkTimeout(false,true);
	}
}


int OIOSocketEx2::readUpto(void *pBuffer, int len)
{
	int bytesRead=0;
	while(len)
	{
		m_ReadDataBuf.len = len;
		m_ReadDataBuf.buf = (char *)pBuffer; 
		DWORD dwReadBytes = 0;
		DWORD Flags = 0;
		ZeroMemory(&m_ReadOverlapped, sizeof(WSAOVERLAPPED));
		m_ReadOverlapped.hEvent = m_ReadEventArray[0];		
		if(WSARecv(sockNum, &m_ReadDataBuf, 1, &dwReadBytes, &Flags, &m_ReadOverlapped, NULL) == SOCKET_ERROR)
		{ 
			int err = ::WSAGetLastError();

			if(err != WSA_IO_PENDING)    
			{
				throw SockException2("Recv error", err);
			}else
			{
				DWORD res = ::WSAWaitForMultipleEvents(2, m_ReadEventArray, FALSE, readTimeout, FALSE);
				if ( res == WSA_WAIT_TIMEOUT)
				{
					throw TimeoutException2("Read data timeout");
				}
				else if (res == WSA_WAIT_FAILED )
				{
					throw SockException2("WSA_WAIT_FAILED", ::WSAGetLastError());
				}
				else if (res - WSA_WAIT_EVENT_0 == 1)
				{
					throw SockException2("Socket is close", res);
				}
				//检测异步读数据的状态
				BOOL bResult = ::WSAGetOverlappedResult(sockNum, 	//要检测的Socket
					&m_ReadOverlapped, 							//overlapped 结构
					&dwReadBytes, 						//已经读取的字节数
					TRUE,								//是否要等到读结束才返回
					&Flags);
				if (!bResult)
					throw SockException2("WSAGetOverlappedResult failed", ::WSAGetLastError());
				if (dwReadBytes == 0)
				{//Socket已经关闭
					break;
					//throw SockException2("Remote peer close socket", ::WSAGetLastError());
				}
			}
		}

		if (dwReadBytes == 0)
			break;

		::WSAResetEvent(m_ReadEventArray[0]);

		bytesRead += dwReadBytes;
		len -= dwReadBytes;
		pBuffer = (char *)pBuffer + dwReadBytes;
	}

	return bytesRead;
}


int OIOSocketEx2::read(void * pBuffer, int len)
{
	DWORD bytesRead = 0;

	while(len)
	{
		m_ReadDataBuf.len = len;
		m_ReadDataBuf.buf = (char *)pBuffer; 
		DWORD dwReadBytes = 0;
		DWORD Flags = 0;
		ZeroMemory(&m_ReadOverlapped, sizeof(WSAOVERLAPPED));
		m_ReadOverlapped.hEvent = m_ReadEventArray[0];		
		if(WSARecv(sockNum, &m_ReadDataBuf, 1, &dwReadBytes, &Flags, &m_ReadOverlapped, NULL) == SOCKET_ERROR)
		{ 
			int err = ::WSAGetLastError();

			if(err != WSA_IO_PENDING)    
			{
				throw SockException2("Recv error", err);
			}else
			{
				DWORD res = ::WSAWaitForMultipleEvents(2, m_ReadEventArray, FALSE, readTimeout, FALSE);
				if ( res == WSA_WAIT_TIMEOUT)
				{
					throw TimeoutException2("Read data timeout");
				}
				else if (res == WSA_WAIT_FAILED )
				{
					throw SockException2("WSA_WAIT_FAILED", ::WSAGetLastError());
				}
				else if (res - WSA_WAIT_EVENT_0 == 1)
				{
					throw SockException2("Socket is close", res);
				}
				//检测异步读数据的状态
				BOOL bResult = ::WSAGetOverlappedResult(sockNum, 	//要检测的Socket
					&m_ReadOverlapped, 							//overlapped 结构
					&dwReadBytes, 						//已经读取的字节数
					TRUE,								//是否要等到读结束才返回
					&Flags);
				if (!bResult)
					throw SockException2("WSAGetOverlappedResult failed", ::WSAGetLastError());
				if (dwReadBytes == 0)
				{//Socket已经关闭
					//break;
					throw SockException2("Remote peer close socket", ::WSAGetLastError());
				}
			}
		}

		::WSAResetEvent(m_ReadEventArray[0]);

		if (dwReadBytes == 0)
			throw SockException2("Remote peer close socket");

		bytesRead += dwReadBytes;
		len -= dwReadBytes;
		pBuffer = (char *)pBuffer + dwReadBytes;
	}

	return bytesRead;
}

// --------------------------------------------------
void OIOSocketEx2::write(const void *pBuffer, int len)
{
	DWORD bytesWrite = 0;

	while(len)
	{
		m_WriteDataBuf.len = len;
		m_WriteDataBuf.buf = (char *)pBuffer; 
		DWORD dwWriteBytes = 0;
		DWORD Flags = 0;
		ZeroMemory(&m_WriteOverlapped, sizeof(WSAOVERLAPPED));
		m_WriteOverlapped.hEvent = m_WriteEventArray[0];		

		if(::WSASend(sockNum, &m_WriteDataBuf, 1, &dwWriteBytes, Flags, &m_WriteOverlapped, NULL) == SOCKET_ERROR)
		{ 
			int err = ::WSAGetLastError();

			if(err != WSA_IO_PENDING)    
			{
				throw SockException2("Send data error", ::WSAGetLastError());
			}else
			{
				DWORD res = ::WSAWaitForMultipleEvents(2, m_WriteEventArray, FALSE, readTimeout, FALSE);
				
				if ( res == WSA_WAIT_TIMEOUT)
				{
					throw TimeoutException2("Send data timeout");
				}
				else if (res == WSA_WAIT_FAILED )
				{
					throw SockException2("WSA_WAIT_FAILED", ::WSAGetLastError());
				}
				else if (res - WSA_WAIT_EVENT_0 == 1)
				{
					throw SockException2("Socket is close", res);
				}
				//检测异步读数据的状态
				BOOL bResult = ::WSAGetOverlappedResult(sockNum, 	//要检测的Socket
					&m_WriteOverlapped, 							//overlapped 结构
					&dwWriteBytes, 						//已经读取的字节数
					TRUE,								//是否要等到读结束才返回
					&Flags);
				
				if (!bResult)
					throw SockException2("WSAGetOverlappedResult failed", ::WSAGetLastError());

				if (dwWriteBytes == 0)
				{
					throw SockException2("Remote peer close socket", err);
				}
			}
		}

		::WSAResetEvent(m_WriteEventArray[0]);

		bytesWrite += dwWriteBytes;
		len -= dwWriteBytes;
		pBuffer = (char *)pBuffer + dwWriteBytes;
	}

}


void OIOSocketEx2::close()
{
	if (exitEvent != INVALID_HANDLE_VALUE)
	{
		::SetEvent(exitEvent);
	}

	if (m_WriteEventArray[0] != INVALID_HANDLE_VALUE)
		::WSAResetEvent(m_WriteEventArray[0]);

	if (m_ReadEventArray[0] != INVALID_HANDLE_VALUE)
		::WSAResetEvent(m_ReadEventArray[0]);

	if (sockNum)
	{
		::shutdown(sockNum, SD_SEND);
		setTimeout(2000, 2000);
		::closesocket(sockNum);
		sockNum=0;
	}
}


int OIOSocketEx2::recvPacket(char * & buf, int len)
{
	bool bInterBuf = false;
	if (len == 0) 
		bInterBuf = true;

	char* pbuf = new char[sizeof(len)];
	//接收数据长度
	int rt = read(pbuf, sizeof(len));
	if (rt != sizeof(len))
	{
		delete[] pbuf;
		return SOCKET_ERROR;
	}
	
	memcpy((void*)&len, pbuf, sizeof(len));
	delete[] pbuf;

	if (len > 0x1000000)
		throw SockException2("recvPacket len err");
	
	if (bInterBuf) {
		if (len <= 0)
			return 0;
	    buf = new char[len+1]; //should be delete outside
	}

	rt = read(buf, len);
	if (len == rt)
		return len;
	else
	{
		delete[] buf;
		return SOCKET_ERROR;
	}
}

int OIOSocketEx2::sendPacket(const char* buf, int len)
{
    char* pbuf = new char[sizeof(len)];
    memcpy(pbuf, (void*)&len, sizeof(len));
    //发送数据长度
    write(pbuf, sizeof(len));
    delete[] pbuf;
    write(buf, len);
    return len;
}


void OIOSocketEx2::setTimeout(unsigned long wt, unsigned long rt)
{
	readTimeout = rt;
	writeTimeout = wt;
}