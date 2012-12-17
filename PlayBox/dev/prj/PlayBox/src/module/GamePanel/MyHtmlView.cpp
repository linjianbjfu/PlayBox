// MyHtmlView.cpp : implementation file
//

#include "stdafx.h"
#include "PlayBox.h"
#include "MyHtmlView.h"
#include ".\myhtmlview.h"


// CMyHtmlView

IMPLEMENT_DYNCREATE(CMyHtmlView, CHtmlView)

CMyHtmlView::CMyHtmlView()
{
}

CMyHtmlView::~CMyHtmlView()
{
}

void CMyHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyHtmlView, CHtmlView)
END_MESSAGE_MAP()


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
	this->pViewDlg = (DownPercentWnd *) this->GetParentOwner ();
}
