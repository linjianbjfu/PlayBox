#ifndef _AFX_GLOBALS_H
#define _AFX_GLOBALS_H


#include "IDataManager.h"
#include "IUIManager.h"
#include "IMessageManager.h"
#include "../AppConfig/config/AfxUserConfig.h"
#include "../../../../CommonLib/prj/Log/KwLogSvr.h"


//������ݹ�����ȫ�ֱ���
IDataManager*		AfxGetDataManager2();

//�����Ϣ������ȫ�ֱ���
IMessageManager*	AfxGetMessageManager();
//��ý��������ȫ�ֱ���
IUIManager*			AfxGetUIManager();

//��ó���������
CWnd*				AfxGetMainWindow();

//����û��������ļ�
AfxUserConfig*		AfxGetUserConfig();
//���ó���������
void				AfxSetMainWindow(CWnd* pWnd );

#define STR_USER_CLICK    "USRCLK"
#define	STR_DEFAULT_SKIN  "default"
#define STR_SKIN_XML	  "config.xml"

#endif