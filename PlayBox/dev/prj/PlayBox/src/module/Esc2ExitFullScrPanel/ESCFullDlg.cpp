// ESCFullDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ESCFullDlg.h"
#include ".\escfulldlg.h"

#include "../gui/util/CBufferDC.h"
#include "../gui/Bitmap/DibBitmap.h"
#include "../AppConfig/config/ConfigAppDef.h"

#include <gdiplus.h>
using namespace Gdiplus;

// ESCFullDlg 对话框

IMPLEMENT_DYNAMIC(ESCFullDlg, CDialog)
ESCFullDlg::ESCFullDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ESCFullDlg::IDD, pParent)
{
	m_pEscTextDlg = new ESCTEXT();
}

ESCFullDlg::~ESCFullDlg()
{
	delete m_pEscTextDlg;
}

void ESCFullDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ESCFullDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



int ESCFullDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if( __super::OnCreate( lpCreateStruct ) == -1 )
	{
		return -1;
	}

	LoadSkin();
	m_pEscTextDlg->Create(IDD_DIALOG_ESCTEXT,this);

	// TODO:  在此添加您专用的创建代码

	return 0;
}


void ESCFullDlg::LoadSkin()
{
	ISkinMgr* pSkinMgr	= AfxGetUIManager()->UIGetSkinMgr();
	m_pBitmap = pSkinMgr->GetDibBmp("Esc");
}



void ESCFullDlg::DrawBar(CDC* dc)
{
	CRect clientRect;
	GetClientRect( clientRect );

	CDC MemDC;
	MemDC.CreateCompatibleDC(dc);
	CBitmap MemBitmap;//定义一个位图对象	
	MemBitmap.CreateCompatibleBitmap( dc,clientRect.Width(),clientRect.Height() );
	MemDC.SelectObject(&MemBitmap);

	m_pBitmap->SetCDibRect(clientRect);
	m_pBitmap->Draw( &MemDC,FALSE);
	dc->BitBlt(0,0,clientRect.Width(),clientRect.Height(),&MemDC,0,0,SRCCOPY);
	

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}


void ESCFullDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawBar(&dc);
	// TODO: 在此处添加消息处理程序代码
//	 不为绘图消息调用 
}

BOOL ESCFullDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void ESCFullDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{

	// 调整自身位置
	CWnd *pDeskTopWnd = GetDesktopWindow();
	CRect rctParent, rctSelf;

	GetWindowRect(rctSelf);
	if (pDeskTopWnd)
	{
		pDeskTopWnd->GetWindowRect(&rctParent);
		rctSelf.left = rctParent.left + ( (rctParent.Width()-rctSelf.Width())/2 );
		rctSelf.top = rctParent.top + 50;
	}

	SetWindowPos(&wndTopMost,rctSelf.left,rctSelf.top,0,0,SWP_NOSIZE);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);

	::SetLayeredWindowAttributes(GetSafeHwnd(),RGB(255,0,255),150,LWA_COLORKEY|LWA_ALPHA);

	if(bShow==true)
	{	
		m_pEscTextDlg->ShowWindow(SW_SHOW);
	}
	else
	{	
		m_pEscTextDlg->ShowWindow(SW_HIDE);
	}

	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
}

void ESCFullDlg::ShowDlg(BOOL bShow)
{
	if( bShow )
	{
		ShowWindow(SW_SHOW);
		m_pEscTextDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		ShowWindow(SW_HIDE);
		m_pEscTextDlg->ShowWindow(SW_HIDE);
	}
}
BOOL ESCFullDlg::PreTranslateMessage(MSG* pMsg)
{
	
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return 0;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
