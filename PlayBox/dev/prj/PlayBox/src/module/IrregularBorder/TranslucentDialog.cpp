#include "stdafx.h"
#include "TranslucentDialog.h"
#include <algorithm>
#include "../LayoutMgr/SeperateMode/WindowsMover.h"

CTranslucentDialog::CTranslucentDialog(UINT nIDTemplate, LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	,m_bEnableDrag(true)
	, m_bCenterAligned(true)
{
	m_pImage = NULL;
	m_nCx = m_nCy = 0;
}

CTranslucentDialog::~CTranslucentDialog()
{
	delete m_pImage;
	m_pImage = NULL;
}

BEGIN_MESSAGE_MAP(CTranslucentDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CTranslucentDialog::SetEnableDrag(bool bEnableDrag)
{
	m_bEnableDrag = bEnableDrag;
}

void CTranslucentDialog::SetCenterAligned(bool bCenterAligned)
{
	m_bCenterAligned = bCenterAligned;
}

BOOL CTranslucentDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	// must be WS_POPUP
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_CHILD) == 0);

	m_blend.BlendOp = 0;
	m_blend.BlendFlags = 0;
	m_blend.AlphaFormat = 1;
	m_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA
	return TRUE;
}

BOOL CTranslucentDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CTranslucentDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Must be overlapped
	VERIFY((lpCreateStruct->style & WS_POPUP) == 0);
	VERIFY((lpCreateStruct->style & WS_CHILD) == 0);

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pLastImage = NULL;
	ModifyStyle(WS_CAPTION, WS_POPUP, 0);
	return 0;
}

void CTranslucentDialog::UpdatePosition(POINT p)
{
	m_ptPos = p;
}

int CTranslucentDialog::GetImageWidth()
{
	if(m_pImage == NULL)
		return 0;
	else
		return m_pImage->GetWidth();
}

int CTranslucentDialog::GetImageHeight()
{
	if(m_pImage == NULL)
		return 0;
	else
		return m_pImage->GetHeight();
}

void CTranslucentDialog::UpdateView(BOOL bMustUpdate)
{
	if(m_pImage == NULL)
	{ 
		return;
	}
	if(m_pLastImage == m_pImage && !bMustUpdate)
	{
		if(m_pLastImage == NULL)
		{
			return;
		}
		TRACE("w: %d ,h: %d, wold:%d, hold%d\r\n", m_pImage->GetWidth(),m_pImage->GetHeight(),m_pImage->GetWidth(),m_pImage->GetHeight());
		if(m_pLastImage->GetWidth() == m_pImage->GetWidth() 
			&& m_pLastImage->GetHeight() == m_pImage->GetHeight())
		{
			return;
		}
	}

	m_pLastImage = m_pImage;
	
	SIZE sizeWindow = { m_pImage->GetWidth(), m_pImage->GetHeight() };

	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	CWindowsMover::GetInstance()->SetDontMoveContent();

	HDC hDC = ::GetDC(m_hWnd);
	HDC hdcMemory = CreateCompatibleDC(hDC);

	RECT rcWindow;
	GetWindowRect(&rcWindow);

	CRect rcClient;
	GetClientRect(&rcClient);

	SIZE sizeAllWindow = {rcClient.Width(), rcClient.Height()};
	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = rcClient.Width();//sizeWindow.cx;   
	stBmpInfoHeader.biHeight = rcClient.Height();//sizeWindow.cy;   
	stBmpInfoHeader.biPlanes = 1;   
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

	PVOID pvBits = NULL;   
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
	ASSERT(hbmpMem != NULL);

	memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
	if(hbmpMem)   
	{   
		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);
		//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
		POINT ptWinPos = { rcWindow.left, rcWindow.top };
		Gdiplus::Graphics graph(hdcMemory);
		//graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);
		graph.SetSmoothingMode(SmoothingModeAntiAlias);
		graph.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graph.DrawImage(m_pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);

		int nStart, nSize;
		if(m_position == LEFT_TOP_CORNER && m_ptPos.x + 2 < sizeWindow.cx && m_ptPos.y + 2< sizeWindow.cy)
		{
			for(int i = 0 ; i<sizeAllWindow.cy - (m_ptPos.y + 2) ; i++)
			{
				nStart = sizeWindow.cx * 4 * i + (m_ptPos.x + 2) * 4;
				nSize = (m_ptPos.x + 2 < 0) ? sizeWindow.cx * 4 : ( sizeWindow.cx - m_ptPos.x - 2) * 4;
				if(nStart < 0 || nSize + nStart > stBmpInfoHeader.biSizeImage || nSize < 0)
					continue;
				memset(((BYTE*)pvBits) + nStart , 0, nSize);
			}
		}

		if(m_position == RIGHT_TOP_CORNER && m_ptPos.x - 2 > 0 && m_ptPos.y +2 < sizeWindow.cy)
		{
			for(int i = 0 ; i<sizeAllWindow.cy - (m_ptPos.y + 2) ; i++)
			{
				nStart = sizeWindow.cx * 4 * i;
				nSize = (m_ptPos.x - 2 > sizeWindow.cx) ? sizeWindow.cx * 4 : (m_ptPos.x - 2) * 4;
				if(nStart < 0 || nSize + nStart > stBmpInfoHeader.biSizeImage || nSize < 0)
					continue;
				memset(((BYTE*)pvBits) + nStart , 0, nSize);
			}

		}
		
		if(m_position == LEFT_BOTTOM_CORNER&& m_ptPos.x + 2 < sizeWindow.cx  && m_ptPos.y - 2 > 0)
		{
			for(int i = (sizeWindow.cy - m_ptPos.y + 2) ; i<sizeWindow.cy ; i++)
			{
				nStart = sizeWindow.cx * 4 * i + (m_ptPos.x + 2) * 4;
				nSize = (m_ptPos.x + 2 < 0) ? sizeWindow.cx * 4 : ( sizeWindow.cx - m_ptPos.x - 2) * 4;
				if(nStart < 0 || nSize + nStart > stBmpInfoHeader.biSizeImage|| nSize < 0)
					continue;
				memset(((BYTE*)pvBits) + nStart , 0, nSize);
			}
		}
		if(m_position == RIGHT_BOTTOM_CORNER && m_ptPos.x - 2 > 0  && m_ptPos.y - 2 > 0)
		{
			for(int i = (sizeWindow.cy - m_ptPos.y + 2) ; i<sizeWindow.cy ; i++)
			{
				nStart = sizeWindow.cx * 4 * i;
				nSize = (m_ptPos.x - 2 > sizeWindow.cx) ? sizeWindow.cx * 4 : (m_ptPos.x - 2) * 4;
				if(nStart < 0 || nSize + nStart > stBmpInfoHeader.biSizeImage|| nSize < 0)
					continue;
				memset(((BYTE*)pvBits) + nStart , 0, nSize);
			}
		}

		if(!CWindowsMover::GetInstance()->m_bDontMoveContent)
			Sleep(50);

		HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
		typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
		MYFUNC UpdateLayeredWindow;
		UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
		POINT ptSrc = { 0, 0};
		int ret = UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeAllWindow, hdcMemory, &ptSrc, 0, &m_blend, 2);

		graph.ReleaseHDC(hdcMemory);
		::SelectObject( hdcMemory, hbmpOld);   
		::DeleteObject(hbmpMem);  
		FreeLibrary(hFuncInst);
	}
	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);
}

void CTranslucentDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag)
	{
		//::SendMessage(AfxGetMainWindow()->m_hWnd, WM_SYSCOMMAND, 0xF012, 0);
		WPARAM wParam;
		LPARAM lParam;

		ClientToScreen( &point);
		::ScreenToClient( AfxGetMainWindow()->m_hWnd,&point );

		wParam = nFlags;
		lParam = (WORD)point.x;
		lParam += ((WORD)point.y) << 16;		

		::SendMessage(AfxGetMainWindow()->m_hWnd,WM_LBUTTONDOWN, wParam,lParam );	
	}
	CDialog::OnLButtonDown(nFlags, point);
	AfxGetMainWindow()->SetActiveWindow();
	SetWindowPos(AfxGetMainWindow(), 0,0,0,0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
} 

void CTranslucentDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag)
	{
		WPARAM wParam;
		LPARAM lParam;

		ClientToScreen( &point);
		::ScreenToClient( AfxGetMainWindow()->m_hWnd,&point );

		wParam = nFlags;
		lParam = (WORD)point.x;
		lParam += ((WORD)point.y) << 16;		
		//::PostMessage( AfxGetMainWindow()->m_hWnd,WM_NCLBUTTONDOWN, HTCAPTION, lParam );	
		::SendMessage(AfxGetMainWindow()->m_hWnd,WM_LBUTTONUP, wParam,lParam );
	}
	CDialog::OnLButtonUp(nFlags, point);
	AfxGetMainWindow()->SetActiveWindow();
	//AfxGetMainWindow()->SetFocus();
	SetWindowPos(AfxGetMainWindow(), 0,0,0,0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
}

void CTranslucentDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag && (nFlags & MK_LBUTTON))
	{
		WPARAM wParam;
		LPARAM lParam;

		ClientToScreen( &point);
		::ScreenToClient( AfxGetMainWindow()->m_hWnd,&point );

		wParam = nFlags;
		lParam = (WORD)point.x;
		lParam += ((WORD)point.y) << 16;		
		//::PostMessage( AfxGetMainWindow()->m_hWnd,WM_NCLBUTTONDOWN, HTCAPTION, lParam );	
		::SendMessage(AfxGetMainWindow()->m_hWnd,WM_MOUSEMOVE, wParam,lParam );
	}
	CDialog::OnMouseMove(nFlags, point);
}
