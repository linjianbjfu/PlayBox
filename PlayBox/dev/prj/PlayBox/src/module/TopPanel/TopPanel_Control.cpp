#include "stdafx.h"
#include "TopPanelWnd.h"
#include "toppanel_control.h"
#include "TopPanelDef.h"
#include "../../gui/util/ShowMenu.h"
#include "../../Core/CDataManager.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "TopPanelDef.h"

CTopPanelControl* CTopPanelControl::m_pTopPanelControl = NULL;

CTopPanelControl::CTopPanelControl()
{
}

CTopPanelControl::~CTopPanelControl()
{
}

CTopPanelControl* CTopPanelControl::GetInstance()
{
	if( m_pTopPanelControl == NULL )
	{
		m_pTopPanelControl = new CTopPanelControl();
	}
	return m_pTopPanelControl;
}

void CTopPanelControl::DeleteInstance()
{
	if( m_pTopPanelControl != NULL )
	{
		delete m_pTopPanelControl;
	}
}

void CTopPanelControl::DoMin()
{
	GLOBAL_PANELCHANGEDATA->IPanelChange_Min();
}

void CTopPanelControl::HoldWindow()
{
	if( m_pWndTopPanel->m_bMainWndHold )
	{
		AfxGetMainWindow()->SetWindowPos(&CWnd::wndNoTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		m_pWndTopPanel->m_bMainWndHold = false;
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,false);
	}
	else
	{
		AfxGetMainWindow()->SetWindowPos(&CWnd::wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		m_pWndTopPanel->m_bMainWndHold = true;
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,true);
	}
}

void CTopPanelControl::AutoRunHideMainWindow()
{
	m_pWndTopPanel->m_bHideMainWindow=TRUE;
}

BOOL CTopPanelControl::IsHold()
{
	return m_pWndTopPanel->m_bMainWndHold;
}

void CTopPanelControl::ShowMenu(CPoint p)
{
	if(m_pWndTopPanel != NULL)
	{				
		ShowMenu(&m_pWndTopPanel->m_pShowMenu, p);
	}
}

void CTopPanelControl::ShowMenu(CShowMenu **ppShowMenu, CPoint p)
{
	*ppShowMenu = new CShowMenu(m_pWndTopPanel, IDR_MENU_TITLE, 0);
	CShowMenu *pMenu = (*ppShowMenu);
	//处理登录菜单
	//if(IUserMan::User_IsLogin())
	//{
	//	pMenu->DisableItem( ID_MEMU_LOGON_IN );
	//}
	//else
	//{
	//	pMenu->DisableItem( ID_MEMU_LOGON_OUT );
	//}
	
	if( !AfxGetUIManager()->UICanExit() )
		pMenu->DisableItem(ID_APP_EXIT);

	//处理 总在最前菜单
	//if(m_pWndTopPanel->m_bMainWndHold)
	//	pMenu->CheckItem( IDC_TOPPANEL_HOLD );

	m_pWndTopPanel->m_vecSubjects.clear();
	m_pWndTopPanel->m_vecSubjectShowNames.clear();

	SubjectData* pSubData	= AfxGetUIManager()->UIGetSkinMgr()->GetSubject();
	OneSubject*	 CurSub		= AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();

	for(size_t idx = 0;idx < pSubData->vecSubject.size();idx++ )
	{
		if( pSubData->vecSubject[idx].dSystemPos > 0 && !pSubData->vecSubject[idx].bNeedDown )
		{
			m_pWndTopPanel->m_vecSubjects.push_back( pSubData->vecSubject[idx].strSubName );
			m_pWndTopPanel->m_vecSubjectShowNames.push_back( pSubData->vecSubject[idx].strShowName );
		}
	}

	m_pWndTopPanel->m_vecSkins.clear();
	GetAllSkinName( m_pWndTopPanel->m_vecSkins );	

	pMenu->ShowMenu( p );	
	delete pMenu;
	(*ppShowMenu) = NULL;
}

//获取到所有的皮肤的名称，注意：不是路径
void CTopPanelControl::GetAllSkinName( vector<string>& vSkinName )
{
	char szbuffer[512];
	CLhcImg::GetHomePath(szbuffer,512);
	strcat(szbuffer,"\\skin\\");
	string sHomePath = szbuffer;
	vector<string> vecSkins;
	AfxGetUIManager()->UIGetSkinMgr()->GetSkinNames( vecSkins,true);
	for( unsigned int n=0; n<vecSkins.size(); n++ )
	{
		if( YL_FileInfo::IsValid( sHomePath + vecSkins[n] + "\\skin.jpg" ) ) 
		{
			vSkinName.push_back( vecSkins[n] );
		}
	}
}

void CTopPanelControl::ClickButton(int pos)
{
	if(pos == TOPPANEL_NOWPLAYING)
	{
		::SendMessage(m_pWndTopPanel->m_hWnd,WM_COMMAND,10100,0);
	}
	else if(pos == TOPPANEL_NETLIB)
	{
		::SendMessage(m_pWndTopPanel->m_hWnd,WM_COMMAND,10102,0);
	}
	else if(pos == TOPPANEL_DOWNLOAD)
	{
		::SendMessage(m_pWndTopPanel->m_hWnd,WM_COMMAND,10103,0);
	}
	else if(pos == TOPPANEL_COMMEND)
	{
		::SendMessage(m_pWndTopPanel->m_hWnd,WM_COMMAND,10101,0);
	}
}