#ifndef _HTTP_INT_H
#define _HTTP_INT_H

#pragma once

// #include "YL_P2PDef.h"
enum HTTP_DOWN_FAILED_REASON
{
	HTTP_FAILED_UNKNOWN = 0,
	HTTP_FAILED_NOTCONNECT,
	HTTP_FAILED_CANT_CREATE_LOCAL_FILE,
	HTTP_FAILED_CANT_WRITE_LOCAL_FILE,
	HTTP_FAILED_SOCKET_ERROR,
	HTTP_FAILED_TIMEOUT,

	HTTP_FAILED_NOPEER,
	HTTP_FAILED_CANCELED,
	HTTP_FAILED_HTTP_NOTUSE
};

//HTTP������Ϣ������
class HTTPObserver
{
public:
	virtual ~HTTPObserver(){};

public:

	//���ؿ�ʼ
	virtual void HTTPOb_DownStart(const char* szUrl, const char* lpszFilePath, unsigned int unFileSize){};

	//�������
	virtual void HTTPOb_DownFinish(const char* szUrl){};

	//����ʧ��
	virtual void HTTPOb_DownFailed(const char* szUrl, HTTP_DOWN_FAILED_REASON Hdfr){};

	//���ؽ���
	virtual void HTTPOb_DownProgress(const char* szUrl, unsigned int unContinue, unsigned int unSpeed){};

	virtual void HTTPOb_DownloadPause(const char* szUrl){};
};

#endif