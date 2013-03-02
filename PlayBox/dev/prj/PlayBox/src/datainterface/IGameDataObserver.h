#ifndef _I_GAMEDATA_OBSERVER_H
#define _I_GAMEDATA_OBSERVER_H

#include "IMessageManager.h"

class IGameDataObserver : public virtual IMessageObserver
{
public:
	virtual void IGameData_Changed(){};
};

#endif