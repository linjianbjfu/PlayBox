#include "stdafx.h"
#include "LocalSearchEdit.h"

#define WM_LOCAL_SEARCH_KEY		WM_USER+1
#define MSG_LOCAL_SEARCH_CLICK	WM_USER+2

IMPLEMENT_DYNAMIC(CLocalSearchEdit, CEdit)

CLocalSearchEdit::CLocalSearchEdit()
{
	m_bFocus      = false;
	m_bMouseOver  = false;
}

CLocalSearchEdit::~CLocalSearchEdit()
{
}

BEGIN_MESSAGE_MAP(CLocalSearchEdit, CEdit)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnEnSetfocus)
	ON_MESSAGE(WM_LOCAL_SEARCH_KEY, OnSearchKey)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

BOOL CLocalSearchEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (message != WM_CTLCOLOREDIT) 
	{
		return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
	}

	COLORREF clrText = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditText");
	COLORREF clrBk = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditBK");

	HDC hdcChild = (HDC)wParam;
	SetTextColor(hdcChild, clrText);
	SetBkColor(hdcChild, clrBk);
	return TRUE;
}

void CLocalSearchEdit::OnPaint() 
{
	return __super::OnPaint();
	CPaintDC dc(this);
	CRect r;
	CString text;
	
	COLORREF clrText = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditText");
	COLORREF clrBk = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditBK");
	COLORREF clrTip = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditTip");

	GetWindowText(text);
	GetWindowRect(&r);
	ScreenToClient(&r);
	dc.SelectObject(GetFont());
	dc.FillSolidRect(r, clrBk);
	r.OffsetRect(3,0);

	dc.SelectObject(GetFont());
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int nMargin = (r.Height() - nFontHeight) / 2;
	r.DeflateRect(0, nMargin);
	SetRectNP(&r);

	if( text.Trim().GetLength() == 0 )
	{
		if( GetFocus() != this)
		{
			text = LOCAL_SEARCH_EDIT_TOOLTIP;
		}
		dc.SetTextColor(clrTip);		
		dc.DrawText(text, text.GetLength(), r, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
	}else
	{
		if(text.Find(LOCAL_SEARCH_EDIT_TOOLTIP) == 0 )
			dc.SetTextColor(clrTip);
		else
			dc.SetTextColor(clrText);
		dc.DrawText(text, text.GetLength(), r, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
	}
	////////////////////////


	if(text.Find(LOCAL_SEARCH_EDIT_TOOLTIP) == 0 )
		dc.SetTextColor(clrTip);
	else
		dc.SetTextColor(clrText);
}

BOOL CLocalSearchEdit::PreTranslateMessage(MSG *pMsg)
{
	switch(pMsg->message)
	{	
	case WM_KEYDOWN :
		{
			switch (pMsg->wParam)
			{
			case VK_UP :
			case VK_DOWN :
				/*
				if(m_tip.IsWindowVisible())
				{
					m_tip.m_list.SetFocus();
					m_tip.m_list.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);			
					return true;
				}
				break;
*/
			case VK_RETURN:
				GetWindowText(m_curString);
				GetParent()->SendMessage(MSG_LOCAL_SEARCH_CLICK);
				{
					PostMessage (EM_SETSEL, 0, 0) ;
				}
//				m_tip.ShowWindow(SW_HIDE);
				break;
				
			case 0x56:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					Paste();
					return TRUE;
				}
				break;
			case 0x43:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					Copy();
					return TRUE;
				}
				break;
			case 0x58:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					Cut();
					return TRUE;
				}
				break;
			default:
				break;
			}
		}
	case WM_WINDOWPOSCHANGED :
	case WM_WINDOWPOSCHANGING :
		/*
		if(m_tip.IsWindowVisible())
			m_tip.Show();
		*/
		break;
	default :
		break;
	}	
	return CEdit::PreTranslateMessage(pMsg);
}

int CLocalSearchEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	ResizeWindow();
	/*
	m_tip.m_pEdit = this;
	m_tip.Create(IDD_SEARCHKEY_TIP_DIALOG);
	*/
	return 0;
}

void CLocalSearchEdit::ResizeWindow()
{
	CRect r;
	
	GetClientRect(&r);
	r.left += 3;
	r.top += 2;
	SetRect(&r);
}

void CLocalSearchEdit::OnDestroy()
{
// 	m_tip.DestroyWindow();
	__super::OnDestroy();
}

void CLocalSearchEdit::OnEnKillfocus()
{
	m_bFocus = false;
	SetEditBorder();
	/*
	if(m_tip.IsWindowVisible() && GetFocus() != &m_tip.m_list)
		m_tip.ShowWindow(SW_HIDE);
		*/
	//如果为空，则显示提示语
	CString cstr;
	GetWindowText(cstr);
	cstr.Trim(" ");
	if(cstr=="")
	{
		SetWindowText("");
	}
	Invalidate();
}


void CLocalSearchEdit::OnEnSetfocus()
{/*
	m_bFocus = true;
	SetEditBorder();
	POINT pt;
	::GetCursorPos(&pt);
	HWND hwnd = ::WindowFromPoint(pt);

	Invalidate();

	CString str;
	GetWindowText(str);
// 	if(str.Find(LOCAL_SEARCH_EDIT_TOOLTIP) == 0 && hwnd == m_hWnd)
// 	{
// 		SetWindowText("");
// 		str = "";
// 	}
	//选中当前的搜索词
	int iLength = str.GetLength();
	if( iLength != 0 )
	{
		//CPaintDC dc(this);
		CDC *pDC = GetDC();
		CRect rcClient;
		GetClientRect(&rcClient);
		rcClient.OffsetRect(3, 2);
		pDC->SelectObject(GetFont());
		pDC->DrawText(str, &rcClient, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
		rcClient.OffsetRect(-3, -2);
		ClientToScreen(&rcClient);
		CPoint pt;
		::GetCursorPos(&pt);
		int iSel = iLength*(pt.x - rcClient.left)/rcClient.Width();
		iSel++;
		SetSel(0,iSel);
		ReleaseDC(pDC);
	}
	//当获得焦点时，引起刷新，不再显示提示语
	if( str == "" )
	{
		Invalidate();
	}*/
}

void CLocalSearchEdit::SetEditBorder()
{
	if( m_bFocus || m_bMouseOver )
	{
		GetParent()->PostMessage(MSG_EDIT_BORDER_CHANGE,1,0);
	}else
	{
		GetParent()->PostMessage(MSG_EDIT_BORDER_CHANGE,0,0);
	}
}

LRESULT CLocalSearchEdit::OnSearchKey(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR pStr = (LPCTSTR)wParam;
	m_curString = pStr;
	SetWindowText(pStr);
	GetParent()->SendMessage(MSG_LOCAL_SEARCH_CLICK);
	PostMessage(EM_SETSEL, 0, 0);
// 	m_tip.ShowWindow(SW_HIDE);

	return LRESULT(0);
}

void CLocalSearchEdit::OnSizing(UINT fwSide, LPRECT pRect)
{
	CEdit::OnSizing(fwSide, pRect);
/*
	if(m_tip.IsWindowVisible())
		m_tip.Show();
		*/
}

void CLocalSearchEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	ResizeWindow();

	/*
	if(m_tip.IsWindowVisible())
		m_tip.Show();
		*/
}


void CLocalSearchEdit::OnMainWindowMove()
{	
/*	if( m_tip.m_hWnd != NULL && m_tip.IsWindowVisible())
		m_tip.Show();
		*/
}
void CLocalSearchEdit::OnShowHistory()
{
// 	m_tip.OnShowHistory();
}
void CLocalSearchEdit::OnAddHistory(LPCTSTR pszHistory)
{
// 	m_tip.OnAddHistory(pszHistory);
}

void CLocalSearchEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !m_bMouseOver )
	{
		m_bMouseOver = true;
		SetEditBorder();
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
	}
	__super::OnMouseMove(nFlags, point);
}

LPARAM CLocalSearchEdit::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bMouseOver = false;
	SetEditBorder();
	ReleaseCapture();
	return 0;
}