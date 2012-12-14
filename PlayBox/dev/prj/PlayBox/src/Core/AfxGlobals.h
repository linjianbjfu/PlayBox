#ifndef _AFX_GLOBALS_H
#define _AFX_GLOBALS_H


#include "IDataManager.h"
#include "IUIManager.h"
#include "IMessageManager.h"
#include "../AppConfig/config/AfxUserConfig.h"
#include "../../../../CommonLib/prj/Log/KwLogSvr.h"


//获得数据管理类全局变量
IDataManager*		AfxGetDataManager2();

//获得消息管理类全局变量
IMessageManager*	AfxGetMessageManager();
//获得界面管理类全局变量
IUIManager*			AfxGetUIManager();

//获得程序主窗口
CWnd*				AfxGetMainWindow();

//获得用户的配置文件
AfxUserConfig*		AfxGetUserConfig();
//设置程序主窗口
void				AfxSetMainWindow(CWnd* pWnd );

#define STR_USER_CLICK    "USRCLK"
#define	STR_DEFAULT_SKIN  "default"
#define STR_SKIN_XML	  "config.xml"

#endif