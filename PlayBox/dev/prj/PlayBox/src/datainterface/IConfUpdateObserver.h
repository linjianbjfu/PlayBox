#ifndef _I_CONF_UPDATE_OBSERVER_H
#define _I_CONF_UPDATE_OBSERVER_H

#include "IMessageManager.h"

class IConfUpdateObserver : public virtual IMessageObserver
{
public:
	virtual void	IConfUpdate_Update(){};
	virtual void	IConfUpdate_ConfigFile(){};
	virtual void	IConfUpdate_CodecSucc(){};
};

#endif