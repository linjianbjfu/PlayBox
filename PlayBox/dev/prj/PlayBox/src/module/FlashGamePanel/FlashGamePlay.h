#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "ILayoutChangeObserver.h"

class CxSkinButton;
class CShockwaveFlash;
class COneFlashGameControl;
class CFlashGameDownloadWnd;

class CFlashGamePlay : public CBasicWnd,
	public ILayoutChangeObserver
{
	DECLARE_DYNAMIC(CFlashGamePlay)
public:
	CFlashGamePlay(COneFlashGameControl* pCtrl);
	virtual ~CFlashGamePlay();
	void ShowButtons(bool bShow);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnIntroClicked();
	afx_msg void OnRecommandClicked();
	void ILayoutChangeOb_UpdateLayout(HWND hWnd);
private:
	COneFlashGameControl* m_pCtrl;
	void InitFlashParams();
	CxSkinButton* m_pBtnIntroCtrl;
	CxSkinButton* m_pBtnRecommandCtrl;
	CShockwaveFlash* m_pShockwave;
	CFlashGameDownloadWnd* m_pDownload;
};