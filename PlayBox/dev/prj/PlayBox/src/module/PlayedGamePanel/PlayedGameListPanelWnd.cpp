#include "stdafx.h"
#include "PlayedGameListPanelWnd.h"
#include <algorithm>
#include <set>
#include <vector>
#include "../../Core/CDataManager.h"
#include "../../../../CommonLib/prj/Log/KwLogSvr.h"
#include "YL_StringUtil.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/ConfigAppDef.h"

using std::set;
using std::vector;

IMPLEMENT_DYNAMIC(PlayedGameListPanelWnd, CLocalMusicCoverList)

PlayedGameListPanelWnd::PlayedGameListPanelWnd()
: m_iMouseDownItem(-1)
, m_szKey("-1")
, m_szKeyWord("")
, m_iMovePreItem(-1),
m_bTrackMouse(false)
{	
}

PlayedGameListPanelWnd::~PlayedGameListPanelWnd()
{
}

BEGIN_MESSAGE_MAP(PlayedGameListPanelWnd, CLocalMusicCoverList)
	ON_WM_SHOWWINDOW()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

void PlayedGameListPanelWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		// ReGetData();
	}
	__super::OnShowWindow(bShow, nStatus);
}

void PlayedGameListPanelWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	__super::OnRButtonUp(nFlags, point);

	m_pMenu = new CShowMenu(this,IDR_MENU_COVERLIST);
	if( !HitTest(m_iMouseDownItem,point) )
	{
        //禁用删除
		m_pMenu->DisableItem( ID_CL_DELFROMDISK );		
	}
	ClientToScreen(&point);
	m_pMenu->ShowMenu(point);
	delete m_pMenu;
	m_pMenu = NULL;
}

BOOL PlayedGameListPanelWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	static BOOL bPrompted = FALSE;

	WORD lowWord	= LOWORD( wParam );
	WORD hiWord		= HIWORD( wParam );
	switch( lowWord )
	{
	case ID_CL_LARGEICON: // 大图标
		SetImgState(LMCI_Big);
		break;
	case ID_CL_MIDICON: // 中等图标
		SetImgState(LMCI_Middle);
		break;
	case ID_CL_SMALLICON: // 小图标
		SetImgState(LMCI_Small);
		break;
	case ID_CL_DELFROMDISK: // 从磁盘删除
		{
			if (!bPrompted)
			{
				bPrompted = TRUE;
// 				int nRet = AfxMessageBox ("您确定要删除所选游戏吗？", MB_OKCANCEL);
				int nRet = ::MessageBox (AfxGetMainWindow ()->m_hWnd, "您确定要删除所选游戏吗？",
					"小宝贝游戏盒", MB_OKCANCEL);
				if (nRet == IDCANCEL)
					break;
			}
			vector<int> vAlbum;
			vAlbum = GetSelectItem();
			RidVector rv;
			for (int i=0;i<vAlbum.size();i++)
			{
				string strGID = m_DataMgr.m_vItem[vAlbum[i]].strGID;
				rv.push_back (strGID);
			}

			for (RidVector::iterator it = rv.begin (); it != rv.end (); it++)
			{
				GLOBAL_LOCALGAME->ILocalGameData_DelGame(*it);
			}

			rv.clear ();

			UpdateList();
			OnMemoryDraw();
		}
		break;
	default:
		break;
	}
	return __super::OnCommand(wParam, lParam);
}

void PlayedGameListPanelWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	vector<int> vSel = GetSelectItem();
	if ( vSel.size() > 0 )
	{
		LMC_ItemInfo ii = m_DataMgr.m_vItem[vSel[0]];

		if (ii.iGameType == OneLocalGame::TYPE_FLASH_GAME) // flash game
		{
			TAB_ITEM tItem;
			tItem.eumType = TAB_FLASHGAME;
			tItem.strName = string(ii.strItemName);
			tItem.strParam = string("method=playswfgame\n\n") + "id=" + ii.strGID + "\n"
				+ "name=" + tItem.strName + "\n";
			GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
		}
		else if (ii.iGameType == OneLocalGame::TYPE_WEB_GAME) // web game
		{
			TAB_ITEM tItem;
			tItem.eumType = TAB_WEBGAME;
			tItem.strName = string(ii.strItemName);
			tItem.strParam = string("method=webgame\n\n")
				+ "id=" + ii.strGID + "\n"
				+ "svrid=" + ii.strSvrID + "\n"
				+ "name=" + tItem.strName + "\n"
				+ "picurl=" + ii.strImgPath.GetBuffer(0) + "\n";
			ii.strImgPath.ReleaseBuffer();
			GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
		}

		//OnMemoryDraw ();
	}

	__super::OnLButtonDblClk(nFlags, point);
}

int PlayedGameListPanelWnd::ReGetData()
{
	m_DataMgr.ClearData();

	LocalGameList lgl;
	GLOBAL_LOCALGAME->ILocalGameData_GetAllGame( lgl );
	for( LocalGameList::iterator it1 = lgl.begin(); it1 != lgl.end(); it1++ )
	{
		CString strDetail;
		InsertItem( it1->strPicPath.c_str(), 
			it1->strName.c_str(), strDetail, it1->strID, it1->strSrvID, it1->nGameType, FALSE );
	}
	UpdateList();
	OnMemoryDraw();
	return 10;
}

int PlayedGameListPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_tootip.m_pClientWnd = this;
	m_tootip.CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",0,CRect(0,0,0,0),NULL,0);
	m_tootip.m_hParent = GetSafeHwnd();
	m_tootip.m_clrBK = RGB(255, 255, 225);	

	return 0;
}

void PlayedGameListPanelWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_DELETE)
	{
		vector<int> vItem = GetSelectItem();
		int i = 0;
		RidVector rv;

		for( ; i<vItem.size(); i++  )
		{
			LMC_ItemInfo ii = GetItemInfoByIndex( vItem[i] );
			rv.push_back (ii.strGID);
// 			GLOBAL_LOCALGAME->ILocalGameData_DelGame( ii.strGID );
		}

		for (RidVector::iterator it = rv.begin (); it != rv.end (); it++)
		{
			GLOBAL_LOCALGAME->ILocalGameData_DelGame(*it);
		}

		rv.clear ();

		UpdateList();
		OnMemoryDraw();

// 		if( i > 0 ) //如果有删除东西，就刷新
// 		{
// 			ReGetData();
// 		}		
	}
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void PlayedGameListPanelWnd::DrawPlayBtn(CRect rc,BOOL blNormal/* =TRUE */)
{
	Rect rcImg(rc.left, rc.top,rc.Width(),rc.Height());
	static BOOL blInit=TRUE;
	static Image* pImageNormal = NULL;
	static Image* pImageOver = NULL;
	if (blInit)
	{
		blInit=FALSE;
		TCHAR path_tmp[MAX_PATH]={0};
		::GetModuleFileName(NULL,path_tmp,MAX_PATH);
		CString szPlaybtnNormal(path_tmp);
		szPlaybtnNormal=szPlaybtnNormal.Mid(0,szPlaybtnNormal.ReverseFind('\\'));
		CString szPlaybtnOver = szPlaybtnNormal;
		szPlaybtnNormal+="\\CoverPlayNormal.png";
		szPlaybtnOver += "\\CoverPlayOver.png";
		USES_CONVERSION;
		Image img(A2W(szPlaybtnNormal.GetBuffer()),FALSE );
		pImageNormal=img.Clone();
		Image imgOver(A2W(szPlaybtnOver.GetBuffer()));
		pImageOver = imgOver.Clone();
	}
	Graphics gc(GetDC()->m_hDC);
	if (blNormal)
		gc.DrawImage(pImageNormal,rcImg);
	else
		gc.DrawImage(pImageOver,rcImg);
}

void PlayedGameListPanelWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bTrackMouse = TRUE;
	}

	if (m_blDrag||m_blTracker)
	{
		return __super::OnMouseMove(nFlags,point);
	}

	static CRect rcPre(0,0,0,0);	
	int iItem;	
	BOOL blHitTest=HitTest(iItem,point);
	static int ipreITem = -1;
	static int ipreon = -1;
	if (blHitTest)
	{
 		if (!rcPre.IsRectNull()&&ipreITem!=iItem)
 			RedrawWindow(&rcPre);

		Graphics gc(GetDC()->m_hDC);
		Rect rc(m_DataMgr.m_vItem[iItem].rc.right-32,m_DataMgr.m_vItem[iItem].rc.bottom-32,26,26);
		CRect rcImg(rc.X,rc.Y,rc.Width+rc.X,rc.Height+rc.Y);
		int ihitarea=-1;
		if (rcImg.PtInRect(point))
		{
			ihitarea = 1;
		}
		else
		{
			ihitarea = 2;
		}

		if (iItem!=ipreITem ||ihitarea!=ipreon)
		{
			if (rcImg.PtInRect(point) )
			{
				RedrawWindow(&rcPre);
				ipreon = 1;
				DrawPlayBtn(rcImg,FALSE);
			}
			else 
			{
				RedrawWindow(&rcPre);
				ipreon = 2; 
				DrawPlayBtn(rcImg,TRUE);
			}
		}

		rcPre.SetRect(rc.X,rc.Y,rc.X+rc.Width,rc.Y+rc.Height);
		m_iMovePreItem=iItem;
		ipreITem = iItem;
		//显示tooltips
		CString info;
		LMC_ItemInfo ii = m_DataMgr.m_vItem[iItem];

		OneLocalGame og;
		memset (&og, 0, sizeof (OneLocalGame));
		og.strName = ii.strItemName;

		if( GLOBAL_LOCALGAME->ILocalGameData_GetGameByID( ii.strGID, og ) )
		{
			info.Format("名称: %s$#游戏简介:%s", og.strName.c_str(), og.strIntro.c_str() );
		}else
		{
			info.Format("名称: %s", ii.strItemName );
		}
		//ii.strGID
		CPoint pt(0,0);
		GetCursorPos(&pt);
		if( m_tootip.m_lastPoint != pt )
		{
			m_tootip.HideTooTips();
		}
		m_tootip.ShowToolTips( info,pt);
	}
	if (!blHitTest)
	{
		if (!rcPre.IsRectNull())
			RedrawWindow(&rcPre);
		m_iMovePreItem=-1;
		ipreITem = -1;
	}
	__super::OnMouseMove(nFlags, point);
}


void PlayedGameListPanelWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	
	int iItem;
	BOOL blHittest = HitTest(iItem,point);
	if (blHittest)
	{
		CRect rc=m_DataMgr.m_vItem[iItem].rc;
		rc.SetRect(rc.right-32,rc.bottom-32,rc.right-7,rc.bottom-7);
		if (rc.PtInRect(point))
		{
			DrawPlayBtn(rc,FALSE);
			return ;
		}
	}
}

void PlayedGameListPanelWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	int iItem;
	BOOL blHittest = HitTest(iItem,point);
	if (blHittest)
	{
		CRect rc=m_DataMgr.m_vItem[iItem].rc;
		rc.SetRect(rc.right-32,rc.bottom-32,rc.right-7,rc.bottom-7);
		if (rc.PtInRect(point))
		{
			//CString szArtist = m_DataMgr.m_vItem[iItem].strItemName;
			//vector<Song> vSong;
			//CPData::GetInstance()->GetSongByArtist(szArtist.GetBuffer(),vSong);
			//for (int i=0;i<vSong.size();i++)
			//	CLMCommon::PlayMusicUseMbox(vSong[i].strPath.c_str());
			DrawPlayBtn(rc,FALSE);
			return ;
		}
	}
	__super::OnLButtonUp(nFlags, point);
}

void PlayedGameListPanelWnd::OnDestroy()
{
	m_tootip.DestroyWindow();
	CLocalMusicCoverList::OnDestroy();
}

LPARAM PlayedGameListPanelWnd::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bTrackMouse = FALSE;
	m_tootip.HideTooTips();
	return 0;
}

void PlayedGameListPanelWnd::Recycle()
{
	ShowWindow( SW_HIDE ); //这样才能让PlayedGameListPanelWnd中的OnShowWindow时获取数据
}

BOOL PlayedGameListPanelWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_tootip.HideTooTips();
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}


void PlayedGameListPanelWnd::ReSetGameList(LocalGameList arrGames)
{
	m_DataMgr.ClearData();
	
	LocalGameList::iterator it = arrGames.begin();
	for (; it != arrGames.end(); it++)
	{
		CString strDetail;
		InsertItem(it->strPicPath.c_str(),
				   it->strName.c_str(),
				   strDetail,
				   it->strID,
				   it->strSrvID,
				   it->nGameType,
				   FALSE );
	}
	UpdateList();
	OnMemoryDraw();
}

