#ifndef _I_SKIN_UPDATE_OBSERVER_H
#define _I_SKIN_UPDATE_OBSERVER_H

#include "IMessageManager.h"

class ISkinUpdateObserver :
	public IMessageObserver
{
public:
	virtual void	ISkinUpdate_Success(){};
};
#endif