#ifndef _DOWNLOADER_INT_H
#define _DOWNLOADER_INT_H

#pragma once

//Downloader������Ϣ������
class DownloaderObserver
{
public:
	virtual ~DownloaderObserver(){};

public:

	//���ؿ�ʼ
	virtual void DownloaderOb_DownStart(const char* lpszFilePath, unsigned int unFileSize){};

	//�������
	virtual void DownloaderOb_DownFinish(){};

	//����ʧ��
	virtual void DownloaderOb_DownFailed(){};

	//���ؽ���
	virtual void DownloaderOb_DownProgress(unsigned int unContinue, unsigned int unSpeed){};

};

#endif