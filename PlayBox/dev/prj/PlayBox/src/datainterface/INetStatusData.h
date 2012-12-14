#ifndef _I_NETSTATUS_H
#define _I_NETSTATUS_H

#include "IData.h"


class INetStatus: public IData
{
public:
	virtual bool	IIsNetConnect()	= 0;
};


#endif