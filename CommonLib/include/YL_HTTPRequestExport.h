#ifndef _HTTPREQUEST_EXPORT_H
#define _HTTPREQUEST_EXPORT_H

#pragma once

#include "YL_HTTPInt.h"

#define DllExport __declspec( dllexport )  

#ifdef __cplusplus
extern "C" { 
#endif

	//启动KwMV
	DllExport void StartKWHD( const char* pszAppName = "" );

	//退出KwMV
	DllExport void CloseKWHD();

	//启动下载资源
	DllExport bool StartHttpDown(const char* szUrl, HTTPObserver* pHTTPObserver);

	//停止下载资源
	DllExport void StopHttpDown(const char* szUrl, HTTPObserver* pHTTPObserver);

	//停止下载
	DllExport void StopAllHttp(void);

#ifdef __cplusplus
}
#endif




#endif