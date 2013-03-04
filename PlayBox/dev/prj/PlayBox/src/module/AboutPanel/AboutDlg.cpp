#include "stdafx.h"
#include "AboutDlg.h"
#include "./src/Core/AfxGlobals.h"
#include ".\aboutdlg.h"

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)/*,
	m_pBmpBkAbout(NULL)*/
{}

CAboutDlg::~CAboutDlg()
{}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_ABOUT_OK, OnClickedOK)
	ON_BN_CLICKED(ID_BTN_ABOUT_CLOSE,OnClickedClose)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// CAboutDlg 消息处理程序
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rectNULL(0,0,0,0);

	m_btnOK.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_ABOUT_OK);
	m_btnClose.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_ABOUT_CLOSE);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this,"AboutDlg_OK",&m_btnOK);
	pLayoutMgr->RegisterCtrl(this,"AboutDlg_Close",&m_btnClose);
	pLayoutMgr->CreateControlPane( this,"AboutDlg","normal" );
	pLayoutMgr->CreateBmpPane( this,"AboutDlg","normal" );
	//m_pBmpBkAbout = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("bkAboutUs");

	CRect winPos(0,0,320,180);
	CRect wndRect;
	
	CWnd *pWnd = GetParent();
	pWnd->GetWindowRect(&wndRect);
	int x,y;
	x=(wndRect.left+wndRect.right)/2-160;
	y=(wndRect.top+wndRect.bottom)/2-90;
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
	winPos.right=winPos.left+319;
	
	if (winPos.right>screenX)
	{
		winPos.right=screenX-20;
		winPos.left=winPos.right-319;
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

void CAboutDlg::OnClickedOK()
{
	CDialog::OnOK();
}
void CAboutDlg::OnClickedClose()
{
	::PostMessage(this->m_hWnd,WM_CLOSE,NULL,NULL);
}
void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false ) ;	
}

UINT CAboutDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+29;
	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}
