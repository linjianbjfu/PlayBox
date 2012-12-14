#ifndef _I_Ad_OBSERVER_H
#define _I_Ad_OBSERVER_H

#include "IMessageManager.h"

//QQ数据类发送的事件接口
class IAdObserver:	public virtual IMessageObserver
{

public:
	//qq数据更新接口
	virtual	void	AdOb_StartUpdata( ){};
	virtual	void	AdOb_UpdataSuc( ){};
	virtual	void	AdOb_UpdateFailed( ){};
};

#endif