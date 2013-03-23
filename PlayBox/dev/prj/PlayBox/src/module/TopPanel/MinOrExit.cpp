// MinOrExit.cpp : 实现文件
//

#include "stdafx.h"
#include "MinOrExit.h"
#include "./src/Core/AfxGlobals.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "TopPanelDef.h"
#include "../LayoutMgr/SeperateMode/HostWndMgr.h"

// CMinOrExit 对话框

IMPLEMENT_DYNAMIC(CMinOrExit, CDialog)
CMinOrExit::CMinOrExit(CWnd* pParent /*=NULL*/)
	: CDialog(CMinOrExit::IDD, pParent)
	,m_chkRad(FALSE)
	,m_chkBox(FALSE)
{
	m_bkg= ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("MinOrExitBkg");
}

CMinOrExit::~CMinOrExit()
{
}

void CMinOrExit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMinOrExit, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CMinOrExit 消息处理程序
BOOL CMinOrExit::OnInitDialog()
{
	CDialog::OnInitDialog();

	COLORREF m_color(RGB(255,0,255));
	SetWindowRgn(m_bkg->CreateRgnFromBitmap(m_color,this),TRUE);

	bool exitchoice;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,exitchoice);
	m_chkRad=exitchoice?TRUE:FALSE;

	CRect rectNULL(0,0,0,0);
	m_btnClose.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_CANCLE);
	m_btnCancle.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_CANCLE);
	m_btnOk.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_OK);
	m_rad1unCheck.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_RAD1);
	m_rad1Checked.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_RAD1);
	m_rad2unCheck.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_RAD2);
	m_rad2Checked.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_RAD2);
	m_boxunCheck.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_BOX);
	m_boxChecked.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_MINORCLOSE_BOX);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_close",&m_btnClose);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_cancle",&m_btnCancle);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_ok",&m_btnOk);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_rad1unCheck",&m_rad1unCheck);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_rad1Checked",&m_rad1Checked);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_rad2unCheck",&m_rad2unCheck);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_rad2Checked",&m_rad2Checked);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_m_boxunCheck",&m_boxunCheck);
	pLayoutMgr->RegisterCtrl( this,"MinOrExit_m_boxChecked",&m_boxChecked);

	pLayoutMgr->CreateControlPane( this,"MinOrExit","normal" );
	pLayoutMgr->CreateBmpPane( this,"MinOrExit","normal" );

	if (m_chkRad)
	{
		CheckRad2();
	}
	else
	{
		CheckRad1();
	}
	m_boxChecked.ShowWindow(SW_HIDE);
	m_boxunCheck.ShowWindow(SW_SHOW);
	CRect winPos(0,0,320,180);
	CRect wndRect;
	
	CWnd *pWnd = NULL;
	if(AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
	{
		pWnd = CHostWndMgr::GetInstance()->GetMainWnd();
	}
	else
		pWnd = GetParent();
	pWnd->GetWindowRect(&wndRect);
	int x,y;
	x=(wndRect.left+wndRect.right)/2-160;
	y=(wndRect.top+wndRect.bottom)/2-90;
	YL_Log("MinOrExit.txt",LOG_DEBUG,"OnInitDialog","win rect x: %d, y: %d, r: %d, b: %d.", wndRect.left, wndRect.top, wndRect.right, wndRect.bottom);
	int screenX=GetSystemMetrics(SM_CXSCREEN);
	int screenY=GetSystemMetrics(SM_CYSCREEN);
	if (x>=0)
	{
		winPos.left=x;
	}
	else
	{
		winPos.left=20;
	}
	winPos.right=winPos.left+320;
	
	if (winPos.right>screenX)
	{
		winPos.right=screenX-20;
		winPos.left=winPos.right-320;
	}
	if (y>0)
	{
		winPos.top=y;
	}
	else
	{
		winPos.top=20;
	}
	winPos.bottom=winPos.top+180;
	if (winPos.bottom>screenY)
	{
		winPos.bottom=screenY-20;
		winPos.top=winPos.bottom-180;
	}
	MoveWindow(winPos);
	return TRUE;
}
void CMinOrExit::CheckRad1()
{
	m_chkRad=FALSE;
	m_rad1Checked.ShowWindow(SW_SHOW);
	m_rad1unCheck.ShowWindow(SW_HIDE);
	m_rad2Checked.ShowWindow(SW_HIDE);
	m_rad2unCheck.ShowWindow(SW_SHOW);
}
void CMinOrExit::CheckRad2()
{
	m_chkRad=TRUE;
	m_rad1Checked.ShowWindow(SW_HIDE);
	m_rad1unCheck.ShowWindow(SW_SHOW);
	m_rad2Checked.ShowWindow(SW_SHOW);
	m_rad2unCheck.ShowWindow(SW_HIDE);
}
void CMinOrExit::CheckBox()
{
	m_chkBox=TRUE;
	m_boxChecked.ShowWindow(SW_SHOW);
	m_boxunCheck.ShowWindow(SW_HIDE);
}
void CMinOrExit::UnCheckBox()
{
	m_chkBox=FALSE;
	m_boxChecked.ShowWindow(SW_HIDE);
	m_boxunCheck.ShowWindow(SW_SHOW);
}
BOOL CMinOrExit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	bool askexit,exitchoice;
	WORD lowWord	= LOWORD( wParam );
	switch (lowWord)
	{
	case ID_BTN_MINORCLOSE_RAD1:
		CheckRad1();
		break;
	case ID_BTN_MINORCLOSE_RAD2:
		CheckRad2();
		break;
	case ID_BTN_MINORCLOSE_BOX:
		if (m_chkBox)
		{
			UnCheckBox();
		}
		else
		{
			CheckBox();
		}
		break;
	case ID_BTN_MINORCLOSE_CANCLE:
		CDialog::OnCancel();
		break;
	case ID_BTN_MINORCLOSE_OK:
		OnOK();
		break;
	}
	return CDialog::OnCommand(wParam,lParam);
}



void CMinOrExit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false ) ;	
}

void CMinOrExit::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CRect rad1Rect(24,83,296,95);
	CRect rad2Rect(24,104,162,118);
	CRect boxRect(10,152,84,170);
	if (PtInRect(&rad1Rect,point))
	{
		CheckRad1();
	}
	else if (PtInRect(&rad2Rect,point))
	{
		CheckRad2();
	}
	else if(PtInRect(&boxRect,point))
	{
		m_chkBox?UnCheckBox():CheckBox();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CMinOrExit::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+29;
	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}

void CMinOrExit::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	bool askexit,exitchoice;
	ShowWindow(SW_HIDE);
	askexit=m_chkBox;
	exitchoice=m_chkRad;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_IFASKEXIT,askexit);
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,exitchoice);

	CDialog::OnOK();
}
