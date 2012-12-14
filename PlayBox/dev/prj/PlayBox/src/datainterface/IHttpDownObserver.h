#ifndef _HTTP_DOWN_OBSERVER_H
#define _HTTP_DOWN_OBSERVER_H

#include "IMessageManager.h"
#include "YL_HTTPDownFile.h"

class IHttpDownObserver : public IMessageObserver
{
public:
	virtual void HttpDownOb_DownStart( string& strID ) = 0;
	virtual void HttpDownOb_DownFinish( string& strID, string& strSwfPath ) = 0;
	virtual void HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r ) = 0;
	virtual void HttpDownOb_DownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed) = 0;
	//dPercent»°÷µ∑∂Œß[0,1]
};

#endif