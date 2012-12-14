// PassMouseWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "mainhostwnd.h"
#include "WindowsMover.h"
#include ".\hostwnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../LayoutMgr/SeperateMode/HostWndMgr.h"
// CPassMouseWnd
#define DEFAULT_WIDTH	320
#define DEFAULT_HEIGHT	342
#define PLAYLIST_MIN_HEIGHT	210

CMainHostWnd::CMainHostWnd(CWnd* pParent /*=NULL*/): CHostWnd(pParent)
{
	m_nControlWndHeight = 0;
	m_pControlWnd = ::AfxGetUIManager()->UIGetLayoutMgr()->GetWnd("ControlPanel");
	m_pPlistWnd = ::AfxGetUIManager()->UIGetLayoutMgr()->GetWnd("SepPlayList");
	
	CString strControlMode = ::AfxGetUIManager()->UIGetLayoutMgr()->GetWndPanelMode(m_pControlWnd);
	int mode = 1;
	//chaohua edit
	//AfxGetUserConfig()->GetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_SEPMODE_START_MODE,mode );
	
	if(mode & SHOWLIST)
		m_bShowPlaylist = true;
	else
		m_bShowPlaylist = false;

	if(strControlMode == "normal")
		m_bShowMV = false;
	else
		m_bShowMV = true;

}

CMainHostWnd::~CMainHostWnd()
{
	//CWnd* pWnd = CHostWndMgr::GetInstance()->GetHostWnd("sepSongLibrary");
	//CRect rc = this->GetWindowRect(&rc);

	//if(CWindowsMover::GetInstance()->IsTwoWindowsAttached(pWnd, this))
	//	m_bSepWndsAttached = true;
	//else
	//	m_bSepWndsAttached = false;

	//m_nSepMainWndPos = rc.left + (rc.top << 16);
	//AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_SEPWINDOWS_ATTACHED,m_bSepWndsAttached , true);
	//AfxGetUserConfig()->SetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_SEPWINDOW_DELTAPOS,m_nSepMainWndPos, true );

	if(m_pWnd != NULL)
		::SetParent(m_pWnd->m_hWnd,m_hParentWnd);
}


BEGIN_MESSAGE_MAP(CMainHostWnd, CHostWnd)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CMainHostWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//m_bShowMV = true;
	CWindowsMover::GetInstance()->AddWndindow(this, m_isMainWnd);

	//m_bShowPlaylist = true;
	m_pControlWnd->SetParent(this);
	m_pPlistWnd->SetParent(this);

	SetMinSize();
	return TRUE;
}

void CMainHostWnd::OnLButtonDown(UINT nFlags, CPoint point)
{	
	__super::OnLButtonDown(nFlags, point);

}

void CMainHostWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
}

void CMainHostWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove( nFlags,point );

}

void CMainHostWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx,cy);

	if( AfxGetUIManager()->UIGetLayoutMgr() != NULL )
	{
		DWORD count = ::GetTickCount();
		CDC* dc = GetDC();
		HRGN hrgn = AfxGetUIManager()->UIGetSkinMgr()->GetFrameRGN(cx, cy , dc);
		ReleaseDC(dc);
		::SetWindowRgn(m_hWnd, hrgn, true);
		AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(this->m_hWnd);
	}

	if(!m_bShowPlaylist)
	{
		CRect rc;
		GetClientRect(&rc);
		m_pControlWnd->MoveWindow(&rc);
	}
	else
		MoveContent();

	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CMainHostWnd::CalResizingByMVRatio(CRect& rcControlWnd, CRect& rcPlaylist)
{
	CRect rc = rcControlWnd;
	rc.DeflateRect(6, 42,6, 84);

	float w1 = rc.Width();
	float h1 = rc.Height();
	float w = rc.Width() - m_nDeltaWidth;
	float h = rc.Height() - m_nDeltaHeight;
	float calheight;

	calheight = w1 * 3/4;

	if(calheight + 126 < DEFAULT_HEIGHT)
		calheight = DEFAULT_HEIGHT - 126;

	if (
		(h > h1 && calheight < h && calheight > h1) ||
		(h < h1 && calheight < h1 && calheight > h)
		)
	{
		rcControlWnd.bottom = rcControlWnd.top + calheight + 126;
		rcPlaylist.top = rcControlWnd.bottom;
	}
	//高度变小，mv的高度符合比例了，缩小playlist
	else if(h1 < h && calheight >= h && rcPlaylist.Height() > PLAYLIST_MIN_HEIGHT)
	{
		rcControlWnd.bottom = rcControlWnd.bottom - m_nDeltaHeight;
		rcPlaylist.top = rcPlaylist.top - m_nDeltaHeight;
	}
	//高度变大，mv高度已经满足了，拉大playlist
	else if(h1 > h && calheight <= h)
	{
		rcControlWnd.bottom = rcControlWnd.bottom - m_nDeltaHeight;
		rcPlaylist.top = rcPlaylist.top - m_nDeltaHeight;
	}
}

void CMainHostWnd::MoveContent()
{
	CRect rc;
	GetClientRect(&rc);

	if( !m_bShowPlaylist)
	{
		m_pControlWnd->GetWindowRect(&rc);
		MoveWindow(&rc);
	}
	else
	{
		//if(m_bShowMV)
		//{
		//	CRect rc1, rc2;
		//	m_pControlWnd->GetWindowRect(&rc1);
		//	m_pPlistWnd->GetWindowRect(&rc2);
		//	CalResizingByMVRatio(rc1, rc2);
		//	this->ScreenToClient(&rc1);
		//	this->ScreenToClient(&rc2);
		//	m_pControlWnd->MoveWindow(&rc1);
		//	m_pPlistWnd->MoveWindow(&rc2);
		//}
		if(
			(( m_iDirection & DIRECTION_TOP) || ( m_iDirection & DIRECTION_BOTTOM) )
			&&  m_bShowMV )
		{
			CRect rcList;
			m_pPlistWnd->GetWindowRect(&rcList);
			int nControlWndHeight = rc.Height() - rcList.Height();

			if(nControlWndHeight < DEFAULT_HEIGHT)
			{
				//欣赏区域保持最小高度。playlist填充剩余区域。
				rcList.bottom = rcList.top + (rc.Height() - DEFAULT_HEIGHT);
				rc.bottom = rc.top + DEFAULT_HEIGHT;
			}
			else
				rc.bottom = rc.top + nControlWndHeight;

			CRect rcControl, rcPlaylist;
			rcControl = rc;
			//
			rc.top = rc.Height();
			rc.bottom = rc.top + rcList.Height();
			rcPlaylist = rc;
			CalResizingByMVRatio(rcControl,rcPlaylist);
			//play最小高度处理
			if(rcPlaylist.Height() < PLAYLIST_MIN_HEIGHT)
			{
				CRect rect;
				this->GetWindowRect(&rect);

				int bottom = (rcControl.bottom < rcPlaylist.bottom) ? rcPlaylist.bottom: rcControl.bottom;
				if(bottom > rect.Height())
					bottom = rect.Height();
				rcControl.bottom = bottom - PLAYLIST_MIN_HEIGHT;
				rcPlaylist.top = rcControl.bottom;
				rcPlaylist.bottom = bottom;
			}
			m_pControlWnd->MoveWindow(&rcControl);
			m_pPlistWnd->MoveWindow(&rcPlaylist);
		}
		else
		{
			CRect rcControl;
			m_pControlWnd->GetClientRect(&rcControl);

			if(!m_bShowMV && rcControl.Height() < 127)
				rcControl.bottom += 127;

			int nPlaylistHeight = rc.Height() - rcControl.Height();

			if(nPlaylistHeight < PLAYLIST_MIN_HEIGHT)
			{
				rcControl.bottom = rc.bottom - PLAYLIST_MIN_HEIGHT;
			}
			

			//if(nPlaylistHeight < 0)
			//	return;
			if(m_bShowMV && rcControl.Height() < DEFAULT_HEIGHT)
			{
				rcControl.bottom = rcControl.top + DEFAULT_HEIGHT;
			}
			rc.top = rcControl.Height();
			m_pPlistWnd->MoveWindow(rc);

			rcControl.right = rc.right;
			m_pControlWnd->MoveWindow(&rcControl);
		}
	}
}

void CMainHostWnd::ChangeWatchMode()
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd *pWnd = ::AfxGetUIManager()->UIGetLayoutMgr()->GetWnd("ControlPanel");
	bool bKeepMVSize = false;
	CString strCur = ::AfxGetUIManager()->UIGetLayoutMgr()->GetCurrentLayer("MultiNowPlaying");
	if(m_bShowMV)
	{
		m_pControlWnd->GetWindowRect(&rc);
		::AfxGetUIManager()->UIGetLayoutMgr()->SaveWndPanel("ControlPanel", "watchmode");
		CHostWndMgr::GetInstance()->ShowPopupWndsBeforeMainChange(false);
		::AfxGetUIManager()->UIGetLayoutMgr()->CreateControlPane( pWnd,"ControlPanel", "normal");
		::AfxGetUIManager()->UIGetLayoutMgr()->CreateBmpPane( pWnd,"ControlPanel","normal");
		//记录关闭mv之前mv的高度。
		m_nControlWndHeight = rc.Height();
	}
	else
	{
		CHostWndMgr::GetInstance()->ShowPopupWndsBeforeMainChange(true);
		::AfxGetUIManager()->UIGetLayoutMgr()->CreateControlPane( pWnd,"ControlPanel", "watchmode");
		::AfxGetUIManager()->UIGetLayoutMgr()->CreateBmpPane( pWnd,"ControlPanel","watchmode");

		//如果记录了mv之前的高度，则回复mv的高度。
		if(m_nControlWndHeight != 0)
		{
			MoveMvSize(m_nControlWndHeight, rc.Width());
			bKeepMVSize = true;
		}
	}
	//临时这样改
	::AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer("MultiNowPlaying", strCur);

	m_bShowMV = !m_bShowMV;
	SetMinSize();

	if(!bKeepMVSize)
		MoveContent();

	if(m_bShowMV)
		LogUserActMsg(STR_USER_CLICK, "BTN:SHOW_MV");
	else
		LogUserActMsg(STR_USER_CLICK, "BTN:HIDE_MV");

	if(m_bShowMV)
	{
		CHostWndMgr::GetInstance()->ShowPopupWndsAfterMainChange(true);
	}
	else
	{
		CHostWndMgr::GetInstance()->ShowPopupWndsAfterMainChange(false);
	}
//	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

}


void CMainHostWnd::ChangePlaylistMode()
{
	CRect rc;
	CRect rcControl;

	GetWindowRect(&rc);
	m_pControlWnd->GetWindowRect(&rcControl);
//	CWnd *pWnd = ::AfxGetUIManager()->UIGetLayoutMgr()->GetWnd("ControlPanel");

	m_bShowPlaylist = !m_bShowPlaylist;

	SetMinSize();
	CRect rcPlaylist;
	m_pPlistWnd->GetWindowRect(rcPlaylist);

	if(m_bShowPlaylist)
	{

		m_pPlistWnd->ShowWindow(SW_SHOW);
		rc.bottom += rcPlaylist.Height();

		if(!m_isShowMVWhenClosePlaylist && m_bShowMV)
			rc.bottom -= rcControl.Height() - 127;

		if(rc.Height() < m_nMinCy)
			rc.bottom = rc.top + m_nMinCy;


		MoveWindow(&rc);

	}
	else
	{
		m_pPlistWnd->ShowWindow(SW_HIDE);
		rc.bottom -= rcPlaylist.Height();
		MoveWindow(&rc);
		m_isShowMVWhenClosePlaylist = m_bShowMV;
	}

	//chaohua edit
	//if(m_bShowPlaylist)
	//	LogUserActMsg(STR_USER_CLICK, "BTN:SHOW_PLAYLIST");
	//else
	//	LogUserActMsg(STR_USER_CLICK, "BTN:HIDE_PLAYLIST");


	//if(m_bShowPlaylist)
	//	AfxGetUserConfig()->SetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_SEPMODE_START_MODE,SHOWLIST,true );
	//else
	//	AfxGetUserConfig()->SetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_SEPMODE_START_MODE,0,true );
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}


bool CMainHostWnd::_needChangeCursor(const CPoint& Pt )
{

	bool ret = __super::_needChangeCursor(Pt);
	if(ret == true && !m_bShowPlaylist && !m_bShowMV)
	{
		if(m_iDirection & DIRECTION_TOP)
			m_iDirection ^= DIRECTION_TOP;

		if(m_iDirection & DIRECTION_BOTTOM)
			m_iDirection ^= DIRECTION_BOTTOM;

		if(m_iDirection == 0)
		{
			m_bChangeCursor = true;
			return false;
		}

	}
	return ret;

}

void CMainHostWnd::SetTo1x()
{
	CRect rc, rcControl;
	GetWindowRect(&rc);

	if(!m_bShowPlaylist)
	{
		rc.right = rc.left + DEFAULT_WIDTH;
		rc.bottom = rc.top + DEFAULT_HEIGHT;
	}
	else
	{
		m_iDirection = DIRECTION_TOP;
		m_pControlWnd->GetWindowRect(&rcControl);
		int deltax = rcControl.Width() - DEFAULT_WIDTH;
		int deltay = rcControl.Height() - DEFAULT_HEIGHT;

		rc.right = rc.left + DEFAULT_WIDTH;
		rc.bottom = rc.bottom - deltay;
	}

	this->MoveWindow(&rc);

	CHostWndMgr::GetInstance()->PopupWndsChangeTo1x();
}

void CMainHostWnd::MoveMvSize(int height , int width)
{
	CRect rc, rcControl, rcPlaylist;
	GetWindowRect(&rc);

	if(!m_bShowPlaylist)
	{
		rc.right = rc.left + width;
		rc.bottom = rc.top + height;
	}
	else
	{
		m_pPlistWnd->GetWindowRect(&rcPlaylist);
		m_iDirection = DIRECTION_TOP;
		m_pControlWnd->GetWindowRect(&rcControl);
		int deltax = rcControl.Width() - width;
		//int deltay = rcControl.Height() - 629;
		rcPlaylist.top = rc.top + height;
		rcControl.bottom = rc.top + height;

		if(rcPlaylist.bottom - rcPlaylist.top < PLAYLIST_MIN_HEIGHT)
			rcPlaylist.bottom = rcPlaylist.top + PLAYLIST_MIN_HEIGHT;
		if(rc.bottom < rcPlaylist.bottom)
			rc.bottom = rcPlaylist.bottom;

		ScreenToClient(rcPlaylist);
		ScreenToClient(rcControl);
		m_pPlistWnd->MoveWindow(&rcPlaylist);
		m_pControlWnd->MoveWindow(&rcControl);

		rc.right = rc.left + width;
		//rc.bottom = rc.bottom - deltay;
	}

	this->MoveWindow(&rc);

}
void CMainHostWnd::SetTo2x()
{
	MoveMvSize(629 , 600);
	CHostWndMgr::GetInstance()->PopupWndsChangeTo2x();
}

void CMainHostWnd::SetMinSize()
{
	//return;
	m_nMinCx = DEFAULT_WIDTH;

	if(m_bShowPlaylist)
	{
		if(!m_bShowMV)
			m_nMinCy = 127*2 + PLAYLIST_MIN_HEIGHT;
		else
			m_nMinCy = DEFAULT_HEIGHT + PLAYLIST_MIN_HEIGHT;
	}
	else
	{
		if(!m_bShowMV)
			m_nMinCy = 127;
		else
			m_nMinCy = DEFAULT_HEIGHT;
	}
}