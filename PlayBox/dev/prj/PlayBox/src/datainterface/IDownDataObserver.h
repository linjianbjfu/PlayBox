#pragma once

#include "IMessageManager.h"
#include "IDownDataDef.h"

class IDownDataDownObserver : public IMessageObserver
{
public:
	virtual void IDownDataOb_DownNotify(DownloadNotifyCode dnc,		//下载通知码
										const RidVector& rv) = 0;	//下载通知参数
};

