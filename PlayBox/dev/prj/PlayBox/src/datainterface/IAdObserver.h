#ifndef _I_Ad_OBSERVER_H
#define _I_Ad_OBSERVER_H

#include "IMessageManager.h"

//QQ�����෢�͵��¼��ӿ�
class IAdObserver:	public virtual IMessageObserver
{

public:
	//qq���ݸ��½ӿ�
	virtual	void	AdOb_StartUpdata( ){};
	virtual	void	AdOb_UpdataSuc( ){};
	virtual	void	AdOb_UpdateFailed( ){};
};

#endif