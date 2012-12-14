#pragma once

#include "IMessageManager.h"
#include "IDownDataDef.h"

class IDownDataDownObserver : public IMessageObserver
{
public:
	virtual void IDownDataOb_DownNotify(DownloadNotifyCode dnc,		//����֪ͨ��
										const RidVector& rv) = 0;	//����֪ͨ����
};

