#include "stdafx.h"
#include "CDataManager.h"
#include "CMessageManager.h"
#include "CUIManager.h"

static AfxUserConfig	g_cAfxUserConfig;
static CDataManager		g_cDataManager;
static CMessageManager	g_cMessageManager;
static CUIManager		g_cUIManager;
static CWnd*			g_pWnd = NULL;
//������ݹ�����ȫ�ֱ���
IDataManager*		AfxGetDataManager2()
{
	return &g_cDataManager;
}

//�����Ϣ������ȫ�ֱ���
IMessageManager*	AfxGetMessageManager()
{
	return &g_cMessageManager;
}

IUIManager*			AfxGetUIManager()
{
	return &g_cUIManager;
}


//��ó���������
CWnd*				AfxGetMainWindow()
{
	return g_pWnd;
}

//���ó���������
void				AfxSetMainWindow( CWnd* pWnd )
{
	g_pWnd = pWnd;
}

//����û��������ļ�
AfxUserConfig*		AfxGetUserConfig()
{
	return &g_cAfxUserConfig;
}