#include "stdafx.h"
#include "SetBaseButton.h"


// SetBaseButton

IMPLEMENT_DYNAMIC(SetBaseButton, CButton)
SetBaseButton::SetBaseButton()
{
}

SetBaseButton::~SetBaseButton()
{
}


BEGIN_MESSAGE_MAP(SetBaseButton, CButton)
	//ON_WM_CTLCOLOR_REFLECT()  
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


HBRUSH SetBaseButton::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetBkColor( RGB(255,255,255) );
	pDC->SetTextColor( RGB(0,0,0) );
	HBRUSH hbr = ::CreateSolidBrush( RGB(255,255,255) );
	return hbr;
}

BOOL SetBaseButton::OnEraseBkgnd(CDC* pDC) 
{
	//½ûÖ¹»æÖÆµ×É«	
	return TRUE;
}
