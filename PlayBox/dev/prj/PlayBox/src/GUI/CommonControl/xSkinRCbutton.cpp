#include "stdafx.h"
#include "xSkinRCbutton.h"
#include ".\xskinrcbutton.h"

/////////////////////////////////CxSkinStatic
IMPLEMENT_DYNAMIC(CxSkinStatic, CStatic)
CxSkinStatic::CxSkinStatic()
:m_colorBkg(RGB(255,255,255))
,m_colorText(RGB(0,0,0))
,m_pMsgWnd(NULL)
{
}

CxSkinStatic::~CxSkinStatic()
{
}


BEGIN_MESSAGE_MAP(CxSkinStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()  
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

HBRUSH CxSkinStatic::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetBkColor(m_colorBkg);
	pDC->SetTextColor(m_colorText);
	return m_hbrBkg;
}

BOOL CxSkinStatic::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CxSkinStatic::SetSkin(COLORREF &colorText,COLORREF &colorBkg)
{
	m_colorText=colorText;
	m_colorBkg=colorBkg;
}

BOOL CxSkinStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (m_colorBkg==RGB(255,0,0))
	{
		CBrush brNull;
		brNull.CreateStockObject(NULL_BRUSH);
		m_hbrBkg = (HBRUSH)brNull.m_hObject;
	}
	else
	{
		m_hbrBkg = ::CreateSolidBrush(m_colorBkg);
	}
	
	return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

void CxSkinStatic::SetMsgWnd(CWnd *pWnd)
{
	m_pMsgWnd=pWnd;
}

void CxSkinStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
	m_pMsgWnd->SendMessage(WM_LBUTTONDOWN);
	m_pMsgWnd->SendMessage(WM_LBUTTONUP);
}

void CxSkinStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

UINT CxSkinStatic::OnNcHitTest(CPoint point)
{
	return HTCLIENT;
}


/////////////////////////////////////CxSkinRCbutton
BEGIN_MESSAGE_MAP(CxSkinRCbutton, CWnd)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_LBUTTONUP()
//	ON_WM_MBUTTONDBLCLK()
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CxSkinRCbutton::CxSkinRCbutton()
:m_bCheck(FALSE)
,m_pBmpCheckNormal(NULL)
,m_pBmpCheckOver(NULL)
,m_pBmpUnCheckNormal(NULL)
,m_pBmpUnCheckOver(NULL)
,m_iGroupId(-1)
{

}

CxSkinRCbutton::~CxSkinRCbutton()
{
	map<CxSkinRCbutton *,int>::iterator ite=s_mapGroup.find(this);
	if (ite!=s_mapGroup.end())
	{
		s_mapGroup.erase(ite);
	}
}


int CxSkinRCbutton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_text.Create("",WS_CHILDWINDOW,CRect(0,0,0,0),GetParent(),NULL);
	__super::SetSkin(m_pBmpUnCheckNormal,m_pBmpUnCheckNormal,m_pBmpUnCheckOver,m_pBmpDisable,NULL,NULL,0,0,0);

	m_font.CreateFont(
		12,						     // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet 
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"ËÎÌו"); 

	m_text.SetFont(&m_font);
	m_text.SetMsgWnd(this);


	return 0;
}


void CxSkinRCbutton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!IsWindowEnabled())
	{
		return;
	}
	if (m_iGroupId!=-1)
	{
		if (!m_bCheck)
		{
			SetCheck(TRUE);
		}
	}
	else
	{
		SetCheck(!m_bCheck);
	}

	CxSkinButton::OnLButtonDown(nFlags, point);

	GetParent()->SendMessage(MSG_RCBUTTON_CLICKED,GetDlgCtrlID());

}

void CxSkinRCbutton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!IsWindowEnabled())
	{
		return;
	}

	__super::OnLButtonUp(nFlags, point);
}


void CxSkinRCbutton::SetSkin(CDibBitmap *pBmpCheckNormal,CDibBitmap *pBmpCheckOver,CDibBitmap *pBmpUnCheckNormal,CDibBitmap *pBmpUnCheckOver,CDibBitmap *pBmpDisable,COLORREF colorText,COLORREF colorTextBkg)
{
	m_pBmpCheckNormal=pBmpCheckNormal;
	m_pBmpCheckOver=pBmpCheckOver;
	m_pBmpUnCheckNormal=pBmpUnCheckNormal;
	m_pBmpUnCheckOver=pBmpUnCheckOver;
	m_pBmpDisable=pBmpDisable;
	m_text.SetSkin(colorText,colorTextBkg);
}

void CxSkinRCbutton::SetWindowText(LPCTSTR lpszString)
{
	m_text.SetWindowText(lpszString);
	MoveText();
}

void CxSkinRCbutton::GetWindowText(CString& rString)
{
	m_text.GetWindowText(rString);
}

void CxSkinRCbutton::SetCheck(BOOL bCheck)
{
	if (bCheck && m_iGroupId!=-1)
	{
		for (map<CxSkinRCbutton *,int>::iterator ite=s_mapGroup.begin();ite!=s_mapGroup.end();ite++)
		{
			if (ite->second==m_iGroupId)
			{
				ite->first->SetCheck(FALSE);
			}
		}
	}
	m_bCheck=bCheck;
	if (m_pBmpCheckNormal)
	{
		if (m_bCheck)
		{
			__super::SetSkin(m_pBmpCheckNormal,m_pBmpCheckNormal,m_pBmpCheckOver,m_pBmpDisable,NULL,NULL,0,0,0);
		}
		else
		{
			__super::SetSkin(m_pBmpUnCheckNormal,m_pBmpUnCheckNormal,m_pBmpUnCheckOver,m_pBmpDisable,NULL,NULL,0,0,0);
		}
	}
}

int CxSkinRCbutton::GetCheck()
{
	if (m_iGroupId!=-1)
	{
		map<CxSkinRCbutton *,int>::iterator ite=s_mapGroup.begin();
		int iPos=0;
		for (;ite!=s_mapGroup.end();ite++)
		{
			if (ite->second==m_iGroupId)
			{
				if (!ite->first->m_bCheck)
				{
					iPos++;
				}
				else
				{
					return iPos;
				}
			}
		}
		if (ite==s_mapGroup.end()) 
		{
			return -1;
		}
	}
	else
	{
		return m_bCheck;
	}
	return -1;
}

void CxSkinRCbutton::SetGroupId(int iGroupId)
{
	if (m_iGroupId!=-1)
	{
		s_mapGroup.insert(pair<CxSkinRCbutton *,int>(this,iGroupId));
	}
	else
	{
		map<CxSkinRCbutton *,int>::iterator ite=s_mapGroup.find(this);
		if (ite!=s_mapGroup.end())
		{
			ite->second=iGroupId;
		}
		else
		{
			s_mapGroup.insert(pair<CxSkinRCbutton *,int>(this,iGroupId));
		}
	}
	m_iGroupId=iGroupId;
}

int CxSkinRCbutton::GetGroupId()
{
	return m_iGroupId;
}

void CxSkinRCbutton::OnSize(UINT nType, int cx, int cy)
{
	CxSkinButton::OnSize(nType, cx, cy);

	MoveText();
}

void CxSkinRCbutton::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CxSkinButton::OnWindowPosChanged(lpwndpos);
	
	MoveText();
}

void CxSkinRCbutton::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CxSkinButton::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		MoveText();
		m_text.ShowWindow(SW_SHOW);

	}
	else
	{
		m_text.ShowWindow(SW_HIDE);
	}
}

void CxSkinRCbutton::MoveText()
{
	CRect btnRect,textRect;
	GetWindowRect(&btnRect);

	CString strText;
	m_text.GetWindowText(strText);

	CDC *pDc=m_text.GetDC();
	LOGFONT   lf;   
	memset(&lf,0,sizeof(LOGFONT));   
	strcpy(lf.lfFaceName,   "ËÎÌו");   
	lf.lfHeight   =   12;   
	HFONT hfont=CreateFontIndirect(&lf);   
	HFONT hOldFont=(HFONT)pDc->SelectObject(hfont);
	CSize size=pDc->GetTextExtent(strText.Trim());


	textRect.left=btnRect.right+4;
	textRect.top=btnRect.top+2;
	textRect.right=textRect.left+size.cx;
	textRect.bottom=btnRect.bottom;

	m_text.ScreenToClient(&textRect);
	m_text.MoveWindow(textRect);
}

void CxSkinRCbutton::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);
}

LRESULT CxSkinRCbutton::OnMouseLeave(WPARAM wParam,LPARAM lParama)
{
	return __super::OnMouseLeave(wParam,lParama);
}

map<CxSkinRCbutton *,int> CxSkinRCbutton::s_mapGroup;



void CxSkinRCbutton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags,point);
}

