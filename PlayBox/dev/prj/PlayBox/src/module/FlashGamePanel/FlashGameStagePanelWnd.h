#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGameStageLeft;
class MyWebBrowserWnd;
class COneFlashGameControl;
class CFlashGameIntro;

class CFlashGameStagePanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameStagePanelWnd)
public:
	CFlashGameStagePanelWnd(COneFlashGameControl* pCtrl);
	virtual ~CFlashGameStagePanelWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	COneFlashGameControl* m_pCtrl;
	CFlashGameStageLeft*  m_pStageLeft;
	CFlashGameIntro* m_pIntro;
};