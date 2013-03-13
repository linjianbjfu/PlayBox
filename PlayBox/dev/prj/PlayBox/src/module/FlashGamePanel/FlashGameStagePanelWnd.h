#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGameStageLeftPanelWnd;
class MyWebBrowserWnd;

class CFlashGameStagePanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGamePanelWnd)
public:
	CFlashGameStagePanelWnd();
	virtual ~CFlashGameStagePanelWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CFlashGameStageLeftPanelWnd*   m_pStageLeft;
	MyWebBrowserWnd* m_pIntro;
};