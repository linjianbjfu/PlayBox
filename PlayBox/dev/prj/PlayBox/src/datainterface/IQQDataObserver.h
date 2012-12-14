#ifndef _I_QQ_DATA_OBSERVER_H
#define _I_QQ_DATA_OBSERVER_H

#include "IMessageManager.h"

//QQ数据类发送的事件接口
class IQQDataObserver:	public virtual IMessageObserver
{

public:
	//qq数据更新接口
	virtual	void	QQDataOb_StartUpdata( ){};
	virtual	void	QQDataOb_UpdataSuc( ){};
	virtual	void	QQDataOb_UpdateFailed( ){};
};


#endif