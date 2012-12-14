#ifndef _P2P_INT_H
#define _P2P_INT_H

#pragma once

#include <string>

#include "YL_P2PDef.h"

//p2p下载消息接收类
class P2PObserver
{
public:
	virtual ~P2PObserver(){};

public:

	//下载开始
	virtual void P2POb_DownStart(const string& strID, const char* lpszFilePath, unsigned int unFileSize){};

	//下载完成
	virtual void P2POb_DownFinish(const string& strID){};

	//下载失败
	virtual void P2POb_DownFailed(const string& strID, P2P_DOWN_FAILED_REASON pdfr){};

	//下载进行
	virtual void P2POb_DownProgress(const string& strID, unsigned int unContinue, unsigned int unFinished, unsigned int unSpeed){};

	//下载暂停
	virtual void P2POb_DownPause(const string& strID){};

	//下载变更
	virtual void P2POb_SigChange(const string& strID, const string& strIDNew, const char* szRid){};
};

#endif