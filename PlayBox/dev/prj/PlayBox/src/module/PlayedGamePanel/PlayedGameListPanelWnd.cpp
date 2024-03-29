#include "stdafx.h"
#include "PlayedGameListPanelWnd.h"
#include <algorithm>
#include <vector>
#include "YL_StringUtil.h"
#include "../../Core/CDataManager.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/ConfigAppDef.h"

IMPLEMENT_DYNAMIC(PlayedGameListPanelWnd, CLocalMusicCoverList)

PlayedGameListPanelWnd::PlayedGameListPanelWnd()
: m_iMouseDownItem(-1)
, m_szKey("-1")
, m_iMovePreItem(-1),
m_bTrackMouse(false) {}

PlayedGameListPanelWnd::~PlayedGameListPanelWnd() {}

BEGIN_MESSAGE_MAP(PlayedGameListPanelWnd, CLocalMusicCoverList)
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
				int nRet = ::MessageBox (AfxGetMainWindow ()->m_hWnd, "您确定要删除所选游戏吗？",
					DESCRIP_MAINEXE, MB_OKCANCEL);
				if (nRet == IDCANCEL)
					break;
			}
			std::vector<int> vAlbum;
			int iRmLast = 0,iCount = 0;
			vAlbum = GetSelectItem();
			std::vector<GameKey> vecGameKey;
			for (int i=0;i<vAlbum.size();i++)
			{
				GameKey gameKey;
				gameKey.nGameType = m_DataMgr.m_vItem[vAlbum[i]].nGameType;
				gameKey.strGameID = m_DataMgr.m_vItem[vAlbum[i]].strGID;
				gameKey.strSvrID = m_DataMgr.m_vItem[vAlbum[i]].strSvrID;
				vecGameKey.push_back(gameKey);
			}	
			GLOBAL_GAME->IGameData_DelGame(vecGameKey);
			//通过gamedata_observer回调来刷新界面，而不是用下面的代码
			//for (int i=0;i<vAlbum.size();i++)
			//{
			//	vector<LMC_ItemInfo>::iterator iterBegin = m_DataMgr.m_vItem.begin();
			//	iRmLast < vAlbum[i] ? m_DataMgr.m_vItem.erase(iterBegin + vAlbum[i] -iCount) : \
			//		m_DataMgr.m_vItem.erase(iterBegin + vAlbum[i] +iCount);
			//	iCount++;
			//	iRmLast = vAlbum[i];
			//}	
			//UpdateList();
			//OnMemoryDraw();
		}
		break;
	}
	return __super::OnCommand(wParam, lParam);
}

void PlayedGameListPanelWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);
	PlaySelectedItem();
}

void PlayedGameListPanelWnd::PlaySelectedItem()
{
	std::vector<int> vSel = GetSelectItem();
	if ( vSel.size() > 0 )
	{
		LMC_ItemInfo ii = m_DataMgr.m_vItem[vSel[0]];
		OneGame og;
		if (GLOBAL_GAME->IGameData_GetGameByID(ii.nGameType, ii.strGID, ii.strSvrID, og))
		{
			TAB_ITEM tItem;
			if (ii.nGameType & OneGame::FLASH_GAME)
			{
				tItem.enumType = TAB_FLASHGAME;
				tItem.strParam = string("method=flashgame") + BOX_DELIMITERS
					+ "id=" + og.strID +BOX_DELIMITERS 
					+ "name=" + og.strName + BOX_DELIMITERS
					+ "picurl=" + og.strPicSvrPath + BOX_DELIMITERS
					+ "swfurl=" + og.strGameSvrPath + BOX_DELIMITERS;
			}
			else if (ii.nGameType & OneGame::WEB_GAME)
			{
				tItem.enumType = TAB_WEBGAME;
				tItem.strParam = string("method=webgame") + BOX_DELIMITERS
					+ "id=" + og.strID + BOX_DELIMITERS
					+ "svrid=" + og.strSrvID + BOX_DELIMITERS
					+ "name=" + og.strName + BOX_DELIMITERS
					+ "picurl=" + og.strPicSvrPath + BOX_DELIMITERS;
			}
			else
				return;

			tItem.strTitle = string(ii.strItemName);
			GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
		}
	}
}

int PlayedGameListPanelWnd::ReGetData()
{
	m_DataMgr.ClearData();

	GameList lgl;
	GLOBAL_GAME->IGameData_GetGame(lgl, 0);
	for( GameList::iterator it1 = lgl.begin(); it1 != lgl.end(); it1++ )
	{
		std::string strLocalPicPath;
		it1->GetLocalPicPath(strLocalPicPath);
		InsertItem( strLocalPicPath.c_str(), 
			it1->strName.c_str(), 
			"", 
			it1->strID, 
			it1->strSrvID,
			"",
			it1->nGameType, 
			FALSE );
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
		std::vector<int> vItem = GetSelectItem();
		std::vector<GameKey> vecGameKey;
		for(int i = 0; i<vItem.size(); i++)
		{
			LMC_ItemInfo ii = GetItemInfoByIndex( vItem[i] );
			GameKey gameKey;
			gameKey.nGameType = ii.nGameType;
			gameKey.strGameID = ii.strGID;
			gameKey.strSvrID = ii.strSvrID;
			vecGameKey.push_back(gameKey);
		}	
		GLOBAL_GAME->IGameData_DelGame(vecGameKey);
		UpdateList();
		OnMemoryDraw();	
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
	gc.DrawImage(blNormal ? pImageNormal : pImageOver, rcImg);
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
		return __super::OnMouseMove(nFlags,point);

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
		int ihitarea = rcImg.PtInRect(point) ? 1 : 2;
		if (iItem!=ipreITem ||ihitarea != ipreon)
		{
			RedrawWindow(&rcPre);
			ipreon = ihitarea;
			DrawPlayBtn(rcImg, !rcImg.PtInRect(point));
		}
		rcPre.SetRect(rc.X,rc.Y,rc.X+rc.Width,rc.Y+rc.Height);
		m_iMovePreItem=iItem;
		ipreITem = iItem;
		//显示tooltips
		CString info;
		LMC_ItemInfo ii = m_DataMgr.m_vItem[iItem];

		OneGame og;
		memset (&og, 0, sizeof (OneGame));
		og.strName = ii.strItemName;
		info.Format("名称: %s", ii.strItemName );

		CPoint pt(0,0);
		GetCursorPos(&pt);
		if( m_tootip.m_lastPoint != pt )
			m_tootip.HideTooTips();

		m_tootip.ShowToolTips( info,pt);
	}
	if (!blHitTest)
	{
		if (!rcPre.IsRectNull())
			RedrawWindow(&rcPre);
		m_iMovePreItem=-1;
		ipreITem = -1;
	}
	::SetCursor(::LoadCursor(NULL, blHitTest ? IDC_HAND : IDC_ARROW));
	__super::OnMouseMove(nFlags, point);
}

void PlayedGameListPanelWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	PlaySelectedItem();
	//单击选中一个item
	//int iItem;
	//BOOL blHittest = HitTest(iItem,point);
	//if (blHittest)
	//{
	//	CRect rc=m_DataMgr.m_vItem[iItem].rc;
	//	rc.SetRect(rc.right-32,rc.bottom-32,rc.right-7,rc.bottom-7);
	//	if (rc.PtInRect(point))
	//		DrawPlayBtn(rc,FALSE);
	//}
}

void PlayedGameListPanelWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
	int iItem;
	BOOL blHittest = HitTest(iItem,point);
	if (blHittest)
	{
		CRect rc=m_DataMgr.m_vItem[iItem].rc;
		rc.SetRect(rc.right-32,rc.bottom-32,rc.right-7,rc.bottom-7);
		if (rc.PtInRect(point))
			DrawPlayBtn(rc,FALSE);
	}
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

void PlayedGameListPanelWnd::ReSetGameList(GameList arrGames)
{
	m_DataMgr.ClearData();
	GameList::iterator it = arrGames.begin();
	for (; it != arrGames.end(); it++)
	{
		std::string strLocalPicPath;
		it->GetLocalPicPath(strLocalPicPath);
		InsertItem(strLocalPicPath.c_str(),
				   it->strName.c_str(),
				   "",
				   it->strID,
				   it->strSrvID,
				   "",
				   it->nGameType,
				   FALSE);
	}
	UpdateList();
	OnMemoryDraw();
}