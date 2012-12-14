#ifndef _I_PANEL_CHANGE_OBSERVER_H
#define _I_PANEL_CHANGE_OBSERVER_H

#include "IMessageManager.h"

class IPanelChangeObserver:public IMessageObserver
{
public:
	virtual void	IPanelChangeOb_WindowMax(){};
	virtual void	IPanelChangeOb_WindowMin(){};
	virtual void	IPanelChangeOb_WindowRestore(){};
	virtual void	IPanelChangeOb_WindowClose(){};
	virtual void	IPanelChangeOb_ToFullScreen( CWnd* pWnd ){};
	virtual void	IPanelChangeOb_ExitFullScreen( CWnd* pWnd ){};
};

#endif