#ifndef _I_QQ_DATA_OBSERVER_H
#define _I_QQ_DATA_OBSERVER_H

#include "IMessageManager.h"

//QQ�����෢�͵��¼��ӿ�
class IQQDataObserver:	public virtual IMessageObserver
{

public:
	//qq���ݸ��½ӿ�
	virtual	void	QQDataOb_StartUpdata( ){};
	virtual	void	QQDataOb_UpdataSuc( ){};
	virtual	void	QQDataOb_UpdateFailed( ){};
};


#endif