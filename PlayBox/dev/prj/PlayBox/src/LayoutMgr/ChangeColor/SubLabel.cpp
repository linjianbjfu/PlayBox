#include "stdafx.h"
#include "SubLabel.h"
#include "WndColorMgr.h"


extern CWndColorMgr   gWndColor;
CSubLabel::CSubLabel()
{
	m_textstr = "";
	m_bselect = FALSE;
	m_Fontstatic=new   CFont;
}

CSubLabel::~CSubLabel()
{
	if(m_Fontstatic->m_hObject!=NULL)
		m_Fontstatic->DeleteObject();
	delete m_Fontstatic;
}

BEGIN_MESSAGE_MAP(CSubLabel, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CSubLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	m_Fontstatic->CreateFont(12,0,0,0,400,FALSE,FALSE,0,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_MODERN,_T("ËÎÌו"));     
	return 0;
}

void CSubLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(rc,&CBrush(RGB(255,255,255)));
	HGDIOBJ oldobj = dc.SelectObject(m_Fontstatic);
	rc.DeflateRect(3,0,3,0);
	if(m_bselect)
	{
		dc.SetTextColor(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2);
	}
	else
		dc.SetTextColor(RGB(0x85,0x85,0x85));
	if(m_textstr != "")
		dc.DrawText(m_textstr,rc,DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS);
	dc.SelectObject(oldobj);

}

void  CSubLabel::SetText(CString str)
{
	m_textstr = str;
}

void CSubLabel::SetSelect(BOOL bsel)
{
	m_bselect = bsel;
}