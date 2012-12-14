#pragma once
#include "resource.h"
#include <map>
#include "../../gui/CommonControl/xSkinButton.h"
#include "HostWnd.h"
using namespace std;
// CBasicMouseWnd
#define SHOWLIST	1
#define SHOWMV		2
class CMainHostWnd : public CHostWnd
{
private:
	bool	m_bShowMV;
	bool	m_bShowPlaylist;
	int		m_nControlWndHeight;
	int		m_isShowMVWhenClosePlaylist;
private:
	void	MoveContent();
	bool	m_bSepWndsAttached;
	int		m_nSepMainWndPos;
public:
	CMainHostWnd(CWnd* pParent = NULL);
	virtual ~CMainHostWnd();

	void	ChangeWatchMode();
	void	ChangePlaylistMode();


	bool	IsShowPlaylist(){return m_bShowPlaylist;};
	bool	IsShowMV(){return m_bShowMV;};
	void	SetTo1x();
	void	SetTo2x();
	bool	_needChangeCursor(const CPoint& Pt );

	void	MoveMvSize(int height , int width);
	void	SetMinSize();
	void	CalResizingByMVRatio(CRect& rcControlWnd, CRect& rcPlaylist);
	CWnd*	m_pControlWnd;
	CWnd*	m_pPlistWnd;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL	OnInitDialog();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnSize(UINT nType, int cx, int cy);

};

