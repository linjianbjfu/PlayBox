#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/ILayoutChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "../../Global/GlobalVariable.h"

class CxSkinButton;
class MyWebBrowserWnd;

class WebGamePlayingWnd : public CBasicWnd,
						  public ILayoutChangeObserver
{
	DECLARE_DYNAMIC(WebGamePlayingWnd)
public:
	WebGamePlayingWnd();
	virtual ~WebGamePlayingWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnClickedFullScreen();
	afx_msg void	OnClickedGameRight();
private:
	WEB_GAME			m_webGame;

	CxSkinButton*		m_pBtnFullScreen;
	CxSkinButton*		m_pBtnGameRight;
	MyWebBrowserWnd*	m_pWndWebGame;
	void	UpdateAllWnd();

public:
	virtual void	ILayoutChangeOb_InitFinished();
	virtual void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);
	virtual void	ILayoutChangeOb_UpdateLayout(HWND hWnd);

	void	Init();
	void	LoadSkin();
	void	SetGameEntry( WEB_GAME wg );
};