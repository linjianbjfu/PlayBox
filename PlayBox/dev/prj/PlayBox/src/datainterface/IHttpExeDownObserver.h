#ifndef _HTTP_EXE_DOWN_OBSERVER_H
#define _HTTP_EXE_DOWN_OBSERVER_H

#include "IMessageManager.h"
#include "YL_HTTPDownFile.h"

class IHttpExeDownObserver : public IMessageObserver
{
public:
	virtual void HttpExeDownOb_DownStart( string& strID ) = 0;
	virtual void HttpExeDownOb_DownFinish( string& strID) = 0;
	virtual void HttpExeDownOb_DownQuickPlay (string &strID) = 0;
	virtual void HttpExeDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r ) = 0;
	virtual void HttpExeDownOb_DownDecompress (string &strID) = 0;
	virtual void HttpExeDownOb_DownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed, bool bQuickPlay = false) = 0;
	//dPercent»°÷µ∑∂Œß[0,1]
};

#endif