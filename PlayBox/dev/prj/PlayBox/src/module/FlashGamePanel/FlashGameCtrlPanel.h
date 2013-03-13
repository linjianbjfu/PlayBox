#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CxSkinButton;

class CFlashGameCtrlPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameCtrlPanelWnd)
public:
	CFlashGameCtrlPanelWnd();
	virtual ~CFlashGameCtrlPanelWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnClickedReplay();
	afx_msg void	OnClickedPause();
	afx_msg void	OnClickedFullScreen();
	afx_msg void	OnClickedExitFullScreen();
	afx_msg void	OnClickedMute();
	afx_msg void	OnClickedUnMute();	
	afx_msg void	OnClickedCut();

private:
	CxSkinButton*		m_pBtnRePlay;
	CxSkinButton*		m_pBtnMute;
	CxSkinButton*		m_pBtnUnMute;
	CxSkinButton*		m_pBtnToFullScreen;
	CxSkinButton*		m_pBtnExitFullScreen;
	CxSkinButton*		m_pBtnPause;
};