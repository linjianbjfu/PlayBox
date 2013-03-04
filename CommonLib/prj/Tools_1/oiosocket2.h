#ifndef _OIOSOCKETEX2_H_
#define _OIOSOCKETEX2_H_

#include <winsock2.h>
#include <string>
using std::string;

#define MAXBUFFERSIZE 65536
#define DISABLE_NAGLE 1
class GeneralException2
{
public:
	GeneralException2(const char *m, int e = 0) 
	{
		strcpy(msg,m);
		err=e;
	}
	char msg[128];
	int err;
};

class StreamException2 : public GeneralException2
{
public:
	StreamException2(const char *m) : GeneralException2(m) {}
	StreamException2(const char *m,int e) : GeneralException2(m,e) {}
};

class SockException2 : public StreamException2
{
public:
	SockException2(const char *m="Socket") : StreamException2(m) {}
	SockException2(const char *m, int e) : StreamException2(m,e) {}
};

class TimeoutException2 : public StreamException2
{
public:
	TimeoutException2(const char *m="Timeout") : StreamException2(m) {}
};

/**
 * 重叠IO TCP套接字类
 */
class OIOSocketEx2
{
public:
	/**
	 * 初始化网络
	 */
	static void	init();

	OIOSocketEx2()
	:sockNum(0)
	{
		exitEvent = WSACreateEvent();
		m_ReadEventArray[0] = WSACreateEvent();
		m_WriteEventArray[0] = WSACreateEvent();
		m_ReadEventArray[1] = m_WriteEventArray[1] = exitEvent;
		readTimeout = 10000;
		writeTimeout = 10000;
	}
	/**
	 * 析构函数
	 */
	virtual ~OIOSocketEx2();

	virtual void	open(unsigned long Ip, unsigned short port);
	virtual void	open(const char* lpszIp, unsigned short port);
	virtual void	open(const struct sockaddr_in *in_Socks, int socksLen);

	virtual int		read(void * pBuffer, int len);
	virtual int		readUpto(void *, int);
	virtual void	write(const void *, int);
	virtual void	bind(const unsigned short port);
	virtual void	connect();
	virtual void	close();
	virtual bool	active() {return sockNum != 0;}
	virtual void	setBlocking(bool);
	virtual int		sendPacket(const char* buf, int len);	
	virtual int		recvPacket(char * & buf, int len = 0);

	void	setReuse(bool);
	void	setNagle(bool);
	void	setLinger(int);
	void	checkTimeout(bool,bool);
	void	setTimeout(unsigned long wt, unsigned long rt);
	unsigned int getIP(const char *name);

	string  strIP;
private:
	SOCKET sockNum;
	struct sockaddr_in remoteAddr;

	WSAEVENT exitEvent;

	WSAOVERLAPPED  m_ReadOverlapped;
	WSAEVENT m_ReadEventArray[2];
	WSABUF m_ReadDataBuf;

	WSAOVERLAPPED  m_WriteOverlapped;
	WSAEVENT m_WriteEventArray[2];
	WSABUF m_WriteDataBuf;
	unsigned long writeTimeout;
	unsigned long readTimeout;
};

#endif
