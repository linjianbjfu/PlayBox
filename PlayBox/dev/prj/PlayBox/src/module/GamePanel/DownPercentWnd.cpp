#include "stdafx.h"
#include "DownPercentWnd.h"
#include "YL_StringUtil.h"
#include "../../Global/GlobalSwfPath.h"
#include "../../Core/CDataManager.h"
#include "src/GUI/CommonControl/xSkinButton.h"

IMPLEMENT_DYNAMIC(DownPercentWnd, CWnd)
DownPercentWnd::DownPercentWnd()
{
	m_pBtnClose = new CxSkinButton;
	m_isFailed		 = false;
	m_dDownPercent	 = 0;
 	m_colBk			 = RGB(255,255,255);
	m_colText		 = RGB(103,103,103);
	m_rcFlash		 = CRect(0, 0, 0, 0);
	m_ad.strLinkUrl.clear();
	m_ad.strPicPath.clear();
	m_rcFlash.SetRectEmpty();

	m_pBmpBgL  = NULL;
	m_pBmpBgM  = NULL;
	m_pBmpBgR  = NULL;
	m_pBmpM    = NULL;
	m_pImgAd   = NULL;
	m_pWnd = NULL;
	m_bNavi = true;

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
	m_font.CreateFontIndirect(&lf);
}

DownPercentWnd::~DownPercentWnd()
{
	m_font.DeleteObject();
	/*if(m_pWnd != NULL)
	{
		delete m_pWnd;
		m_pWnd = NULL;
	}*/
}

BEGIN_MESSAGE_MAP(DownPercentWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_CLOSETAB,	OnBnCloseTab)
END_MESSAGE_MAP()

int DownPercentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );
	CRect rectNULL(0,0,0,0);
	m_pBtnClose->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CLOSETAB);
	LoadSkin();	

	if( GLOBAL_ADDATA->GetLoadingPicAd( m_ad ) )
	{
		USES_CONVERSION;
		m_pImgAd = new Image( A2W(m_ad.strPicPath.c_str()), FALSE );
	}
	return 0;
}

void DownPercentWnd::OnPaint()
{
	CPaintDC dc(this);
	//双缓冲
	CRect rect;
	GetClientRect( &rect );

	CDC MemDC2;
	CBitmap MemBitmap2;

	MemDC2.CreateCompatibleDC( &dc ); 
	int iSaveDc = MemDC2.SaveDC();

	MemDC2.SelectObject(&m_font);	
	MemDC2.SetTextColor(m_colText);
	MemBitmap2.CreateCompatibleBitmap( &dc, rect.Width(), rect.Height() );//创建字体大小长度info1size
	MemDC2.SelectObject(&MemBitmap2);
	MemDC2.SetBkMode(TRANSPARENT);
	//////内存DC中绘画-begin
	//1画背景色
	MemDC2.FillSolidRect( &rect, m_colBk );
	//2画进度条
	
	//rect.top = rect.bottom - 90 -14;
	DrawProgress( &MemDC2, rect );
	//3写信息文字
	DrawInfoText( &MemDC2, rect );
	//4画广告
	DrawAd( &MemDC2, rect );
	//////内存DC中绘画-end
	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC2, 0, 0, SRCCOPY );
	MemDC2.RestoreDC(iSaveDc);
	MemDC2.DeleteDC();	
	MemBitmap2.DeleteObject();	
}

void DownPercentWnd::DrawProgress( CDC* pDc, CRect rc )
{
	if( m_pBmpBg_Full == NULL || m_pBmpBgM == NULL 
		|| m_pBmpBgR == NULL )
	{
		return;
	}
	//确定进度条范围：中心点向下30像素
	//CPoint ptCenter = rc.CenterPoint();
	//CRect rcProgressBarBg( rc );
	//rcProgressBarBg.left += 30;
	//rcProgressBarBg.right -= 70;
	//rcProgressBarBg.top = /*ptCenter.y + 30*/rc.top+40+320 +15; 
	
	if(m_pWnd == NULL)
	{
		DrawAd(pDc,rc);
	}
	CRect rect;
	m_pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	CRect rcProgressBarBg( rect );
	rcProgressBarBg.top =rcProgressBarBg.bottom +40;
	rcProgressBarBg.bottom = rcProgressBarBg.top + m_pBmpBg_Full->GetHeight();
	rcProgressBarBg.right -=30;
	//画进度条背景
	//Paint the full progress.
	CRect rcBgL( rcProgressBarBg );
	m_pBmpBg_Full->SetCDibRect( rcBgL );
	m_pBmpBg_Full->Draw( pDc, TRUE, RGB(255,0,0) );
	//Paint the right background.
	CRect rcBgR( rcProgressBarBg );
	rcBgR.left = rcBgR.right - m_pBmpBgR->GetWidth();
	m_pBmpBgR->SetCDibRect( rcBgR );
	m_pBmpBgR->Draw( pDc, TRUE, RGB(255,0,0) );
	//Paint the middle parts.
	CRect rcBgM( rcProgressBarBg );
	m_dDownPercent <0 ? 0: m_dDownPercent;
	rcBgM.left  = rcProgressBarBg.left +m_dDownPercent*rcProgressBarBg.Width() ;
	rcBgM.right = rcBgR.left;
	m_pBmpBgM->SetCDibRect( rcBgM );
	m_pBmpBgM->Draw( pDc, TRUE, RGB(255,0,0) );

	//Draw close button.
	CRect rcBtn(rcProgressBarBg);
	rcBtn.right +=10;
	m_pBtnClose->MoveWindow(rcBtn.right,rcBtn.top,20,20,true);
}

void DownPercentWnd::DrawInfoText( CDC* pDc, CRect rc )
{
	//确定进度条范围:中心点向下50像素
	/*CPoint ptCenter = rc.CenterPoint();
	CRect rcInfo( rc );
	rcInfo.top = rc.bottom-90 +15+ 35;
	rcInfo.bottom = rcInfo.top + 15;*/
	CRect rect;
	m_pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top = rect.bottom +10;
	rect.bottom =rect.top +25;
	CRect rcInfo(rect);
	//写字
	pDc->SelectObject( &m_font );
	pDc->SetTextColor( m_colText );
	for( int i=0; i<m_vecText.size(); i++ )
	{
		pDc->DrawText( m_vecText[i].c_str(), -1, &rcInfo, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
		rcInfo.OffsetRect( 0, 20 ); //下移20像素
	}	
}
BOOL DownPercentWnd::AddDynamicView(LPCTSTR  lpszLabel, CRuntimeClass * pViewClass)
{
	TRY
	{
		if (m_pWnd != NULL)
			return FALSE;
		m_pWnd = (CMyHtmlView *)pViewClass->CreateObject ();
		if (m_pWnd == NULL)
			::AfxThrowMemoryException ();
	}
	CATCH_ALL(e)
	{
			return FALSE;
	}
	END_CATCH_ALL

	DWORD dwStyle = AFX_WS_DEFAULT_VIEW ;
	//Set the rect of dynamic created view.
	CRect rect;
	GetClientRect(&rect);
	rect.top += 40;
	rect.bottom =rect.top + 320;
	rect.right -=90;
	rect.left =rect.right - 460;
	rect.left > 0? rect.left:0;

	if(!m_pWnd->Create (NULL,NULL,dwStyle,rect,this,CMyHtmlView::IDD ))
		return FALSE;

	m_pWnd->OnInitialUpdate ();
	m_pWnd->EnableWindow (TRUE);
	m_pWnd->ShowWindow (SW_SHOW);

	return TRUE;
	return 0;
}

void DownPercentWnd::DrawAd( CDC* pDc, CRect rc)
{
	//static int i = 0;
	if(!this->AddDynamicView (_T(""),RUNTIME_CLASS(CMyHtmlView)))
		{
			return ;
		}
	if(m_pWnd != NULL&&m_bNavi == true)
	{
		m_pWnd->Navigate2 ("www.baidu.com");
		m_bNavi = false;
	}
	////确定范围:从最上方到中心点,居中显示
	//int iAdMaxHeight = rc.Height()/2;
	//int iAdMaxWidth  = rc.Width();

	//float fPerY = (float)(m_pImgAd->GetHeight()) / (float)iAdMaxHeight;
	//float fPerX = (float)(m_pImgAd->GetWidth())  / (float)iAdMaxWidth;

	//CPoint pCenter = rc.CenterPoint();
	//int iX = 0;
	//int iY = 0;
	//if( fPerY <= 1 && fPerX <= 1 )//图片原始大小显示
	//{
	//	iX = m_pImgAd->GetWidth();
	//	iY = m_pImgAd->GetHeight();
	//}else
	//if( fPerY > fPerX ) //图片太高
	//{
	//	iX = (float)(m_pImgAd->GetWidth()) / (float)fPerY;
	//	iY = iAdMaxHeight;
	//}else //图片太长
	//{
	//	iX = iAdMaxWidth;
	//	iY = (float)(m_pImgAd->GetHeight()) / (float)fPerX;
	//}
	//m_rcFlash.left = pCenter.x - ( iX / 2 );
	//m_rcFlash.top  = ( pCenter.y / 2 ) - ( iY / 2 );
	//m_rcFlash.right  = m_rcFlash.left + iX;
	//m_rcFlash.bottom = m_rcFlash.top  + iY;

	//Rect rcImg( m_rcFlash.left, m_rcFlash.top, m_rcFlash.Width(), m_rcFlash.Height() );
	//Graphics gc( pDc->GetSafeHdc() );
	//gc.DrawImage( m_pImgAd, rcImg );
}

void DownPercentWnd::SetDownPercent( double dPercent )
{
	m_dDownPercent = dPercent;
	CRect rect;
	GetClientRect(&rect);
	//rect.top += (rect.bottom -90 - 14);
	//rect.bottom -=90;
	rect.top +=(360);
	this->InvalidateRect (rect);
	//Invalidate();
}

void DownPercentWnd::SetText( vector<string>& vecText )
{
	m_vecText.clear();
	copy( vecText.begin(), vecText.end(), back_inserter(m_vecText) );
}

void DownPercentWnd::SetFailed( bool bFailed )
{
	m_isFailed = bFailed;
}

void DownPercentWnd::LoadSkin()
{
	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	m_colText  = pSkinMgr->GetColor( "DownProgressWndTextColor" );
	//m_pBmpBgL  = pSkinMgr->GetDibBmp( "Lo_Bg_L" );
	//m_pBmpBgM  = pSkinMgr->GetDibBmp( "Lo_Bg_M" );
	//m_pBmpBgR  = pSkinMgr->GetDibBmp( "Lo_Bg_R" );
	//m_pBmpM    = pSkinMgr->GetDibBmp( "Lo_M" );

	m_pBmpBg_Full  = pSkinMgr->GetDibBmp( "Progress_Bg_Full" );
	m_pBmpBgM  = pSkinMgr->GetDibBmp( "Progress_Bg_M" );
	m_pBmpBgR  = pSkinMgr->GetDibBmp( "Progress_Bg_R" );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this, "DownPercentDlg_btnClose", m_pBtnClose);
	pLayoutMgr->CreateControlPane( this,"DownPercentDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"DownPercentDlg","normal" );
}
void DownPercentWnd::OnDestroy()
{
	AfxGetUIManager()->UIGetSkinMgr()->RemoveSkinWnd( this );
	__super::OnDestroy();
}

void DownPercentWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	::SetCursor(::LoadCursor(NULL, m_rcFlash.PtInRect(point) ? IDC_HAND : IDC_ARROW));
	__super::OnMouseMove(nFlags, point);
}

void DownPercentWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rcFlash.PtInRect( point ) && 
		m_ad.strLinkUrl.length() > 0)
		ShellExecute(NULL, "open", "iexplore.exe", m_ad.strLinkUrl.c_str(),"", SW_SHOWNORMAL);	
	__super::OnLButtonDown(nFlags, point);
}
void DownPercentWnd::OnBnCloseTab()
{
	TAB_ITEM tabItem;
	GLOBAL_TABBARDATA->ITabBar_GetCurItem(tabItem);
	GLOBAL_TABBARDATA->ITabBar_DeleteTab(tabItem);
}
