#ifndef _I_LAYOUT_CHANGE_OBSERVER_H
#define _I_LAYOUT_CHANGE_OBSERVER_H

#include "IMessageManager.h"

class ILayoutChangeObserver:public IMessageObserver
{
public:
	virtual void	ILayoutChangeOb_LayerChanged(string paneName, string oldLayerName, string newLayerName ){};
	virtual void	ILayoutChangeOb_PaneChanged(string oldPaneName, string newPaneName){};
	virtual void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath){};
	virtual void	ILayoutChangeOb_ColorChanged(){};
	virtual void	ILayoutChangeOb_ColorChanging(){};
	virtual void	ILayoutChangeOb_UpdateLayout(HWND m_hWnd){};
	virtual void	ILayoutChangeOb_InitFinished(){};
	virtual void	ILayoutChangeOb_InitBegin(){};
};

#endif