// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "PushBox.h"
#include "MyButton.h"
#include ".\mybutton.h"


// CMyButton

IMPLEMENT_DYNAMIC(CMyButton, CButton)
CMyButton::CMyButton()
{
}

CMyButton::~CMyButton()
{
}


BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()



// CMyButton message handlers


void CMyButton::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CButton::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
