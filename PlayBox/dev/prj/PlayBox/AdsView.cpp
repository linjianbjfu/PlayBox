// AdsView.cpp : implementation file
//

#include "stdafx.h"
#include "PlayBox.h"
#include "AdsView.h"
#include ".\adsview.h"


// CAdsView

IMPLEMENT_DYNCREATE(CAdsView, CHtmlView)

CAdsView::CAdsView()
{
}

CAdsView::~CAdsView()
{
}

void CAdsView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAdsView, CHtmlView)
END_MESSAGE_MAP()


// CAdsView diagnostics

#ifdef _DEBUG
void CAdsView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CAdsView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CAdsView message handlers

void CAdsView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	this->m_pParentDlg = (CPlayBoxDlg *)this->GetParentOwner();
}
