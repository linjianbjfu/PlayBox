// SkinWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SkinWnd.h"

#include "BitMapOperate.h"
#include "WndColorMgr.h"

#include ".\IChangeColorMgr.h"
#include "../../appconfig/Config/ConfigSettingDef.h"
#include "YL_FileInfo.h"

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;
// CSkinWnd

IMPLEMENT_DYNCREATE(CSkinWnd, CFrameWnd)

CSkinWnd::CSkinWnd()
{
	miScrollPos =0;
	mbIsHasScroll = false;
	m_nMaxViewPos = 0;

	m_font.CreateFont(
		12,                        // nHeight
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
}

CSkinWnd::~CSkinWnd()
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		CSkinButton* pBtn = *m_colBtnList_itor;
		delete pBtn->m_BitMap;
		delete pBtn;

		*m_colBtnList_itor = NULL;
		m_colBtnList_itor++;
	}
	m_noDownloadSkinMap.clear();
}

BEGIN_MESSAGE_MAP(CSkinWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
	ON_WM_NCPAINT()
	ON_MESSAGE(WM_SKIN_DOWNLOAD_FAILED, OnSkinDownLoadFailed)
END_MESSAGE_MAP()

// CSkinWnd message handlers

BOOL CSkinWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	miScrollPos = 0;

	CRect rc;	
	int lnCount = (int)( m_colBtnList.size() );
	int lnLine  = lnCount/ROW_COUNT+ (lnCount%ROW_COUNT == 0? 0:1);
	BITMAP   lBitMap;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != lnCount) && (NULL != (*m_colBtnList_itor)->GetBitmapEx())  )
	{
		CBitmap *bmp = (*m_colBtnList_itor)->GetBitmapEx();
		bmp->GetBitmap( &lBitMap );
		int lnBmpHight = lBitMap.bmHeight;
		int lnCy = lnLine* (lnBmpHight + 2*TOP_SPACE+FONT_HEIGHT) + TOP_SPACE;
		if( lnCy> cs.cy )
		{
			mbIsHasScroll = true;
			m_nMaxViewPos = lnCy;
		}
	}		

	return CFrameWnd::PreCreateWindow(cs);
}

void CSkinWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	/*RECT lrect;
	::GetClientRect( this->m_hWnd, &lrect);
	gBitMapOperate.FillRectangle(dc.m_hDC, lrect,  RGB(0,255,255)  , true ) ;*/

	int nSaveDC = dc.SaveDC();

	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(rc,&CBrush(RGB(255,255,255)));

	dc.SelectObject(&m_font);			
	dc.SetBkMode(TRANSPARENT);		

	m_colBtnList_itor = m_colBtnList.begin() ;
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		CRect rectBtn;
		(*m_colBtnList_itor)->GetWindowRect(rectBtn);
		ScreenToClient(rectBtn);
		rectBtn.top = rectBtn.bottom;
		rectBtn.bottom += FONT_HEIGHT;

		dc.DrawText( (*m_colBtnList_itor)->GetText(),rectBtn,DT_CENTER |DT_NOPREFIX);
		
		m_colBtnList_itor++;
	}
	dc.RestoreDC(nSaveDC);
}
void CSkinWnd::UpdateScrollBar()
{
	mpVerticleScrollbar.UpdateScrollBar();
	mpHorizontalScrollbar.UpdateScrollBar();
}
void CSkinWnd::UpdateScrollBarPosition()
{
	mpVerticleScrollbar.SetPosition();
	mpHorizontalScrollbar.SetPosition();
}
void CSkinWnd::UpdateControlsEnable(BOOL abEnable)
{
	if (abEnable)
	{
		if (mpVerticleScrollbar)
		{
			mpVerticleScrollbar.EnableWindow(TRUE);
			mpVerticleScrollbar.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (mpVerticleScrollbar)
		{
			mpVerticleScrollbar.EnableWindow(FALSE);
			mpVerticleScrollbar.ShowWindow(SW_HIDE);
		}
	}
}
void CSkinWnd::InitScrollBar()
{
	miScrollPos = 0;

	CRect rc;
	this->GetClientRect( &rc );
	int lnCount = (int)( m_colBtnList.size() );
	int lnLine  = lnCount/ROW_COUNT+ (lnCount%ROW_COUNT == 0? 0:1);
	BITMAP   lBitMap;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != lnCount) && (NULL != (*m_colBtnList_itor)->GetBitmapEx())  )
	{
		CBitmap *bmp = (*m_colBtnList_itor)->GetBitmapEx();
		bmp->GetBitmap( &lBitMap );
		int lnBmpHight = lBitMap.bmHeight;
		int lnCy = lnLine* (lnBmpHight + 2*TOP_SPACE + FONT_HEIGHT ) + TOP_SPACE;
		if( lnCy> rc.Height() )
		{
			mbIsHasScroll = true;
			m_nMaxViewPos = lnCy;
		}
	}

	GetWindowRect(mrcOriginalRect);
	mrcOriginalRect.bottom = mrcOriginalRect.top + m_nMaxViewPos;


	if(mbIsHasScroll)
	{
		CWnd* lpParent = GetParent();

		//运行时建立自定义滚动条
		mpVerticleScrollbar.Create(  NULL, WS_CHILD | SS_LEFT | SS_NOTIFY | WS_VISIBLE | WS_GROUP, CRect(0 ,0, 0, 0),   lpParent);
		mpHorizontalScrollbar.Create(NULL, WS_CHILD | SS_LEFT | SS_NOTIFY | WS_VISIBLE | WS_GROUP, CRect(0 ,0, 0, 0),   lpParent);

		//设置滚动条方向
		mpVerticleScrollbar.SetScrollOrientation(esoVertical);
		mpHorizontalScrollbar.SetScrollOrientation(esoHorizontal);
		//设置滚动条控制控件
		mpVerticleScrollbar.SetControlWnd(this);
		mpHorizontalScrollbar.SetControlWnd(this);
		//设置滚动条位置
		UpdateScrollBarPosition();
		if (IsWindowEnabled())
			UpdateControlsEnable(TRUE);
		else
			UpdateControlsEnable(FALSE);
		UpdateScrollBar();
	}
}
void CSkinWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int liDelta;
	int liMaxPos = mrcOriginalRect.Height()- miHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (miScrollPos >= liMaxPos)
			return;

		liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		break;

	case SB_LINEUP:
		if (miScrollPos <= 0)
			return;
		liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		break;
	case SB_PAGEDOWN:
		if (miScrollPos >= liMaxPos)
			return;

		liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		break;
	case SB_THUMBTRACK:
		if (miScrollPos >= liMaxPos)
		{
			//moveToPos(IDC_ULUSERINFO_STATIC1);
		}		
		liDelta = nPos;
	case SB_THUMBPOSITION:
		liDelta = (int)nPos - miScrollPos;
		break;

	case SB_PAGEUP:
		if (miScrollPos <= 0)
			return;
		liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		break;
	case WM_MOUSEWHEEL:

		if( 1==nPos)
		{
			if (miScrollPos <= 0) return;
			liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		}

		if(-1 ==nPos)
		{
			if (miScrollPos >= liMaxPos) return;
			liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		}
		break;
	default:
		return;
	}
	miScrollPos += liDelta;
	::SendMessage(mpVerticleScrollbar.m_hWnd,WM_PDLG_MOUSEWHEEL_MSG,SB_VERT,miScrollPos);
	ScrollWindow(0,-liDelta);

	CFrameWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSkinWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if(mbIsHasScroll)
	{
		CFrameWnd::OnSize(nType, cx, cy);

		miHeight = cy;
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL; 
		si.nMin = 0;
		si.nMax = mrcOriginalRect.Height();
		si.nPage = cy+1;
		si.nPos = 0;
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
}

BOOL CSkinWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int liUpOrDown;
	liUpOrDown = (zDelta >= 0) ?  1:-1;
	OnVScroll(WM_MOUSEWHEEL,liUpOrDown,NULL);
	return true;
	//return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}
void CSkinWnd::InsertSmallSkin(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin)
{
	if (0 == astrName.length() ) return ;
	CSkinButton* lpBtnSkin = new CSkinButton();
	lpBtnSkin->SetText( CString(astrName.c_str() ) );
	lpBtnSkin->SetBottonType(lpBtnSkin->SKIN_BOTTON );
	lpBtnSkin->SetBitmap( apBmp);
	lpBtnSkin->SetSkinPath(strPath);	
	m_colBtnList.push_back(lpBtnSkin);


	if( !bHasSkin )
	{
		pair<CString, CSkinButton*> apair;
		apair.first = astrName.c_str();
		apair.second = lpBtnSkin;
		m_noDownloadSkinMap.insert( apair );
	}
}
void CSkinWnd::DownloadSkinFinish( CString strSkinName )
{
	if(strSkinName.GetLength() == 0 )  return;
	map<CString, CSkinButton*>::iterator itor;
	itor = m_noDownloadSkinMap.find( strSkinName );
	if( itor == m_noDownloadSkinMap.end() ) return;
	(*itor).second->EnableWindow( TRUE );
	m_noDownloadSkinMap.erase( itor );
	
}
int CSkinWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	//ModifyStyleEx(WS_BORDER,0,0);
	GetWindowRect(mrcOriginalRect);
	mrcOriginalRect.bottom = mrcOriginalRect.top + m_nMaxViewPos;

	CRect lrectClient;
	GetClientRect(&lrectClient);

	UINT lnBmpWidth = 0;
	UINT lnBmpHight = 0;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != (int)( m_colBtnList.size())  ) && (NULL != (*m_colBtnList_itor)->GetBitmapEx()) ) 
	{
		BITMAP   lBitMap;
		(*m_colBtnList_itor)->GetBitmapEx()->GetBitmap( &lBitMap );
		lnBmpWidth = lBitMap.bmWidth+12;
		lnBmpHight = lBitMap.bmHeight;

		int lnIndex = 0;

		string strSkinName;
		AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX,strSkinName);

		string strSkinPath = AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
		size_t idx = strSkinPath.find_last_of('\\');
		if( idx != string::npos )
		{
			strSkinPath = strSkinPath.substr(0,idx);
			strSkinPath = strSkinPath + "\\" + STR_DEFAULT_SKIN;
		}			

		while( m_colBtnList_itor != m_colBtnList.end() )
		{
			(*m_colBtnList_itor)->Create
				(
				NULL,
				WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,
				CRect(
				lrectClient.left + LIFT_SPACE + (lnIndex%ROW_COUNT)*(lnBmpWidth), 
				lrectClient.top + TOP_SPACE + (int)(lnIndex/ROW_COUNT)*(lnBmpHight+TOP_SPACE+FONT_HEIGHT),
				lrectClient.left + LIFT_SPACE+lnBmpWidth + (lnIndex%ROW_COUNT)*(lnBmpWidth)+4, 
				lrectClient.top + TOP_SPACE + TOP_SPACE + lnBmpHight+(int)(lnIndex/ROW_COUNT)*(lnBmpHight+TOP_SPACE+FONT_HEIGHT)
				),
				this,
				BOTTON_BEGAIN_ID+lnIndex
				);
			(*m_colBtnList_itor)->SetListener(this);
			(*m_colBtnList_itor)->GetBitmapEx()->SetBitmapDimension(lBitMap.bmWidth,lBitMap.bmHeight);
			(*m_colBtnList_itor)->SetPngSkin(CString(strSkinPath.c_str()) + CString("\\png\\") + "yes.png");
			(*m_colBtnList_itor)->ShowWindow(SW_SHOW);


			if(  AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject == "" &&  strSkinName == string((*m_colBtnList_itor)->GetText())  )
			{
				(*m_colBtnList_itor)->SetSelected( true );
			}

			lnIndex++;
			m_colBtnList_itor++;
		}
	}
	m_colBtnList_itor = m_colBtnList.begin();
	return 0;
}
void CSkinWnd::OnDestroy()
{
	CFrameWnd::OnDestroy();
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->DestroyWindow();
		m_colBtnList_itor++;
	}
}

void CSkinWnd::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CFrameWnd::OnNcPaint() for painting messages
}
void CSkinWnd::OnCliekButton(CButton* pBotton)
{	
	if( pBotton->m_hWnd)
	{
		char szBuf[128];
		sprintf(szBuf,"皮肤-%s",((CSkinButton*)pBotton)->GetSkinPath());

		CSkinButton* pcolButton = (CSkinButton*)pBotton;
		CString path = pcolButton->GetSkinPath();
		//int test = pcolButton->GetSkinBitMapIndex();
		if( path != "" )
		{
			if( NULL != mp_EventListener)
			{
				this->mp_EventListener->OnSendChangeSkin( pcolButton->GetSkinPath() ) ;
			}
		}		
	}
}
void CSkinWnd::EnableSkinBtn( BOOL enable )
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->EnableWindow(enable);
		m_colBtnList_itor++;
	}
	m_colBtnList_itor = m_colBtnList.begin();
}
void CSkinWnd::SetTipText( CString astrName,CString strTip)
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		CString nameText = (*m_colBtnList_itor)->GetText();
		if( nameText == astrName )
		{
			(*m_colBtnList_itor)->SetToolTipText( strTip );
		}
		m_colBtnList_itor++;
	}
	m_colBtnList_itor = m_colBtnList.begin();
}
void CSkinWnd::SetUseTip( bool bUseTip)
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->SetUseTip(bUseTip);
		m_colBtnList_itor++;
	}
	m_colBtnList_itor = m_colBtnList.begin();
}
LRESULT CSkinWnd::OnSkinDownLoadFailed(WPARAM wParam, LPARAM lParam)
{
	MessageBox("下载皮肤失败!", "提示", MB_OK);
	return 0;
}


void CSkinWnd::ShowWindow(BOOL bShow)
{
	if( bShow )
	{
		::ShowWindow(m_hWnd,SW_SHOW);
		if( mbIsHasScroll )
		{
			::ShowWindow(mpVerticleScrollbar.m_hWnd,SW_SHOW);
			::ShowWindow(mpHorizontalScrollbar.m_hWnd,SW_SHOW);
		}
	}
	else
	{
		::ShowWindow(m_hWnd,SW_HIDE);
		if( mbIsHasScroll )
		{
			::ShowWindow(mpVerticleScrollbar.m_hWnd,SW_HIDE);
			::ShowWindow(mpHorizontalScrollbar.m_hWnd,SW_HIDE);
		}
	}
}

void CSkinWnd::OnChangeSubject()
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		if( (*m_colBtnList_itor)->GetSelected() == 1 )
		{
			(*m_colBtnList_itor)->SetSelected( false );
			(*m_colBtnList_itor)->Invalidate( TRUE );
			break;
		}
		m_colBtnList_itor ++ ;
	}
	m_colBtnList_itor = m_colBtnList.end();	
}

void CSkinWnd::SkinChanged(const string& strNewSkin)
{
	string strSkinName;
	YL_FileInfo::GetFileName( strNewSkin,strSkinName);

	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		if( (*m_colBtnList_itor)->GetSelected() == 1 )
		{
			(*m_colBtnList_itor)->SetSelected( false );
			(*m_colBtnList_itor)->Invalidate( TRUE );
		}

		CSkinButton* pBtn = (*m_colBtnList_itor);
		if( pBtn->GetText() == strSkinName.c_str() )
		{
			pBtn->SetSelected(true);
		}
		m_colBtnList_itor ++ ;
	}
	m_colBtnList_itor = m_colBtnList.end();
	

	::SendMessage( this->GetParent()->GetSafeHwnd(), MSG_CHANGE_DEF_BTN_COLOR,AfxGetUIManager()->UIGetLayoutMgr()->GetSkinColor(),0 );
}