#ifndef _C_PANELCHANGE_DATA_H
#define _C_PANELCHANGE_DATA_H

#include "../../DataInterface/IPanelChange.h"

class CPanelChange : public IPanelChange
{
public:
	static IData*	GetInstance();
	void	DataAppStart();
	void	DataAppExit();

	void IPanelChange_ToFullScreen( CWnd* pWnd );
	bool IPanelChange_IsFullScreen( );
	void IPanelChange_ExitFullScreen();
	bool IPanelChange_Max();
	bool IPanelChange_Min();
	bool IPanelChange_Restore();
	void IPanelChange_Close();
public:
	static	CPanelChange* m_pPanelChange;
	bool	m_bFullScreen;
	CWnd*	pWndFullScreen;
};

#endif