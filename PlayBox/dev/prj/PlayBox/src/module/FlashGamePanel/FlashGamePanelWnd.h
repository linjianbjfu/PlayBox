#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGameCtrlPanelWnd;
class CFlashGameStagePanelWnd;

class CFlashGamePanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGamePanelWnd)
public:
	CFlashGamePanelWnd();
	virtual ~CFlashGamePanelWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CFlashGameCtrlPanelWnd*   m_pGameCtrlPanelWnd;
	CFlashGameStagePanelWnd*  m_pGameStagePanelWnd;
};