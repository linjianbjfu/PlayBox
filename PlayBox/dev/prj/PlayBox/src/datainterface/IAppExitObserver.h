#ifndef _I_APP_EXIT_OBSERVER_H
#define _I_APP_EXIT_OBSERVER_H

#include "IMessageManager.h"

class IAppExitObserver:public virtual IMessageObserver
{
public:
	virtual void	IAppExit(){};	
};

#endif