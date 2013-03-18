#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "IMuteMsgObserver.h"

class CxSkinButton;
class COneFlashGameControl;

class CFlashGameCtrlPanelWnd : public CBasicWnd,
	public IMuteMsgObserver
{
	DECLARE_DYNAMIC(CFlashGameCtrlPanelWnd)
public:
	CFlashGameCtrlPanelWnd(COneFlashGameControl* pCtrl);
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
	void MuteMsg_Change(bool bMute);
	void UpdateBtnState();
	COneFlashGameControl* m_pCtrl;
	CxSkinButton*		m_pBtnRePlay;
	CxSkinButton*		m_pBtnMute;
	CxSkinButton*		m_pBtnUnMute;
	CxSkinButton*		m_pBtnToFullScreen;
	CxSkinButton*		m_pBtnExitFullScreen;
	CxSkinButton*		m_pBtnPause;

	bool m_bMute;
};