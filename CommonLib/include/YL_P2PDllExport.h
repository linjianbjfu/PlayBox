#ifndef _P2P_DLL_EXPORT_H
#define _P2P_DLL_EXPORT_H

#pragma once

#include "YL_P2PInt.h"

#define DllExport __declspec( dllexport )  

#ifdef __cplusplus
extern "C" { 
#endif
	//����KwMV
	DllExport void StartKWMV(  const char* pszKwMVPath, const char* pszAppName = ""  );

	//�˳�KwMV
	DllExport void CloseKWMV();

	//ע�����P2P��Ϣ����
	DllExport void AttachP2PListener(const P2PObserver* pP2PGObserver);

	//��ע����P2P��Ϣ����
	DllExport void DetachP2PListener(const P2PObserver* pP2PGObserver);

	//����������Դ
	DllExport bool StartDown(const Sign& sign, const char* szRid, 
		const unsigned int& unPriority, EFileType filetype, EDownMode downmode, const P2PObserver* pP2PObserver, bool bContinueDown);

	//ֹͣ������Դ
	DllExport void StopDown(const Sign& sign, P2PObserver* pP2PObserver);

	//ɾ��������Դ
	DllExport void DelRes(const Sign& sign, P2PObserver* pP2PObserver);

	//��ȡ��Դ��Ϣ
	DllExport void GetResInfo(const Sign& sign, unsigned int& uiRate, string& strFilePath);

	//ֹͣ����
	DllExport void StopAll(void);

	//ע��ϵͳ�㲥��Ϣ���մ���
	DllExport void SetSysMsgWnd(HWND hWnd);

#ifdef __cplusplus
}
#endif




#endif