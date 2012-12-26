// ESCTEXT.cpp : 实现文件
//

#include "stdafx.h"
#include "ESCTEXT.h"
#include ".\esctext.h"


// ESCTEXT 对话框

IMPLEMENT_DYNAMIC(ESCTEXT, CDialog)
ESCTEXT::ESCTEXT(CWnd* pParent /*=NULL*/)
	: CDialog(ESCTEXT::IDD, pParent)
{
}

ESCTEXT::~ESCTEXT()
{
}

void ESCTEXT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ESCTEXT, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// ESCTEXT 消息处理程序


void ESCTEXT::DrawBar(CDC* dc)
{
	CRect clientRect;
	GetClientRect( clientRect );
	
	//写字

	CFont ft;
	ft.CreateFont(clientRect.Height()-30,clientRect.Height()-45,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	dc->SelectObject(&ft);

	dc->SetBkColor(GetSysColor(COLOR_BTNFACE));
	
	dc->SetTextColor(RGB(255,255,255));
	dc->DrawText("按  ESC  退 出 全 屏 模 式",clientRect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}


void ESCTEXT::OnPaint()
{
	CPaintDC dc(this); 
	DrawBar(&dc);

	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}



BOOL ESCTEXT::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^WS_EX_LAYERED);

	// CDC *dc=GetDC();

	::SetLayeredWindowAttributes(GetSafeHwnd(),GetSysColor(COLOR_BTNFACE),0,LWA_COLORKEY);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL ESCTEXT::PreTranslateMessage(MSG* pMsg)
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
