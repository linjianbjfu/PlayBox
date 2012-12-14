// PassMouseWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "hostwnd.h"
#include "WindowsMover.h"
#include ".\hostwnd.h"
// CPassMouseWnd

CHostWnd::CHostWnd(CWnd* pParent /*=NULL*/): CDialog(CHostWnd::IDD, pParent)
{
	m_isMainWnd = false;
	m_nL = m_nR = m_nT = m_nB = 0;
	m_pWnd  = NULL;
	m_isResizing = FALSE;
	m_ptStartResizing.x = 0;
	m_ptStartResizing.y = 0;
	m_rectStarResizing.left = 0;
	m_rectStarResizing.right = 0;
	m_rectStarResizing.top = 0;
	m_rectStarResizing.bottom = 0;
	m_iDirection = 0;
	m_bMovable = true;
	m_strContent = "";
}

CHostWnd::~CHostWnd()
{
	if(m_pWnd != NULL)
		::SetParent(m_pWnd->m_hWnd,m_hParentWnd);
}


BEGIN_MESSAGE_MAP(CHostWnd, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(ID_BTN_CLOSE, OnBnClickedClose)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


BOOL CHostWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWindowsMover::GetInstance()->AddWndindow(this, m_isMainWnd);

	m_btnClose.Create(NULL,WS_VISIBLE,CRect(0,0,0,0),this,ID_BTN_CLOSE);

	if(m_strContent != "")
	{
		AfxGetUIManager()->UIGetLayoutMgr()->RegisterCtrl( this,"Close",&m_btnClose);
		AfxGetUIManager()->UIGetLayoutMgr()->CreateControlPane(this,m_strContent, "");
		AfxGetUIManager()->UIGetLayoutMgr()->CreateBmpPane(this, m_strContent, "");
		m_nMinCx = atoi(AfxGetUIManager()->UIGetLayoutMgr()->GetWndParameter(m_hWnd, "sizeXMin"));
		m_nMinCy = atoi(AfxGetUIManager()->UIGetLayoutMgr()->GetWndParameter(m_hWnd, "sizeYMin"));
	}

	return TRUE;
}

void CHostWnd::OnLButtonDown(UINT nFlags, CPoint point)
{	
;//	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)&point );	

	m_iDirection = 0;

	if( _needChangeCursor(point))
	{
		m_bChangeCursor  = true;
		SetCapture();		
		m_isResizing = TRUE;
		m_ptStartResizing = point;
		ClientToScreen( &m_ptStartResizing );
		GetWindowRect( m_rectStarResizing );
		GetWindowRect(&m_rcMain);
		DrawMoveFrame(m_rcMain);
		return ;
	}		
	if(!m_isResizing)
		CWindowsMover::GetInstance()->DoMouseDown(m_hWnd, point);
	m_isResizing = FALSE;

}

void CHostWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_isResizing)
		CWindowsMover::GetInstance()->DoMouseUp(this->m_hWnd, point);

	if( m_isResizing )
	{
		DrawMoveFrame(m_rcMain);
		CRect rc;
		this->GetWindowRect(&rc);
		m_nDeltaHeight = m_rcMain.Height() - rc.Height();
		m_nDeltaWidth = m_rcMain.Width() - rc.Width();
		MoveWindow( m_rcMain,true );
		::InvalidateRect(::GetDesktopWindow(), &m_rcMain, true);
		CWindowsMover::GetInstance()->DoMouseResizingFinished(m_hWnd, nFlags, point, m_iDirection);
	}
	if(::GetCapture() == m_hWnd)
		::ReleaseCapture();
	m_isResizing = FALSE;

}

void CHostWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_isResizing)
	{
		CWindowsMover::GetInstance()->DoMouseMove(m_hWnd, nFlags, point);
	}
	if( !m_isResizing )
	{
		if( !_needChangeCursor( point ) )
		{
			if( m_bChangeCursor )
			{
				HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_ARROW);		
				m_iDirection = 0;
				::SetCursor(m_hCursor);			
				m_bChangeCursor = false;
			}
		}
		else
		{
			m_bChangeCursor = true;
		}
	}

	if( nFlags & MK_LBUTTON && m_isResizing )//鼠标左键处于点击状态下
	{
		CPoint tmpPT = point;
		ClientToScreen(&tmpPT);
		//先获得当前Pt 和 开始点击 的位置的 差值
		CPoint diff;
		diff.x = tmpPT.x - m_ptStartResizing.x;
		diff.y = tmpPT.y - m_ptStartResizing.y;

		CRect newRect;

		if( m_iDirection & DIRECTION_LEFT )
		{
			newRect.left = m_rectStarResizing.left + diff.x;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_TOP )
			{
				newRect.top += diff.y;
			}
			else if( m_iDirection & DIRECTION_BOTTOM )
			{
				newRect.bottom += diff.y;
			}				
		}
		else if( m_iDirection & DIRECTION_RIGHT )
		{		
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right + diff.x;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_TOP )
			{
				newRect.top += diff.y;
			}
			else if( m_iDirection & DIRECTION_BOTTOM )
			{
				newRect.bottom += diff.y;
			}		
		}
		else if( m_iDirection & DIRECTION_TOP )
		{
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top + diff.y;

			if( m_iDirection & DIRECTION_LEFT )
			{
				newRect.left += diff.x;
			}
			else if( m_iDirection & DIRECTION_RIGHT )
			{
				newRect.right += diff.x;
			}				
		}
		else if( m_iDirection & DIRECTION_BOTTOM )
		{
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom + diff.y;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_LEFT )
			{
				newRect.left += diff.x;
			}
			else if( m_iDirection & DIRECTION_RIGHT )
			{
				newRect.right += diff.x;
			}			
		}
		else
		{
			__super::OnMouseMove( nFlags,point );
			return;
		}

		if(newRect.Width() < m_nMinCx)
		{
			if(m_iDirection & DIRECTION_RIGHT)
				newRect.right = newRect.left + m_nMinCx;
			else
				newRect.left = newRect.right - m_nMinCx;
		}
		if(newRect.Height() < m_nMinCy)
		{
			if(m_iDirection & DIRECTION_BOTTOM)
				newRect.bottom = newRect.top + m_nMinCy;
			else
				newRect.top = newRect.bottom - m_nMinCy;
		}
		DrawMoveFrame(m_rcMain);
		m_rcMain = newRect;
		DrawMoveFrame(m_rcMain);

	}
	__super::OnMouseMove( nFlags,point );

}

void CHostWnd::OnPaint()
{
	CPaintDC dc(this);
	//if(this->m_isMainWnd)
	//	dc.TextOut(20,20,"THE MAIN WINDOW");
	//CRect rc;
	//this->GetClientRect(&rc);
	//dc.FillSolidRect(&rc, RGB(100,100,100));
	//dc.Draw3dRect(&rc, RGB(222,111,111), RGB(111,55,55));
	if(m_strContent != "")
		AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd,&dc,true );
}

void CHostWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx,cy);

	if( AfxGetUIManager()->UIGetLayoutMgr() != NULL )
	{
		DWORD count = ::GetTickCount();
		CDC* dc = GetDC();
		HRGN hrgn = AfxGetUIManager()->UIGetSkinMgr()->GetFrameRGN(cx, cy , dc);
		ReleaseDC(dc);
		::SetWindowRgn(m_hWnd, hrgn, true);
		AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(this->m_hWnd);
	}

	if(m_strContent != "")
		AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout( m_hWnd );
	else
		MoveContentWindow();

	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CHostWnd::OnBnClickedOk()
{
	OnOK();
}

BOOL CHostWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	return CDialog::OnEraseBkgnd(pDC);
}
void CHostWnd::MoveContentWindow()
{
	CRect rc;
	if(m_pWnd == NULL)
		return;

	this->GetWindowRect(&rc);
	rc.left += m_nL;
	rc.top += m_nT;
	rc.right -= m_nR;
	rc.bottom -= m_nB;
	ScreenToClient(&rc);
	m_pWnd->MoveWindow(&rc, true);
	m_pWnd->ShowWindow(SW_SHOW);
}

void CHostWnd::HostWindow(CWnd *pWnd, bool isMain)
{
	if(m_strContent == "")
	{
		m_hParentWnd = (pWnd->GetParent() == NULL)? NULL: pWnd->GetParent()->m_hWnd;
		MoveContentWindow();
		m_pWnd = pWnd;
		pWnd->SetParent(this);

	}
}

void CHostWnd::DetachWindow()
{
	if(m_pWnd != NULL)
		::SetParent(m_pWnd->m_hWnd , m_hParentWnd);
}


bool CHostWnd::_needChangeCursor(const CPoint& Pt )
{
	RECT fullRect;

	if(!m_bMovable)
		return false;

	::GetWindowRect(m_hWnd,&fullRect);//获得当前窗口相对于整个屏幕空间的rect			

	//更新 cursor 外观
	//在窗口的 左下角的 6*6 像素区域，鼠标变箭头（re_size窗口）
	if( (Pt.x >= 0 && Pt.x <= 8 ) && (Pt.y > fullRect.bottom - fullRect.top - 8) )
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENESW);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;
		m_iDirection |= DIRECTION_BOTTOM;

		return TRUE;
	}
	//在窗口的 右下角的 6*6 像素区域，鼠标变箭头（re_size窗口）
	if( (Pt.x > fullRect.right - fullRect.left - 8) && (Pt.y > fullRect.bottom - fullRect.top - 8) )
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;
		m_iDirection |= DIRECTION_BOTTOM;

		return TRUE;
	}
	//在窗口的 左上角的 6*6 像素区域，鼠标变箭头（re_size窗口）
	if( (Pt.x >= 0 && Pt.x <= 4 ) && (Pt.y >= 0 && Pt.y <= 4) )
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	//在窗口的 右上角的 6*6 像素区域，鼠标变箭头（re_size窗口）
	if( (Pt.x > fullRect.right - fullRect.left - 8) && (Pt.y >= 0 && Pt.y <= 8))
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENESW);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	//在窗口的 边框区域（左 右 各 3 个像素），鼠标变箭头（IDC_SIZEWE 东西）
	if(Pt.x >= 0 && Pt.x <= 8)
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;

		return TRUE;
	}
	else if(Pt.x >= fullRect.right - fullRect.left -8 && Pt.x <= fullRect.right - fullRect.left)
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;		
		return TRUE;
	}
	//在窗口的 边框区域（上 下 3个像素），鼠标变箭头（IDC_SIZEWE 东西）
	if(Pt.y >= 0 && Pt.y <= 3)
	{	
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENS);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	else if(Pt.y >= fullRect.bottom - fullRect.top -8 && Pt.y <= fullRect.bottom - fullRect.top)
	{
		//更换鼠标的形状，变成斜向的箭头
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENS);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_BOTTOM;
		return TRUE;
	}

	return FALSE;
}

void CHostWnd::DrawMoveFrame(CRect rc)
{
	CWnd* pParentWnd = CWnd::FromHandle(::GetDesktopWindow());//GetParent();
	CWindowDC dc(pParentWnd);


	//CRect rc;
	//this->GetWindowRect(&rc);
	//rc -= rcWin.TopLeft();					 

	CBitmap bmp;

	bmp.LoadBitmap(IDB_FRAME_LINE);

	CBrush brush(&bmp);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		4, 
		rc.Height(), 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		rc.Width(), 
		4, 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.bottom - 4, 
		rc.Width(), 
		4, 
		PATINVERT);

	dc.PatBlt(
		rc.right, 
		rc.top, 
		4, 
		rc.Height(), 
		PATINVERT);

	dc.SelectObject(pOldBrush);
}

void CHostWnd::OnBnClickedClose()
{
	CRect rc;
	m_btnClose.GetWindowRect(&rc);
	if(m_btnClose.IsWindowVisible() && rc.Width() != 0 && rc.Height() != 0)
		ShowWindow(SW_HIDE);
}
void CHostWnd::OnOK()
{
}

void CHostWnd::OnCancel()
{
}

BOOL CHostWnd::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CHostWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);
	AfxGetMainWindow()->MoveWindow(&CRect(lpwndpos->x, lpwndpos->y, 0,0));
}
