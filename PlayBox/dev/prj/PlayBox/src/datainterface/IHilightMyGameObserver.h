#ifndef _HTTP_HILIGHT_MYGAME_OBSERVER_H
#define _HTTP_HILIGHT_MYGAME_OBSERVER_H

#include "IMessageManager.h"

class IHilightMyGameObserver : public IMessageObserver
{
public:
	virtual void HilightMyGameOb_Select ( string& strID ) = 0;
};

#endif