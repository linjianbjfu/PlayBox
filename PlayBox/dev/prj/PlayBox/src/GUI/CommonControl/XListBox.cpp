// XListBox.cpp
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
// Notes on use:  To use in an MFC project, first create
//                a listbox using the standard dialog editor.
//                Be sure to mark the listbox as OWNERDRAW
//                FIXED, and check the HAS STRINGS box.
//                Using Class Wizard, create a variable for
//                the listbox.  Finally, manually edit the
//                dialog's .h file and replace CListBox with
//                CXListBox, and #include XListBox.h.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XListBox.h"
#include "Clipboard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// NOTE - following table must be kept in sync with ColorPickerCB.cpp


BEGIN_MESSAGE_MAP(CXListBox, CListBox)
	//{{AFX_MSG_MAP(CXListBox)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CXListBox

CXListBox::CXListBox()
{
	m_ColorWindow        = ::GetSysColor(COLOR_WINDOW);
	m_ColorHighlight     = ::GetSysColor(COLOR_HIGHLIGHT);
	m_ColorWindowText    = ::GetSysColor(COLOR_WINDOWTEXT);
	m_ColorHighlightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_bColor             = TRUE;
	m_cxExtent           = 0;
	m_nTabPosition       = 8;	// tab stops every 8 columns
	m_nSpaceWidth        = 7;
	m_nContextMenuId     = (UINT)-1;
	for (int i = 0; i < MAXTABSTOPS; i++)
		m_nTabStopPositions[i] = (i+1) * m_nTabPosition * m_nSpaceWidth;
}

CXListBox::~CXListBox()
{
}

//////////////////////////////////////////////////////////////////////////////
// MeasureItem
void CXListBox::MeasureItem(LPMEASUREITEMSTRUCT)
{
}

//////////////////////////////////////////////////////////////////////////////
// CompareItem
int CXListBox::CompareItem(LPCOMPAREITEMSTRUCT)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// DrawItem
void CXListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	COLORREF oldtextcolor, oldbackgroundcolor;

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	pDC->GetCharWidth((UINT) ' ', (UINT) ' ', &m_nSpaceWidth);
	pDC->GetCharWidth((UINT) 'c', (UINT) 'c', &m_nAveCharWidth);

	for (int i = 0; i < MAXTABSTOPS; i++)
		m_nTabStopPositions[i] = (i+1) * m_nAveCharWidth * m_nTabPosition;

	// draw focus rectangle when no items in listbox
	if (lpDIS->itemID == (UINT)-1)
	{
		if (lpDIS->itemAction & ODA_FOCUS)
			pDC->DrawFocusRect(&lpDIS->rcItem);
		return;
	}
	else
	{
		int selChange   = lpDIS->itemAction & ODA_SELECT;
		int focusChange = lpDIS->itemAction & ODA_FOCUS;
		int drawEntire  = lpDIS->itemAction & ODA_DRAWENTIRE;

		if (selChange || drawEntire)
		{
			BOOL sel = lpDIS->itemState & ODS_SELECTED;

			int nLen = CListBox::GetTextLen(lpDIS->itemID);
			if (nLen != LB_ERR)
			{
				char *buf = new char [nLen + 10];
				ASSERT(buf);
				memset(buf,0,nLen + 10);
				if (buf && (GetTextWithColor(lpDIS->itemID, (LPSTR)buf) != LB_ERR))
				{
					//»¹Ô­ÑÕÉ«  modify by eidolon~
					char color[4] = "\0";
					strncpy(color,&buf[0],3);
					int tr = atoi(color);
					strncpy(color,&buf[3],3);
					int tg = atoi(color);
					strncpy(color,&buf[6],3);
					int tb = atoi(color);
					strncpy(color,&buf[9],3);
					int br = atoi(color);
					strncpy(color,&buf[12],3);
					int bg = atoi(color);
					strncpy(color,&buf[15],3);
					int bb = atoi(color);

					COLORREF it(RGB(tr,tg,tb));
					COLORREF ib(RGB(br,bg,bb));
					
					sel = 0;
					COLORREF textcolor = sel ? m_ColorHighlightText : it;
					oldtextcolor = pDC->SetTextColor(textcolor);
					COLORREF backgroundcolor = sel ? m_ColorHighlight : ib;
					oldbackgroundcolor = pDC->SetBkColor(backgroundcolor);

					// fill the rectangle with the background color the fast way
					pDC->ExtTextOut(0, 0, ETO_OPAQUE, &lpDIS->rcItem, NULL, 0, NULL);

					if(strlen(buf)>18)
						pDC->TabbedTextOut(lpDIS->rcItem.left, lpDIS->rcItem.top, &buf[18],
							strlen(&buf[2]), MAXTABSTOPS, (LPINT)m_nTabStopPositions, 0);

					CSize size;
					size = pDC->GetOutputTextExtent(&buf[2]);
					int nScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
					size.cx += nScrollBarWidth;	// in case of vertical scrollbar

					int cxExtent = (size.cx > m_cxExtent) ? size.cx : m_cxExtent;

					if (cxExtent > m_cxExtent)
					{
						m_cxExtent = cxExtent;
						SetHorizontalExtent(m_cxExtent+(m_cxExtent/32));
					}
				}
				if (buf)
					delete [] buf;
			}
		}

		if (focusChange || (drawEntire && (lpDIS->itemState & ODS_FOCUS)))
			pDC->DrawFocusRect(&lpDIS->rcItem);
	}
}

//////////////////////////////////////////////////////////////////////////////
// GetTextWithColor - get text string with color bytes
int CXListBox::GetTextWithColor(int nIndex, LPTSTR lpszBuffer) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return LB_ERR;
	}

	ASSERT(lpszBuffer);
	lpszBuffer[0] = 0;
	return CListBox::GetText(nIndex, lpszBuffer);
}

//////////////////////////////////////////////////////////////////////////////
// GetTextWithColor - get text string with color bytes
void CXListBox::GetTextWithColor(int nIndex, CString& rString) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	rString.Empty();
	CListBox::GetText(nIndex, rString);
}

//////////////////////////////////////////////////////////////////////////////
// GetText - for compatibility with CListBox (no color bytes)
int CXListBox::GetText(int nIndex, LPTSTR lpszBuffer) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return LB_ERR;
	}

	ASSERT(lpszBuffer);

	lpszBuffer[0] = 0;

	int nRet = CListBox::GetText(nIndex, lpszBuffer);

	int n = strlen(lpszBuffer);
	if (n > 2)
		memcpy(&lpszBuffer[0], &lpszBuffer[2], n-1);	// copy nul too

	return nRet;
}

//////////////////////////////////////////////////////////////////////////////
// GetText - for compatibility with CListBox (no color bytes)
void CXListBox::GetText(int nIndex, CString& rString) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	CString str;
	str.Empty();
	CListBox::GetText(nIndex, str);
	if ((!str.IsEmpty()) && (str.GetLength() > 2))
		rString = str.Mid(2);
	else
		rString.Empty();
}

//////////////////////////////////////////////////////////////////////////////
// GetTextLen - for compatibility with CListBox (no color bytes)
int CXListBox::GetTextLen(int nIndex) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return LB_ERR;
	}

	int n = CListBox::GetTextLen(nIndex);
	if (n != LB_ERR && n >= 2)
		n -= 2;
	return n;
}

//////////////////////////////////////////////////////////////////////////////
// SearchString
int CXListBox::SearchString(int nStartAfter, LPCTSTR lpszItem, BOOL bExact) const
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return LB_ERR;
	}

	// start the search after specified index
	int nIndex = nStartAfter + 1;

	int nCount = GetCount();
	if (nCount == LB_ERR)
		return LB_ERR;

	// convert string to search for to lower case
	CString strItem;
	strItem = lpszItem;
	strItem.MakeLower();
	int nItemSize = strItem.GetLength();

	CString strText;

	// search until end
	for ( ; nIndex < nCount; nIndex++)
	{
		GetText(nIndex, strText);
		strText.MakeLower();
		if (!bExact)
			strText = strText.Left(nItemSize);
		if (strText == strItem)
			return nIndex;
	}

	// if we started at beginning there is no more to do, search failed
	if (nStartAfter == -1)
		return LB_ERR;

	// search until we reach beginning index
	for (nIndex = 0; (nIndex <= nStartAfter) && (nIndex < nCount); nIndex++)
	{
		GetText(nIndex, strText);
		strText.MakeLower();
		if (!bExact)
			strText = strText.Left(nItemSize);
		if (strText == strItem)
			return nIndex;
	}

	return LB_ERR;
}

//////////////////////////////////////////////////////////////////////////////
// FindString
int CXListBox::FindString(int nStartAfter, LPCTSTR lpszItem) const
{
	return SearchString(nStartAfter, lpszItem, FALSE);
}

//////////////////////////////////////////////////////////////////////////////
// SelectString
int CXListBox::SelectString(int nStartAfter, LPCTSTR lpszItem)
{
	int rc = SearchString(nStartAfter, lpszItem, FALSE);
	if (rc != LB_ERR)
		SetCurSel(rc);
	return rc;
}

//////////////////////////////////////////////////////////////////////////////
// FindStringExact
int CXListBox::FindStringExact(int nStartAfter, LPCTSTR lpszItem) const
{
	return SearchString(nStartAfter, lpszItem, TRUE);
}

//////////////////////////////////////////////////////////////////////////////
// InsertString - override to add text color
int CXListBox::InsertString(int nIndex, COLORREF tc, COLORREF bc, LPCTSTR lpszItem)
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return LB_ERR;
	}

	CString s;
	s.Empty();
	s = lpszItem;



//	UINT nColor = (UINT) tc;
//	ASSERT(nColor < 16);
//	if (nColor >= 16)
//		tc = Black;

	// don't display \r or \n characters
	int i;
	while ((i = s.FindOneOf("\r\n")) != -1)
		s.SetAt(i, ' ');

	// first character in string is color -- add 1 to color
	// to avoid asserts by CString class
	CString t;
	t .Empty();

	char szBuff[1024] = "\0";
	_stprintf(szBuff,"%03d%03d%03d%03d%03d%03d",GetRValue(tc),GetGValue(tc),GetBValue(tc),
										GetRValue(bc),GetGValue(bc),GetBValue(bc));
	t += szBuff;
	t += s;

	// try to insert the string into the listbox
	i = CListBox::InsertString(nIndex, t);

	return i;
}

//////////////////////////////////////////////////////////////////////////////
// AddString - override to add text color
void CXListBox::AddString(LPCTSTR lpszItem)
{
	AddLine(RGB(0,0,0), RGB(255,255,255), lpszItem);
}

//////////////////////////////////////////////////////////////////////////////
// AddLine
void CXListBox::AddLine(COLORREF tc, COLORREF bc, LPCTSTR lpszLine)
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	CString s;
	s.Empty();
	s = lpszLine;

	if (!m_bColor)
	{
		tc = RGB(0,0,0);			// to force black-only text
		bc = RGB(255,255,255);
	}

	/*UINT nColor = (UINT) tc;
	ASSERT(nColor < 16);
	if (nColor >= 16)
		tc = Black;*/

	// don't display \r or \n characters
	int i;
	while ((i = s.FindOneOf("\r\n")) != -1)
		s.SetAt(i, ' ');

	// first character in string is color -- add 1 to color
	// to avoid asserts by CString class

	//modify by eidolon~
	CString t;
	t .Empty();

	char szBuff[1024] = "\0";
	_stprintf(szBuff,"%03d%03d%03d%03d%03d%03d",GetRValue(tc),GetGValue(tc),GetBValue(tc),
										GetRValue(bc),GetGValue(bc),GetBValue(bc));
	t += szBuff;
	t += s;
	// try to add the string to the listbox
	i = CListBox::AddString(t);
	if (i == LB_ERRSPACE)
	{
		// will get LB_ERRSPACE if listbox is out of memory
		int n = GetCount();

		if (n == LB_ERR)
			return;

		if (n < 2)
			return;

		// try to delete some strings to free up some room --
		// don't spend too much time deleting strings, since
		// we might be getting a burst of messages
		n = (n < 20) ? (n-1) : 20;
		if (n <= 0)
			n = 1;

		SetRedraw(FALSE);
		for (i = 0; i < n; i++)
			DeleteString(0);

		i = CListBox::AddString(t);

		SetRedraw(TRUE);
	}

	if (i >= 0)
	{
		SetTopIndex(i);
	}

	SetCurSel(-1);
}

///////////////////////////////////////////////////////////////////////////////
// Printf
//void _cdecl CXListBox::Printf(Color tc, Color bc, UINT nID, LPCTSTR lpszFmt, ...)
//{
//	char buf[1024], fmt[1024];
//	va_list marker;
//
//	// load format string from string resource if
//	// a resource ID was specified
//	if (nID)
//	{
//		CString s;
//		if (!s.LoadString(nID))
//		{
//			sprintf(s.GetBufferSetLength(80), "Failed to load string resource %u",
//				nID);
//			s.ReleaseBuffer(-1);
//		}
//		strncpy(fmt, s, sizeof(fmt)-1);
//	}
//	else
//	{
//		// format string was passed as parameter
//		strncpy(fmt, lpszFmt, sizeof(fmt)-1);
//	}
//	fmt[sizeof(fmt)-1] = 0;
//
//	// combine output string and variables
//	va_start(marker, lpszFmt);
//	_vsnprintf(buf, sizeof(buf)-1, fmt, marker);
//	va_end(marker);
//	buf[sizeof(buf)-1] = 0;
//
//	AddLine(tc, bc, buf);
//}

//////////////////////////////////////////////////////////////////////////////
// EnableColor
void CXListBox::EnableColor (BOOL bEnable)
{
	m_bColor = bEnable;
}

//////////////////////////////////////////////////////////////////////////////
// SetTabPosition
void CXListBox::SetTabPosition(int nSpacesPerTab)
{
	ASSERT(nSpacesPerTab > 0 && nSpacesPerTab < 11);

	m_nTabPosition = nSpacesPerTab;

	CDC* pDC = GetDC();

	if (pDC)
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);

		pDC->GetCharWidth((UINT) ' ', (UINT) ' ', &m_nSpaceWidth);
		pDC->GetCharWidth((UINT) '9', (UINT) '9', &m_nAveCharWidth);

		for (int i = 0; i < MAXTABSTOPS; i++)
			m_nTabStopPositions[i] = (i+1) * m_nAveCharWidth * m_nTabPosition;

		ReleaseDC(pDC);
	}
}

//////////////////////////////////////////////////////////////////////////////
// GetVisibleLines
int CXListBox::GetVisibleLines()
{
	int nCount = 0;

	CDC* pDC = GetDC();

	if (pDC)
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		int h = tm.tmHeight + tm.tmInternalLeading;
		ReleaseDC(pDC);

		CRect rect;
		GetClientRect(&rect);
		nCount = rect.Height() / h;
	}
	return nCount;
}

//////////////////////////////////////////////////////////////////////////////
// ResetContent
void CXListBox::ResetContent()
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	CListBox::ResetContent();

	m_cxExtent = 0;

	SetHorizontalExtent(m_cxExtent);
}

//////////////////////////////////////////////////////////////////////////////
// SetFont
void CXListBox::SetFont(CFont *pFont, BOOL bRedraw)
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	CListBox::SetFont(pFont, bRedraw);

	CDC* pDC = GetDC();

	if (pDC)
	{
		CFont *pOldFont = pDC->SelectObject(pFont);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		int h = tm.tmHeight;
		SetItemHeight(0, h);

		pDC->SelectObject(pOldFont);

		pDC->GetCharWidth((UINT) ' ', (UINT) ' ', &m_nSpaceWidth);
		pDC->GetCharWidth((UINT) '9', (UINT) '9', &m_nAveCharWidth);

		for (int i = 0; i < MAXTABSTOPS; i++)
			m_nTabStopPositions[i] = (i+1) * m_nAveCharWidth * m_nTabPosition;

		ReleaseDC(pDC);
	}

	m_cxExtent = 0;
}

//////////////////////////////////////////////////////////////////////////////
// OnLButtonDblClk
void CXListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDblClk(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
// OnContextMenu
void CXListBox::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (m_nContextMenuId == -1)
	{
		TRACE("   no context menu\n");
		return;
	}

	CMenu menu;
	if (!menu.LoadMenu(m_nContextMenuId))
	{
		TRACE("   ERROR failed to load %d\n", m_nContextMenuId);
		return;
	}

	menu.GetSubMenu(0)->TrackPopupMenu(0,
		point.x, point.y, this, NULL);
}

//////////////////////////////////////////////////////////////////////////////
// OnEditCopy
void CXListBox::OnEditCopy()
{
	CString str;
	str.Empty();

	int nCount = GetCount();
	int nSel = 0;

	for (int i = 0; i < nCount; i++)
	{
		if (GetSel(i) > 0)
		{
			CString s;
			s.Empty();
			GetText(i, s);
			if (!s.IsEmpty())
			{
				nSel++;
				s.TrimLeft("\r\n");
				s.TrimRight("\r\n");
				if (s.Find('\n') == -1)
					s += "\n";
				s.Replace("\t", " ");
				str += s;
			}
		}
	}

	if (!str.IsEmpty())
		CClipboard::SetText(str);
}

//////////////////////////////////////////////////////////////////////////////
// OnEditClear
void CXListBox::OnEditClear()
{
	ResetContent();
}

//////////////////////////////////////////////////////////////////////////////
// OnEditSelectAll
void CXListBox::OnEditSelectAll()
{
	if (!::IsWindow(m_hWnd))
	{
		ASSERT(FALSE);
		return;
	}

	SelItemRange(TRUE, 0, GetCount()-1);
}
