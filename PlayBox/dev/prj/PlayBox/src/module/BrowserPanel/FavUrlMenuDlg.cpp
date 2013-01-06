// FavUrlMenuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FavUrlMenuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFavUrlMenuDlg 对话框

CFavUrlMenuDlg::CFavUrlMenuDlg(UINT uIDD,
							   char*szParentPath/* =NULL */,
							   BOOL bAddFav /* = TRUE */,
							   CWnd* pParent /* = NULL */)
	: CDialog(uIDD, pParent)
{
	m_IDD = uIDD;
	m_bAddFav = bAddFav;
	m_pParentWnd = pParent;
	if (szParentPath) {
		m_strParentPath = szParentPath;
	}
	else
	{
		char szPath[MAX_PATH] = {0};

		if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_FAVORITES, FALSE) )
		{
			m_strParentPath = szPath;
		}
	}

	m_nCurSel = -1;
	m_pChildDlg	= NULL;

	m_font.CreateFont(12,      // nHeight
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
		"宋体");

	LOGFONT logfont;
	m_font.GetLogFont(&logfont);
	SetUIParam(logfont);
}

void CFavUrlMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFavUrlMenuDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_MENU_DESTROYED, OnChildClosed)
	ON_MESSAGE(WM_MENU_CICKED, OnClickedItem)
	ON_WM_CLOSE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

int CFavUrlMenuDlg::GetUrls( CString strPath, CArray<ITEM, ITEM&> &arrItems )
{
	CStringArray	arrSubDir;
	int curCnt = 0;

	if(strPath[strPath.GetLength() - 1] != _T('\\'))
		strPath += _T('\\');
	CString strFind = strPath + "*.*";

	WIN32_FIND_DATA	findData;
	HANDLE	hFile = NULL;

	hFile = FindFirstFile(strFind, &findData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if ( strcmp(".", findData.cFileName )==0
				|| strcmp("..", findData.cFileName)==0)
			{
				continue;
			}

			// 略过隐藏文件和系统文件
			if ( (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				|| (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
			{
				continue;
			}

			// 目录
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				arrSubDir.Add(findData.cFileName);
			}

			// 文件
			CString strFileName = findData.cFileName;
			strFileName.MakeLower();
			if (strFileName.Right(4) == ".url")
			{
				ITEM item;
				item.type = ITEM_TYPE_URL;
				item.strPath = strPath + strFileName;

				strFileName = strFileName.Left(strFileName.GetLength() - 4);
				item.strName = strFileName;

				arrItems.Add(item);
				curCnt++;
			}

		} while (FindNextFile(hFile, &findData));
	}
	FindClose(hFile);

	INT_PTR nSubDirNum = arrSubDir.GetSize();
	for (INT_PTR i=0; i<nSubDirNum; i++)
	{
		CString strSubDirName = arrSubDir.GetAt(i);

		CArray<ITEM, ITEM&> aItems;
		int n = GetUrls(strPath+strSubDirName, aItems);
		// if (n != 0)	// 不添加空文件夹
		if (1)
		{
			ITEM item;
			item.type = ITEM_TYPE_DIRECTORY;
			item.strName = strSubDirName;
			item.strPath = strPath+strSubDirName;

			arrItems.Add(item);
		}
	}

	return curCnt;
}

BOOL CFavUrlMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetFont(&m_font);

	if (m_bAddFav)
	{
		ITEM item;
		item.type = ITEM_TYPE_BTN_ADD;
		item.strName = "添加到收藏夹";
		item.strPath = m_strParentPath;
		m_arrItems.Add(item);

		item.type = ITEM_TYPE_SEPARATOR;
		m_arrItems.Add(item);

		GetUrls(m_strParentPath, m_arrItems);
	}
	else {
		ITEM item;
		item.type = ITEM_TYPE_BTN_OPEN;
		item.strName = "打开";
		item.strPath = m_strParentPath;
		m_arrItems.Add(item);

		item.type = ITEM_TYPE_BTN_REMOE;
		item.strName = "删除";
		item.strPath = m_strParentPath;
		m_arrItems.Add(item);
	}

	// 获取最长字符串所占宽度
	CClientDC dc(this);
	int nTextWidthMax = 0;
	CSize size;
	dc.SelectObject(&m_font);
	for (INT_PTR i=0; i<m_arrItems.GetSize(); i++)
	{
		CString strText = m_arrItems.GetAt(i).strName;
		size = dc.GetTextExtent(strText);

		if (size.cx > nTextWidthMax)
		{
			nTextWidthMax = size.cx;
		}
	}
	nTextWidthMax += 20;
	size.cx = nTextWidthMax;

	m_rctFirsItem = CRect(CPoint(m_nBorderWidth, m_nBorderWidth), CSize(size.cx+m_nSpacingHeigth*2, size.cy+m_nSpacingHeigth));


	CRect rctWnd(0, 0, 0, 0);
	rctWnd.right = m_nBorderWidth*2 + m_rctFirsItem.Width();
	rctWnd.bottom= m_nBorderWidth*2 + m_rctFirsItem.Height()*m_arrItems.GetSize();

	if (m_nBorderWidth % 2)
	{
		rctWnd.right += 2;
		rctWnd.bottom += 2;
	}

 	dc.BeginPath();
 	dc.RoundRect(rctWnd, CPoint(m_nBorderWidth, m_nBorderWidth));
 	dc.EndPath();
  	HRGN hRgn = PathToRegion(dc.m_hDC);
 	SetWindowRgn(hRgn, FALSE);
	GetRgnBox(hRgn, &rctWnd);
	SetWindowPos(NULL, 0, 0, rctWnd.Width(), rctWnd.Height(), SWP_NOMOVE);
 
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CFavUrlMenuDlg::OnPaint() 
{
	CPaintDC dc(this);

	if (m_nCurSel >= 0)
	{
		CDC dcMem;
		CBitmap bmpMem;
		dcMem.CreateCompatibleDC(&dc);
		bmpMem.CreateCompatibleBitmap(&dc, m_rctFirsItem.Width(), m_rctFirsItem.Height());
		dcMem.SelectObject(&bmpMem);


		dcMem.FillSolidRect(0, 0, m_rctFirsItem.Width(), m_rctFirsItem.Height(), m_rgbCursel);

		dcMem.SetBkMode(TRANSPARENT);
		dcMem.SetTextColor(m_rgbText);
		dcMem.SelectObject(&m_font);

		ITEM item = m_arrItems.GetAt(m_nCurSel);

		CRect rctText(m_nSpacingHeigth, 0, m_rctFirsItem.Width(), m_rctFirsItem.Height());
		dcMem.DrawText(item.strName, rctText, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		CRect rctPaint = m_rctFirsItem;
		rctPaint.top += m_rctFirsItem.Height()*m_nCurSel;
		rctPaint.bottom += m_rctFirsItem.Height()*m_nCurSel;

		dc.BitBlt(rctPaint.left, rctPaint.top, rctPaint.Width(), rctPaint.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.DeleteDC();
		bmpMem.DeleteObject();
	}
}

void CFavUrlMenuDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);

	PostMessage(WM_CLOSE);
}


void CFavUrlMenuDlg::SetUIParam(LOGFONT font,
								 unsigned uBorderWidth,	/* = BORDER_WIDTH_DEFAULT */
								 unsigned uSpacingWidth,/* = SPACING_HEIGHT_DEFAULT */
								 COLORREF rgbText,		/* = RGB_TEXT_DEFAULT */
								 COLORREF rgbBackGround,/* = RGB_BACKGROUND_DEFAULT */
								 COLORREF rgbCurSel,	/* = RGB_CURSEL_DEFAULT */
								 COLORREF rgbBorder		/* = RGB_BORDER_DEFAULT */)
{
	m_font.DeleteObject();
	m_font.CreateFontIndirect(&font);

	m_nFontHeight = font.lfHeight;
	m_nBorderWidth	= uBorderWidth;
	m_nSpacingHeigth= uSpacingWidth;

	m_rgbText		= rgbText;
	m_rgbBackGnd	= rgbBackGround;
	m_rgbCursel		= rgbCurSel;
	m_rgbBorder		= rgbBorder;
}

void CFavUrlMenuDlg::LineAtoB(CDC *pDC, CPoint ptA, CPoint ptB)
{
	CPoint pt = pDC->MoveTo(ptA);
	pDC->LineTo(ptB);
	pDC->MoveTo(pt);
}

BOOL CFavUrlMenuDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rctWnd;
	GetWindowRect(&rctWnd);
	ScreenToClient(rctWnd);
	
	CBrush brush;
	brush.CreateSolidBrush(m_rgbBackGnd);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_SOLID, m_nBorderWidth, m_rgbBorder);
	CPen *pOldPen = pDC->SelectObject(&pen);

	// 画圆角矩形
	rctWnd.left += m_nBorderWidth/2;
	rctWnd.top += m_nBorderWidth/2;
	rctWnd.right -= m_nBorderWidth/2;
	rctWnd.bottom -= m_nBorderWidth/2;

	if ((m_nBorderWidth % 2) )
	{
		rctWnd.right -= 1;
		rctWnd.bottom -= 1;
	}
	pDC->RoundRect(rctWnd, CPoint(m_nBorderWidth, m_nBorderWidth));
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	pDC->SelectObject(m_font);
	pDC->SetTextColor(m_rgbText);
	pDC->SetBkMode(TRANSPARENT);
	CRect rctItem = m_rctFirsItem;
	rctItem.left += m_nSpacingHeigth;
	for (INT_PTR i=0; i<m_arrItems.GetSize(); i++)
	{

		ITEM item = m_arrItems.GetAt(i);

		if (item.type != ITEM_TYPE_SEPARATOR)
		{
			pDC->DrawText(item.strName, rctItem, DT_VCENTER|DT_SINGLELINE);
			if (item.type == ITEM_TYPE_DIRECTORY)
			{	// 画小三角
				int y = rctItem.top + rctItem.Height()/2;
				int x = rctItem.right - m_nSpacingHeigth;
				CPoint ptA(x, y);
				
				y = y - rctItem.Height()/2 + m_nSpacingHeigth/2;
				x -= rctItem.Height()/2;
				CPoint ptB(x, y);
				CPoint ptC(x, y+rctItem.Height() - m_nSpacingHeigth);

				CBrush brush;
				brush.CreateSolidBrush(m_rgbBorder);
				CBrush* pOld = pDC->SelectObject(&brush);
				pDC->BeginPath();
				pDC->MoveTo(ptA.x,ptA.y);
				pDC->LineTo(ptB.x,ptB.y);
				pDC->LineTo(ptC.x,ptC.y);
				pDC->LineTo(ptA.x,ptA.y);
				pDC->EndPath();
				pDC->FillPath();
				pDC->SelectObject(&pOld);
				brush.DeleteObject();
			}
		}
		else
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, m_rgbBorder);
			CPen *pOld = pDC->SelectObject(&pen);

			int y = rctItem.top + rctItem.Height()/2;
			int x = rctItem.left;
			LineAtoB(pDC, CPoint(x,y), CPoint(x+rctItem.Width()-m_nSpacingHeigth, y));

			pDC->SelectObject(pOld);
			pen.DeleteObject();
		}
		rctItem.top += m_rctFirsItem.Height();
		rctItem.bottom = rctItem.top + m_rctFirsItem.Height();
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	return TRUE;
}

int CFavUrlMenuDlg::IsPtInItem(CPoint pt)
{
	CRect rctItem = m_rctFirsItem;

	int nCursel = -1;
	for (int i=0; i<m_arrItems.GetSize(); i++)
	{
		if (rctItem.PtInRect(pt))
		{
			nCursel = i;
		}
		rctItem.top += m_rctFirsItem.Height();
		rctItem.bottom = rctItem.top + m_rctFirsItem.Height();
	}

	if (nCursel>=0 && m_arrItems.GetAt(nCursel).type == ITEM_TYPE_SEPARATOR)
	{
		nCursel = -1;
	}

	return nCursel;
}

BOOL CFavUrlMenuDlg::ShowMenu(CPoint pt)
{	// 判断是否超出显示器边界,如果超出则重新定窗口位置
	CRect rctDeskTop, rctSelf;
	GetDesktopWindow()->GetWindowRect(rctDeskTop);
	GetWindowRect(rctSelf);

	if (pt.x + rctSelf.Width() > rctDeskTop.Width())
	{
		pt.x = rctDeskTop.Width() - rctSelf.Width();
	}

	if (pt.y + rctSelf.Height() > rctDeskTop.Height())
	{
		pt.y = rctDeskTop.Height()-rctSelf.Height();
	}

	SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE);

	return ShowWindow(SW_NORMAL);
}

void CFavUrlMenuDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nCurSel = IsPtInItem(point);

	if (m_nCurSel >= 0)
	{
		ITEM item = m_arrItems.GetAt(m_nCurSel);
		if (item.type == ITEM_TYPE_DIRECTORY)
		{
			OnMouseMove(nFlags, point);
		}
		else if (m_pParentWnd)
		{
			TRACE("\r\n  CFavUrlMenuDlg Send WM_MENU_CLICKED\r\n");
			m_pParentWnd->SendMessage(WM_MENU_CICKED, item.type, (LPARAM)item.strPath.GetBuffer(0));
			item.strPath.ReleaseBuffer();
		}
		PostMessage(WM_CLOSE);
	}
	//CDialog::OnLButtonDown(nFlags, point);
}

void CFavUrlMenuDlg::OnRButtonDown(UINT nFlags, CPoint point)
{

	m_nCurSel = IsPtInItem(point);

	if (m_nCurSel >= 0)
	{
		ITEM item = m_arrItems.GetAt(m_nCurSel);
		if (item.type == ITEM_TYPE_DIRECTORY)
		{
			OnMouseMove(nFlags, point);
		}
		else if (m_bAddFav)
		{
			if (m_pChildDlg != NULL)
			{
				delete m_pChildDlg;
				m_pChildDlg = NULL;
			}
			m_pChildDlg = new CFavUrlMenuDlg(m_IDD, item.strPath.GetBuffer(0), FALSE, this);
			item.strPath.ReleaseBuffer();
			m_pChildDlg->CreateDlg(MAKEINTRESOURCE(m_IDD), this);

			ClientToScreen(&point);
			m_pChildDlg->ShowMenu(point);

			PostMessage(WM_CLOSE);
		}
	}
}

void CFavUrlMenuDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	int iCurselOld = m_nCurSel;

	m_nCurSel = IsPtInItem(point);
	if (iCurselOld != m_nCurSel)
	{	// 鼠标在一个新的item里
		Invalidate();
		if (m_nCurSel >= 0)
		{
			ITEM item = m_arrItems.GetAt(m_nCurSel);
			if (item.type == ITEM_TYPE_DIRECTORY)
			{	// 是一个目录
				if (m_pChildDlg != NULL)
				{	// 正在显示子菜单,则先删除当前子菜单
					delete m_pChildDlg;
					m_pChildDlg = NULL;
				}
				m_pChildDlg = new CFavUrlMenuDlg(m_IDD, item.strPath.GetBuffer(0), TRUE, this);
				item.strPath.ReleaseBuffer();
				m_pChildDlg->CreateDlg(MAKEINTRESOURCE(m_IDD), this);

				CRect rctWnd;
				GetWindowRect(&rctWnd);
				CPoint pt(m_nBorderWidth, m_nBorderWidth);
				pt.y = pt.y + m_nCurSel*m_rctFirsItem.Height() + rctWnd.top;
				pt.x = rctWnd.right;

				m_pChildDlg->ShowMenu(pt);
			}
			else
			{
				if (m_pChildDlg)
				{	// 删除当前显示的子菜单
					delete m_pChildDlg;
					m_pChildDlg = NULL;
				}
			}
		}
		else if (m_pChildDlg != NULL)
		{	// 如果在显示子菜单
			// 则同样画出光标背景
			m_nCurSel = iCurselOld;
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CFavUrlMenuDlg::OnClose()
{
	if (m_pChildDlg == NULL )
	{
		if (m_pParentWnd)
		{
			m_pParentWnd->PostMessage(WM_MENU_DESTROYED);
		}
		TRACE("\r\n CFavUrlMenuDlg ON WM_CLOSE    \r\n");
		__super::EndDialog(0);
	}
}

// 响应自定义消息:WM_MENU_DESTROYED
LRESULT CFavUrlMenuDlg::OnChildClosed(WPARAM wParam, LPARAM lParam)
{
	TRACE("\r\n  CFavUrlMenuDlg  WM_MENU_DESTROYED \r\b");
	if (m_pChildDlg != NULL)
	{
		delete m_pChildDlg;
		m_pChildDlg = NULL;

		OnClose();
	}
	return 0;
}

// 自定义消息 WM_MENU_CLICKED
LRESULT CFavUrlMenuDlg::OnClickedItem(WPARAM wParam, LPARAM lParam)
{
	if (m_pParentWnd)
	{
		TRACE("\r\n  CFavUrlMenuDlg Send WM_MENU_CLICKED\r\n");
		return m_pParentWnd->SendMessage(WM_MENU_CICKED, wParam, lParam);
	}
	return PostMessage(WM_CLOSE);
}