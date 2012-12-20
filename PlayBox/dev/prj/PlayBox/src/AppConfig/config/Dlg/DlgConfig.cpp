#include "stdafx.h"

#include "../../../core/MessageID.h"
#include "DlgConfig.h"
#include "shlobj.h"
#include "../ConfigSettingDef.h"
#include "./src/Core/AfxGlobals.h"
#include "IPanelChange.h"

HWND g_hLastConfWnd = NULL;

IMPLEMENT_DYNAMIC(CConfigDialog, CDialog)

CConfigDialog::CConfigDialog(int iShowIdx, CWnd *pParent) 
	: CDialog(CConfigDialog::IDD, pParent)
{
	m_iShowIdx	= iShowIdx;
	m_bFullScreen = FALSE;
}

CConfigDialog::~CConfigDialog()
{
}

BEGIN_MESSAGE_MAP(CConfigDialog,CDialog)
	ON_BN_CLICKED(IDC_CONF_BTN_OK, OnClickedOK)
    ON_BN_CLICKED(IDCANCEL, OnClickedCancel)
	ON_BN_CLICKED(IDC_CONF_BTN_RESET, OnConfigBtnDefaultBnClicked)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	//保证同时只出现一个配置对话框
	if(g_hLastConfWnd != NULL && g_hLastConfWnd != m_hWnd)
	{
		::SendMessage(g_hLastConfWnd, WM_COMMAND, IDCANCEL, 0);
	}
	g_hLastConfWnd = m_hWnd;

	CenterWindow();
	CRect rctNULL(0,0,0,0);
	m_btnOK.Create(NULL,WS_VISIBLE,rctNULL,this,IDC_CONF_BTN_OK);
	m_btnCancle.Create(NULL, WS_VISIBLE, rctNULL, this, IDCANCEL);
	m_btnReset.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_BTN_RESET);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();

	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnOK",&m_btnOK);
	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnCancel",&m_btnCancle);
	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnReset",&m_btnReset);

	pLayoutMgr->CreateControlPane( this,"ConfigDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"ConfigDlg","normal" );

/* * * * * * * * * * * * * * 原版 * * * *
*	RECT rect;
*	GetDlgItem(IDC_STATIC_RECT)->GetWindowRect(&rect);
*	POINT point;
*	point.x = rect.left;
*	point.y = rect.top;
*	ScreenToClient(&point);
*    //创建右边的所有对话框
*	CRect dlgRc = CRect( point.x,point.y,(rect.right - rect.left)+point.x,(rect.bottom-rect.top)+point.y );
* * * * * * * * * * * * * * * * * * * * */

	CRect dlgRc;
	GetDlgItem(IDC_STATIC_RECT)->GetClientRect(&dlgRc);

	m_downDlg.Create( CDownDialog::IDD, this );
	m_downDlg.m_strAlert = "下载";
	
	m_sysDlg.Create( CSysDialog::IDD, this );
	m_sysDlg.m_strAlert = "系统";

	m_mapBase[CONF_IDX_DOWN]  = &m_downDlg;
	m_mapBase[CONF_IDX_SYS]   = &m_sysDlg;

	//摆放各wnd位置
	CRect rc(dlgRc);
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		AfxGetUIManager()->UIAddDialog(m_mapBase[i]);
		CRect rcOne;
		m_mapBase[i]->GetWindowRect(&rcOne);

		rc.bottom = rc.top + rcOne.Height();
		m_mapBase[i]->MoveWindow( rc );
		m_mapBase[i]->ShowWindow(SW_SHOW);


		rc.top = rc.bottom;
	}

	if( m_bFullScreen )
	{
		::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}

	YL_Log("AddDlg.txt",LOG_DEBUG,"add","ConfigDlg");
	AfxGetUIManager()->UIAddDialog(this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_APPEXIT,this );
	return TRUE; 
}

void CConfigDialog::OnClickedOK()
{
	//检测是否所有对话框上的值都是在指定范围内的
	if( !IsAllInputOK() )
		return;
	//保存所有配置
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->SaveConf();
	}
	//摧毁所有窗口
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		//m_mapBase[i]->DestroyWindow();
		m_mapBase[i]->EndDialog(0);
		AfxGetUIManager()->UIRemoveDialog(m_mapBase[i]);
	}
	g_hLastConfWnd = NULL;

	YL_Log("AddDlg.txt",LOG_DEBUG,"remove","ConfigDlg");
	AfxGetUIManager()->UIRemoveDialog(this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT,this );
	OnOK();
}

bool CConfigDialog::IsAllInputOK()
{
	string strMsg;
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		if( !m_mapBase[i]->IsAllInputOK( strMsg ) )
		{
			if( strMsg.length() != 0 )
			{
				MessageBox( strMsg.c_str(), m_mapBase[i]->m_strAlert.c_str() );
			}		
			return false;
		}
	}
	return true;
}

void CConfigDialog::OnClickedCancel()
{
	g_hLastConfWnd = NULL;

	for( int i=0; i<m_mapBase.size(); i++ )
	{
		//m_mapBase[i]->DestroyWindow();
		m_mapBase[i]->EndDialog(0);
		AfxGetUIManager()->UIRemoveDialog(m_mapBase[i]);
	}
	AfxGetUIManager()->UIRemoveDialog(this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT,this );

	OnCancel();
}

void CConfigDialog::OnConfigBtnDefaultBnClicked()
{
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->SetDefault();
	}
	GetDlgItem(IDC_CONF_BTN_OK)->EnableWindow();
	LogUserActMsg( "SETTING", "SET_CHANGE:DEFAULT" );
}

void CConfigDialog::IAppExit()
{
	SendMessage(WM_COMMAND,IDCANCEL,0);
}

HBRUSH CConfigDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	hbr = ::CreateSolidBrush( RGB(245,245,245) );
	return hbr;
}