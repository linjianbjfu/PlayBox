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
    ON_BN_CLICKED(IDOK, OnClickedOK)
    ON_BN_CLICKED(IDCANCEL, OnClickedCancel)
	ON_BN_CLICKED(IDC_CONFIG_BTN_DEFAULT, OnConfigBtnDefaultBnClicked)
	//ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	//��֤ͬʱֻ����һ�����öԻ���
	if(g_hLastConfWnd != NULL && g_hLastConfWnd != m_hWnd)
	{
		::SendMessage(g_hLastConfWnd, WM_COMMAND, IDCANCEL, 0);
	}
	g_hLastConfWnd = m_hWnd;

	CenterWindow();

	RECT rect;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&rect);
	POINT point;
	point.x = rect.left;
	point.y = rect.top;
	ScreenToClient(&point);
    //�����ұߵ����жԻ���
	CRect dlgRc = CRect( point.x,point.y,(rect.right - rect.left)+point.x,(rect.bottom-rect.top)+point.y );

	m_downDlg.Create( CDownDialog::IDD, this );
	m_downDlg.m_strAlert = "����";
	
	m_sysDlg.Create( CSysDialog::IDD, this );
	m_sysDlg.m_strAlert = "ϵͳ";

	m_mapBase[CONF_IDX_DOWN]  = &m_downDlg;
	m_mapBase[CONF_IDX_SYS]   = &m_sysDlg;

	//�ڷŸ�wndλ��
	CRect rc(dlgRc);
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		CRect rcOne;
		m_mapBase[i]->GetClientRect(&rcOne);

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
	//����Ƿ����жԻ����ϵ�ֵ������ָ����Χ�ڵ�
	if( !IsAllInputOK() )
		return;
	//������������
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->SaveConf();
	}
	//�ݻ����д���
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->DestroyWindow();
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
		m_mapBase[i]->DestroyWindow();
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
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow();
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