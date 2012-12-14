#ifndef _I_PANEL_CHANGE_H
#define _I_PANEL_CHANGE_H

#include "IData.h"

class IPanelChange : public IData
{
public:
	virtual void IPanelChange_ToFullScreen( CWnd* pWnd )= 0;
	virtual void IPanelChange_ExitFullScreen()=0;
	virtual bool IPanelChange_IsFullScreen()= 0;
	virtual bool IPanelChange_Max()= 0;
	virtual bool IPanelChange_Min()= 0;
	virtual bool IPanelChange_Restore()= 0;
	virtual void IPanelChange_Close()=0;
};

#endif