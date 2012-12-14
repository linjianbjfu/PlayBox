#pragma once

#include "../../DataInterface/IAppExitObserver.h"
#include "../../DataInterface/IAdObserver.h"

class CFlashTips : public IAppExitObserver, public IAdObserver
{
public:
	CFlashTips(void);
	~CFlashTips(void);
public:
	void	AdOb_UpdataSuc();
	void	IAppExit();	
};
