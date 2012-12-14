
#include "stdafx.h"
#include "SetBaseEdit.h"

IMPLEMENT_DYNAMIC(SetBaseEdit, CEdit)
SetBaseEdit::SetBaseEdit()
{
}

SetBaseEdit::~SetBaseEdit()
{
}


BEGIN_MESSAGE_MAP(SetBaseEdit, CEdit)
	//ON_WM_CTLCOLOR_REFLECT()  
	//ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



HBRUSH SetBaseEdit::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	HBRUSH hbr = NULL;
	CRect rcClient;
	GetClientRect( &rcClient );
	if( !IsWindowEnabled() )
	{
		pDC->SetBkMode( OPAQUE );
		pDC->SetBkColor( RGB(0,0,0) );
	}
	else
	{
		pDC->SetBkMode( TRANSPARENT );
	}
	return hbr;
}

BOOL SetBaseEdit::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void SetBaseEdit::OnChange()
{
	Invalidate();
}