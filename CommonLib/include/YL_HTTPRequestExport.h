#ifndef _HTTPREQUEST_EXPORT_H
#define _HTTPREQUEST_EXPORT_H

#pragma once

#include "YL_HTTPInt.h"

#define DllExport __declspec( dllexport )  

#ifdef __cplusplus
extern "C" { 
#endif

	//����KwMV
	DllExport void StartKWHD( const char* pszAppName = "" );

	//�˳�KwMV
	DllExport void CloseKWHD();

	//����������Դ
	DllExport bool StartHttpDown(const char* szUrl, HTTPObserver* pHTTPObserver);

	//ֹͣ������Դ
	DllExport void StopHttpDown(const char* szUrl, HTTPObserver* pHTTPObserver);

	//ֹͣ����
	DllExport void StopAllHttp(void);

#ifdef __cplusplus
}
#endif




#endif