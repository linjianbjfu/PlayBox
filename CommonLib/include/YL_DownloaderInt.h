#ifndef _DOWNLOADER_INT_H
#define _DOWNLOADER_INT_H

#pragma once

//Downloader下载消息接收类
class DownloaderObserver
{
public:
	virtual ~DownloaderObserver(){};

public:

	//下载开始
	virtual void DownloaderOb_DownStart(const char* lpszFilePath, unsigned int unFileSize){};

	//下载完成
	virtual void DownloaderOb_DownFinish(){};

	//下载失败
	virtual void DownloaderOb_DownFailed(){};

	//下载进行
	virtual void DownloaderOb_DownProgress(unsigned int unContinue, unsigned int unSpeed){};

};

#endif