#ifndef _I_QQ_ITEM_COUNT_CHANGE_OBSERVER_H
#define _I_QQ_ITEM_COUNT_CHANGE_OBSERVER_H

#include "IMessageManager.h"

class IQQItemCountChangeObserver : public IMessageObserver
{
public:
	virtual void IQQItemChangeOb_CountChange( unsigned int iNewCount ){};
};

#endif