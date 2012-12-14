#include "stdafx.h"
#include "CDataManager.h"
#include "CMessageManager.h"
#include "CUIManager.h"

static AfxUserConfig	g_cAfxUserConfig;
static CDataManager		g_cDataManager;
static CMessageManager	g_cMessageManager;
static CUIManager		g_cUIManager;
static CWnd*			g_pWnd = NULL;
//获得数据管理类全局变量
IDataManager*		AfxGetDataManager2()
{
	return &g_cDataManager;
}

//获得消息管理类全局变量
IMessageManager*	AfxGetMessageManager()
{
	return &g_cMessageManager;
}

IUIManager*			AfxGetUIManager()
{
	return &g_cUIManager;
}


//获得程序主窗口
CWnd*				AfxGetMainWindow()
{
	return g_pWnd;
}

//设置程序主窗口
void				AfxSetMainWindow( CWnd* pWnd )
{
	g_pWnd = pWnd;
}

//获得用户的配置文件
AfxUserConfig*		AfxGetUserConfig()
{
	return &g_cAfxUserConfig;
}