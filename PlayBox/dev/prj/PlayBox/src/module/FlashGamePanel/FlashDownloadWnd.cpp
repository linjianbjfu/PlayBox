#include "stdafx.h"
#include "FlashDownloadWnd.h"
#include "YL_StringUtil.h"
#include "../../Global/GlobalSwfPath.h"
#include "../../Core/CDataManager.h"
#include "src/GUI/CommonControl/xSkinButton.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../TabMan/TabWndFactory.h"
#include "AppConfig/config/ConfigSettingDef.h"
#include "OneFlashGameControl.h"

static int AD_WIDTH  = 360;
static int AD_HEIGHT = 300;

IMPLEMENT_DYNAMIC(CFlashGameDownloadWnd, CWnd)

CFlashGameDownloadWnd::CFlashGameDownloadWnd(COneFlashGameControl* pCtrl) 
	: m_pBtnClose(new CxSkinButton),
	m_isFailed(false), m_dDownPercent(0), m_colBk(RGB(255,255,255)),
	m_colText(RGB(103,103,103)), m_pBmpBgM(NULL), m_pBmpBgR(NULL),
	m_pBmpM(NULL), m_pAdWebWnd(new MyWebBrowserWnd),
	m_pCtrl(pCtrl)
{
	m_pCtrl->SetFlashDownload(this);
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME,
		CONF_SETTING_CONFIG_FLASH_DOWN_AD_URL, m_strAdUrl);
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

CFlashGameDownloadWnd::~CFlashGameDownloadWnd()
{
	m_font.DeleteObject();
	m_pAdWebWnd = NULL;
}

BEGIN_MESSAGE_MAP(CFlashGameDownloadWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CLOSETAB,	OnBnCloseTab)
END_MESSAGE_MAP()

int CFlashGameDownloadWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd(this);
	CRect rectNULL(0,0,0,0);
	m_pBtnClose->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CLOSETAB);
	m_pAdWebWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		rectNULL, this, TabWndFactory::GetInstance()->GenerateID());
	LoadSkin();	

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this, "DownPercentDlg_btnClose", m_pBtnClose);
	pLayoutMgr->CreateControlPane( this,"DownPercentDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"DownPercentDlg","normal" );

	if (!m_strAdUrl.empty())
		m_pAdWebWnd->Navigate(m_strAdUrl);
	return 0;
}

void CFlashGameDownloadWnd::OnPaint()
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
	DrawProgress( &MemDC2, rect );
	//3写信息文字
	DrawInfoText( &MemDC2, rect );
	//////内存DC中绘画-end
	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC2, 0, 0, SRCCOPY );
	MemDC2.RestoreDC(iSaveDc);
	MemDC2.DeleteDC();	
	MemBitmap2.DeleteObject();
	__super::OnPaint();
}

void CFlashGameDownloadWnd::DrawProgress( CDC* pDc, CRect rc )
{
	if(!m_pBmpBg_Full || !m_pBmpBgM || !m_pBmpBgR)
		return;

	CRect rect;
	m_pAdWebWnd->GetWindowRect(&rect);
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

void CFlashGameDownloadWnd::DrawInfoText( CDC* pDc, CRect rc )
{
	//确定进度条范围:中心点向下50像素
	CRect rect;
	m_pAdWebWnd->GetWindowRect(&rect);
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

void CFlashGameDownloadWnd::SetDownPercent( double dPercent )
{
	m_dDownPercent = dPercent;
	CRect rect;
	GetClientRect(&rect);
	rect.top +=(360);
	InvalidateRect (rect);
}

void CFlashGameDownloadWnd::SetText( vector<string>& vecText )
{
	m_vecText.clear();
	copy( vecText.begin(), vecText.end(), back_inserter(m_vecText) );
}

void CFlashGameDownloadWnd::SetFailed( bool bFailed )
{
	m_isFailed = bFailed;
}

void CFlashGameDownloadWnd::LoadSkin()
{
	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	m_colText  = pSkinMgr->GetColor( "DownProgressWndTextColor" );
	m_pBmpBg_Full  = pSkinMgr->GetDibBmp( "Progress_Bg_Full" );
	m_pBmpBgM  = pSkinMgr->GetDibBmp( "Progress_Bg_M" );
	m_pBmpBgR  = pSkinMgr->GetDibBmp( "Progress_Bg_R" );
}
void CFlashGameDownloadWnd::OnDestroy()
{
	AfxGetUIManager()->UIGetSkinMgr()->RemoveSkinWnd( this );
	__super::OnDestroy();
}

void CFlashGameDownloadWnd::OnSize(UINT nType, int cx, int cy)
{
	if (cx != 0 && cy != 0)
	{
		CRect r;
		r.left = (cx - AD_WIDTH)/2;
		r.right = r.left + AD_WIDTH;
		r.top = (cy - AD_HEIGHT)/2;
		r.bottom = r.top + AD_HEIGHT;
		m_pAdWebWnd->MoveWindow(&r);
	}
	__super::OnSize(nType,cx,cy);
}

void CFlashGameDownloadWnd::OnBnCloseTab()
{
	TAB_ITEM tabItem;
	GLOBAL_TABBARDATA->ITabBar_GetCurItem(tabItem);
	GLOBAL_TABBARDATA->ITabBar_DeleteTab(tabItem);
}