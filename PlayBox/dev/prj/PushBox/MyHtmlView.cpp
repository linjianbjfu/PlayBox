// MyHtmlView.cpp : implementation file
//

#include "stdafx.h"
#include "PushBox.h"
#include "MyHtmlView.h"
#include ".\myhtmlview.h"
#include <afxdhtml.h>
#include "resource.h"


// CMyHtmlView

IMPLEMENT_DYNCREATE(CMyHtmlView, CHtmlView)

CMyHtmlView::CMyHtmlView()
{
	EnableAutomation();
}

CMyHtmlView::~CMyHtmlView()
{
}

void CMyHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyHtmlView, CHtmlView)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMyHtmlView,CHtmlView)
	DISP_FUNCTION(CMyHtmlView,"ClosePushBox",btnHideBox,VT_BOOL,VTS_NONE )
END_DISPATCH_MAP()


// CMyHtmlView diagnostics

#ifdef _DEBUG
void CMyHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMyHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CMyHtmlView message handlers

void CMyHtmlView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	this->m_pPushViewDlg = (CPushBoxDlg *)this->GetParentOwner ();
}

void CMyHtmlView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CHtmlView::OnPaint() for painting messages
}

void CMyHtmlView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CHtmlView::OnRButtonDown(nFlags, point);
}

void CMyHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	// TODO: Add your specialized code here and/or call the base class

	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}

BOOL CMyHtmlView::btnHideBox(void)
{
	return (m_pPushViewDlg->ShowWindow(SW_HIDE));
	/*if(m_pPushViewDlg->SendMessage(WM_CLOSE))
	{
		return true;
	}
	return false;*/
}
HRESULT CMyHtmlView::OnGetExternal( LPDISPATCH *lppDispatch)
{
	*lppDispatch = GetIDispatch(TRUE);
	return S_OK;
}

void CMyHtmlView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rect;
	m_pPushViewDlg->GetClientRect(&rect);
	MoveWindow(rect,true);
}

BOOL CMyHtmlView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_RBUTTONDOWN)||(pMsg->message == WM_RBUTTONDBLCLK))
		return true;

	return CHtmlView::PreTranslateMessage(pMsg);
}
