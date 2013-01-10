#pragma once
#include "LocalMusicCoverList.h"
#include "../../gui/util/ShowMenu.h"
#include "../../gui/util/WToolTips.h"
#include "../../gui/CommonControl/SkinButton2.h"
#include "../../datainterface/IDownDataDef.h"
#include "../../Core/CDataManager.h"
#include "../../datainterface/ILocalGameData.h"

class PlayedGameListPanelWnd : public CLocalMusicCoverList
{
	DECLARE_DYNAMIC(PlayedGameListPanelWnd)
public:
	PlayedGameListPanelWnd();
	virtual ~PlayedGameListPanelWnd();
private:
	CShowMenu*		m_pMenu;
	CCWToolTips		m_tootip;
	bool			m_bTrackMouse;
	int				m_iMouseDownItem;
	CSkinButton2*	m_playBtn;
	string			m_szKey;
	string			m_szKeyWord;
	int				m_iMovePreItem;
protected:
	int	 ReGetData();
	void DrawPlayBtn(CRect rc,BOOL blNormal=TRUE);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
	void	Recycle();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void	ReSetGameList(LocalGameList arrGames);
};