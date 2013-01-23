#include "stdafx.h"
#include "TabBarWnd.h"
#include "CDataManager.h"
#include "TabPageControl.h"
#include "../../gui/util/BalloonHelp.h"
#include "../../gui/util/WToolTips.h"

#define ONE_TAB_MAX_WIDTH	100
#define WIDTH_TEXT_LEFT_MARGIN 5 //tab上文字距离左边的间距
#define HEITH_CLOSE_TO_TOP  6	 //关闭按钮和上边的距离
#define	WIDTH_CLOSE_TO_RIGHT 4	 //关闭按钮和右边的距离

IMPLEMENT_DYNAMIC(CTabBarWnd, CBasicWnd)
CTabBarWnd::CTabBarWnd()
{
	m_iTabCur    = -1;
	m_iTabOver   = -1;
	m_iCloseOver = -1;
	m_bOverNewWindow = false;

	m_pBmpRectNormalLeft   = NULL;
	m_pBmpRectNormalMiddle = NULL;
	m_pBmpRectNormalRight  = NULL;

	m_pBmpRectOverLeft   = NULL;
	m_pBmpRectOverMiddle = NULL;
	m_pBmpRectOverRight  = NULL;

	m_pBmpRectDownLeft   = NULL;
	m_pBmpRectDownMiddle = NULL;
	m_pBmpRectDownRight  = NULL;

	m_pBmpCloseNormal = NULL;
	m_pBmpCloseOver   = NULL;
	m_pBmpCloseDown	  = NULL;

	m_pBmpSelCloseNormal = NULL;
	m_pBmpSelCloseOver   = NULL;
	m_pBmpSelCloseDown   = NULL;

	m_pBmpNewWindowNormal = NULL;
	m_pBmpNewWindowOver   = NULL;
	m_pBmpNewWindowDown   = NULL;

	m_colNormalTextColor = RGB(205,205,205);
	m_colDownTextColor	 = RGB(255,175,38);
	m_colMask            = RGB(0,255,0);

	ZeroMemory( &m_bmih, sizeof( BITMAPINFOHEADER ) ); 
	m_bmih.biSize                  = sizeof (BITMAPINFOHEADER) ; 
	m_bmih.biPlanes                = 1 ; 
	m_bmih.biBitCount              = 32;        //这里一定要是32 
	m_bmih.biCompression          = BI_RGB ; 
	m_bmih.biSizeImage            = 0 ; 
	m_bmih.biXPelsPerMeter        = 0 ; 
	m_bmih.biYPelsPerMeter        = 0 ; 
	m_bmih.biClrUsed              = 0 ; 
	m_bmih.biClrImportant         = 0 ; 

	//字体
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));

	lf.lfHeight = 12;
	lf.lfWidth  = 0;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfStrikeOut = FALSE;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
	lf.lfUnderline = false;
	strcpy(lf.lfFaceName, "宋体");

	lf.lfWeight = FW_NORMAL;
	m_fontNormal.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	m_fontDown.CreateFontIndirect(&lf);

	m_pToolTip = new CCWToolTips();

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_TABBAR,(ITabBarObserver*) this);
}

CTabBarWnd::~CTabBarWnd()
{
	m_fontNormal.DeleteObject();
	m_fontDown.DeleteObject();
	delete m_pToolTip;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_TABBAR,(ITabBarObserver*) this);
}

BEGIN_MESSAGE_MAP(CTabBarWnd, CBasicWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


int CTabBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_pToolTip->m_pClientWnd = GetParent();
	m_pToolTip->m_hParent	 = m_hWnd;
	m_pToolTip->CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",TTS_ALWAYSTIP,CRect(0,0,0,0),NULL,0);	
	m_pToolTip->m_clrBK = RGB(255, 255, 225);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateControlPane( this,"tabbarWnd","normal");
	pLayoutMgr->CreateBmpPane( this,"tabbarWnd","normal" );

	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );
	return 0;
}

void CTabBarWnd::LoadSkin()
{
	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	m_pBmpRectNormalLeft   = pSkinMgr->GetDibBmp( "TabBarRectLeftNormal" );
	m_pBmpRectNormalMiddle = pSkinMgr->GetDibBmp( "TabBarRectMiddleNormal" );
	m_pBmpRectNormalRight  = pSkinMgr->GetDibBmp( "TabBarRectRightNormal" );

	m_pBmpRectOverLeft   = pSkinMgr->GetDibBmp( "TabBarRectLeftOver" );
	m_pBmpRectOverMiddle = pSkinMgr->GetDibBmp( "TabBarRectMiddleOver" );
	m_pBmpRectOverRight  = pSkinMgr->GetDibBmp( "TabBarRectRightOver" );

	m_pBmpRectDownLeft   = pSkinMgr->GetDibBmp( "TabBarRectLeftDown" );
	m_pBmpRectDownMiddle = pSkinMgr->GetDibBmp( "TabBarRectMiddleDown" );
	m_pBmpRectDownRight  = pSkinMgr->GetDibBmp( "TabBarRectRightDown" );

	m_pBmpCloseNormal = pSkinMgr->GetDibBmp( "TabBarCloseNormal" );
	m_pBmpCloseOver   = pSkinMgr->GetDibBmp( "TabBarCloseOver" );
	m_pBmpCloseDown   = pSkinMgr->GetDibBmp( "TabBarCloseDown" );

	m_pBmpSelCloseNormal = pSkinMgr->GetDibBmp( "TabBarSelCloseNormal" );
	m_pBmpSelCloseOver   = pSkinMgr->GetDibBmp( "TabBarSelCloseOver" );
	m_pBmpSelCloseDown   = pSkinMgr->GetDibBmp( "TabBarSelCloseDown" );

	m_pBmpNewWindowNormal = pSkinMgr->GetDibBmp( "TabbarNewWindowNormal" );
	m_pBmpNewWindowOver = pSkinMgr->GetDibBmp( "TabbarNewWindowOver" );
	m_pBmpNewWindowDown = pSkinMgr->GetDibBmp( "TabbarNewWindowDown" );		

	m_colNormalTextColor = pSkinMgr->GetColor( "TabBarNormalTextColor" );
	m_colDownTextColor	 = pSkinMgr->GetColor( "TabBarDownTextColor" );
}

void CTabBarWnd::OnSize(UINT nType, int cx, int cy)
{
	CalcTabPosition();
	__super::OnSize(nType,cx,cy);
}

void CTabBarWnd::OnPaint()
{		
	CPaintDC dc(this);
	
	CRect rcClient;
	GetClientRect( &rcClient );
	CDC dcMem;
	dcMem.CreateCompatibleDC( &dc );
	
	BYTE * pBits = NULL; 
	m_bmih.biWidth  = rcClient.Width(); 
	m_bmih.biHeight = rcClient.Height(); 
	HBITMAP bmp = CreateDIBSection(NULL, (BITMAPINFO*)&m_bmih, 
		0, (VOID**)&pBits, NULL, 0) ; 

	dcMem.SelectObject( bmp );
	::DeleteObject( bmp );
	DrawBar( &dcMem );
	dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY );
	dcMem.DeleteDC();
}

void CTabBarWnd::DrawBar( CDC* pDc )
{
	//1画背景
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,pDc,true );
	CDibBitmap* pBmp = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp( "TabBarBK" );
	if( pBmp != NULL )
	{
		CRect rc;
		GetClientRect( &rc );
		pBmp->SetCDibRect( rc );
		pBmp->Draw( pDc, FALSE );
	}
	if(m_vecTab.empty())
		return;

	//2画正常rect和close
	for( int i = 0; i < m_vecTab.size(); i++ )
	{
		DrawTabItemRect( pDc, m_vecTab[i], m_pBmpRectNormalLeft,
			m_pBmpRectNormalMiddle, m_pBmpRectNormalRight, 
			m_vecClose[i], m_pBmpCloseNormal,
			m_vecTi[i].strTitle, false );
	}
	//2画当前rect和close
	if( m_iTabCur != -1 )
	{
		DrawTabItemRect( pDc, m_vecTab[m_iTabCur], m_pBmpRectDownLeft,
			m_pBmpRectDownMiddle, m_pBmpRectDownRight, 
			m_vecClose[m_iTabCur], m_pBmpSelCloseNormal, 
			m_vecTi[m_iTabCur].strTitle, true );
	}
	//3画鼠标over的rect
	if( m_iTabOver != -1 && m_iTabOver != m_iTabCur )
	{
		DrawTabItemRect( pDc, m_vecTab[m_iTabOver], m_pBmpRectOverLeft,
			m_pBmpRectOverMiddle, m_pBmpRectOverRight, 
			m_vecClose[m_iTabOver], m_pBmpCloseNormal, 
			m_vecTi[m_iTabOver].strTitle, false );
	}
	//4画鼠标over的close
	if( m_iCloseOver != -1 && m_iCloseOver != 0 )
	{
		if (m_iCloseOver == m_iTabCur)
		{
			m_pBmpSelCloseOver->SetCDibRect( m_vecClose[m_iCloseOver] );
			m_pBmpSelCloseOver->Draw( pDc, TRUE, m_colMask );
		}else
		{
			m_pBmpCloseOver->SetCDibRect( m_vecClose[m_iCloseOver] );
			m_pBmpCloseOver->Draw( pDc, TRUE, m_colMask );
		}
	}
	//5画newwindow按钮
	if (m_bOverNewWindow)
	{
		m_pBmpNewWindowOver->SetCDibRect( m_rectNewWindow );
		m_pBmpNewWindowOver->Draw( pDc, TRUE, m_colMask );
	}else
	{
		m_pBmpNewWindowNormal->SetCDibRect( m_rectNewWindow );
		m_pBmpNewWindowNormal->Draw( pDc, TRUE, m_colMask );
	}
}

void CTabBarWnd::DrawTabItemRect( CDC* pDc, CRect& rcRect,
		CDibBitmap* pRectLeft, CDibBitmap* pRectMiddle, CDibBitmap* pRectRight,
		CRect& rcClose, CDibBitmap* pClose, string strText, bool bDown )
{
	if( pRectLeft == NULL || pRectMiddle == NULL || pRectRight == NULL || pClose == NULL )
		return;
	CRect rcLeft(rcRect);
	rcLeft.right = rcLeft.left + pRectLeft->GetWidth();
	pRectLeft->SetCDibRect( rcLeft );
	pRectLeft->Draw( pDc, TRUE, m_colMask );

	CRect rcRight(rcRect);
	rcRight.left = rcRight.right - pRectRight->GetWidth();
	pRectRight->SetCDibRect( rcRight );
	pRectRight->Draw( pDc, TRUE, m_colMask );	

	CRect rcMiddle(rcRect);
	rcMiddle.left  = rcLeft.right;
	rcMiddle.right = rcRight.left;
	pRectMiddle->SetCDibRect( rcMiddle );
	pRectMiddle->Draw( pDc, TRUE, m_colMask );	

	//写标题
	CRect rcText( rcRect );
	rcText.left  = rcText.left + WIDTH_TEXT_LEFT_MARGIN;
	rcText.right = rcClose.left;
	if( !rcText.IsRectEmpty() )
	{
		pDc->SetBkMode( TRANSPARENT );

		COLORREF colOld = pDc->GetTextColor();
		CFont* fontOld = NULL;
		if( bDown )
		{
			pDc->SetTextColor( m_colDownTextColor );
			fontOld = pDc->SelectObject( &m_fontDown );
		}else
		{
			pDc->SetTextColor( m_colNormalTextColor );
			fontOld = pDc->SelectObject( &m_fontNormal );
		}
		pDc->DrawText( strText.c_str(), -1, &rcText, /*DT_LEFT*/DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS );

		pDc->SetTextColor( colOld );
		if( fontOld != NULL )
			pDc->SelectObject( fontOld );
	}
	//画关闭按钮
	pClose->SetCDibRect( rcClose );
	pClose->Draw( pDc, TRUE, m_colMask );
}

void CTabBarWnd::ITabBarOb_CreateNewTab(TAB_ITEM &item)
{
	AfxGetMainWindow()->Invalidate ();

	CalcTabPosition();
	Invalidate();
}

void CTabBarWnd::ITabBarOb_OpenExistTab(TAB_ITEM& item)
{
	AfxGetMainWindow ()->Invalidate ();

	CalcTabPosition();
	Invalidate();
}

void CTabBarWnd::ITabBarOb_DelTab(TAB_ITEM& item)
{
	CalcTabPosition();
	Invalidate();
}

void CTabBarWnd::ITabBarOb_OpenTabError( int iErrorCode )
{
	//冒泡提示
	int iCur = GLOBAL_TABBARDATA->ITabBar_GetCurPos();
	if( iCur == 0 )
		iCur = 1;

	CRect rcClose = m_vecClose[iCur];
	CPoint point;
	point.x = rcClose.right - (rcClose.Width()/2);
	point.y = rcClose.bottom;
	ClientToScreen( &point );
	AfxGetMainWindow()->ScreenToClient(&point);
	CBalloonHelp::LaunchBalloon( "提示","打开窗口过多，请关闭无用窗口",
		point,IDI_INFORMATION,CBalloonHelp::unSHOW_CLOSE_BUTTON|CBalloonHelp::unDISABLE_XP_SHADOW,
		AfxGetMainWindow(),"",5000);
}

void CTabBarWnd::CalcTabPosition()
{
	m_iTabCur = GLOBAL_TABBARDATA->ITabBar_GetCurPos();

	m_vecTab.clear();
	m_vecClose.clear();
	m_vecTi.clear();
	m_rectNewWindow.SetRectEmpty();
	GLOBAL_TABBARDATA->ITabBar_GetTabBarData( m_vecTi );

	CRect rc;
	GetClientRect( &rc );
	if( rc.Width() == 0 || rc.Height() == 0 || m_vecTi.size() == 0 )
		return;

	//iWidth	每个btn的宽度
	int iWidth = (int)(rc.Width() / m_vecTi.size());
	if( iWidth > ONE_TAB_MAX_WIDTH )
		iWidth = ONE_TAB_MAX_WIDTH;

	CRect rcBtn( rc );
	CRect rcClose;
	int iStartPos = 9; //最左边空出9个像素
	int iNewWindowStartPos = iStartPos;
	//所有btn，包括btn上的关闭按钮
	for( int i = 0; i < m_vecTi.size(); i++ )
	{
		rcBtn.left = i * iWidth + iStartPos;
		rcBtn.right = rcBtn.left + iWidth;
		iNewWindowStartPos = rcBtn.right;
		if( i == m_iTabCur )
			rcBtn.top = rcBtn.bottom - m_pBmpRectDownMiddle->GetHeight();
		else
			rcBtn.top = rcBtn.bottom - m_pBmpRectNormalMiddle->GetHeight();
		
		m_vecTab.push_back( rcBtn );
	
		if( i == 0 || i == 1 ) //头2个不能关闭
		{
			rcClose.right = rcBtn.right - WIDTH_CLOSE_TO_RIGHT;
			rcClose.left  = rcClose.right;	
			rcClose.top   = rcBtn.top + HEITH_CLOSE_TO_TOP;			
			rcClose.bottom = rcClose.top;
		}else
		{
			rcClose.right = rcBtn.right - WIDTH_CLOSE_TO_RIGHT;
			rcClose.left  = rcClose.right - m_pBmpCloseNormal->GetWidth();	
			rcClose.top   = rcBtn.top + HEITH_CLOSE_TO_TOP;			
			rcClose.bottom = rcClose.top + m_pBmpCloseNormal->GetHeight();
		}
		m_vecClose.push_back( rcClose );
	}
	m_rectNewWindow.left = iNewWindowStartPos;
	m_rectNewWindow.right = m_rectNewWindow.left + m_pBmpNewWindowNormal->GetWidth();
	m_rectNewWindow.bottom = rcBtn.bottom;
	m_rectNewWindow.top = m_rectNewWindow.bottom - m_pBmpNewWindowNormal->GetHeight();
}

void CTabBarWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	AfxGetMainWindow()->Invalidate();
	//是否落在了close上
	if( m_iCloseOver != -1 
		&& m_iCloseOver != 0 )
	{
		TAB_ITEM tmpTI = m_vecTi[m_iCloseOver];
		GLOBAL_TABBARDATA->ITabBar_DeleteTab( tmpTI );

		m_iCloseOver = -1;
		m_iTabOver = -1;

		CalcTabPosition ();
		Invalidate ();
		return;
	}
	//是否落在tab上
	if( m_iTabOver != -1 )
	{
		TAB_ITEM tmpTI = m_vecTi[m_iTabOver];
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tmpTI );
	}
	//是否落在了newwindow按钮上
	if ( m_bOverNewWindow )
	{
		TAB_ITEM tItem;
		tItem.eumType = TAB_BROWSER;
		tItem.strName = "新开窗口";
		tItem.strParam = "";
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
	}
	__super::OnLButtonDown(nFlags, point);
}

void CTabBarWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	int iOver = GetOverClose( point );
	if( iOver != m_iCloseOver )
	{
		m_iCloseOver = iOver;
		CalcTabPosition ();
		Invalidate();
	}

	iOver = GetOverTab( point );
	if( iOver != m_iTabOver )
	{
		m_iTabOver = iOver;
		CalcTabPosition();
		Invalidate();
	}
	m_bOverNewWindow = m_rectNewWindow.PtInRect(point);

	if( m_iTabOver != -1 )
	{
		CPoint pt(0,0);
		GetCursorPos(&pt);
		if( m_pToolTip->m_lastPoint != pt )
			m_pToolTip->HideTooTips();

		CString info = m_vecTi[m_iTabOver].strName.c_str();
		m_pToolTip->ShowToolTips( info,pt);
	}
	__super::OnMouseMove(nFlags, point);
}

int CTabBarWnd::GetOverTab( CPoint pt )
{
	for( int i=0; i<m_vecTab.size(); i++ )
	{
		if( m_vecTab[i].PtInRect( pt ) )
			return i;
	}
	return -1;
}

int CTabBarWnd::GetOverClose( CPoint pt )
{	
	for( int i=0; i<m_vecClose.size(); i++ )
	{
		if( m_vecClose[i].PtInRect( pt ) )
			return i;
	}
	return -1;
}

void CTabBarWnd::OnDestroy()
{
	AfxGetUIManager()->UIGetSkinMgr()->RemoveSkinWnd( this );
	m_pToolTip->DestroyWindow();
	__super::OnDestroy();
}