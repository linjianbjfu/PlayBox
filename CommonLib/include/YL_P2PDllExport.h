#ifndef _P2P_DLL_EXPORT_H
#define _P2P_DLL_EXPORT_H

#pragma once

#include "YL_P2PInt.h"

#define DllExport __declspec( dllexport )  

#ifdef __cplusplus
extern "C" { 
#endif
	//启动KwMV
	DllExport void StartKWMV(  const char* pszKwMVPath, const char* pszAppName = ""  );

	//退出KwMV
	DllExport void CloseKWMV();

	//注册接收P2P消息的类
	DllExport void AttachP2PListener(const P2PObserver* pP2PGObserver);

	//反注接收P2P消息的类
	DllExport void DetachP2PListener(const P2PObserver* pP2PGObserver);

	//启动下载资源
	DllExport bool StartDown(const Sign& sign, const char* szRid, 
		const unsigned int& unPriority, EFileType filetype, EDownMode downmode, const P2PObserver* pP2PObserver, bool bContinueDown);

	//停止下载资源
	DllExport void StopDown(const Sign& sign, P2PObserver* pP2PObserver);

	//删除下载资源
	DllExport void DelRes(const Sign& sign, P2PObserver* pP2PObserver);

	//获取资源信息
	DllExport void GetResInfo(const Sign& sign, unsigned int& uiRate, string& strFilePath);

	//停止下载
	DllExport void StopAll(void);

	//注册系统广播消息接收窗口
	DllExport void SetSysMsgWnd(HWND hWnd);

#ifdef __cplusplus
}
#endif




#endif