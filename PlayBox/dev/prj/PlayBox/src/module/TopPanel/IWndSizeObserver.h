#pragma once

class IWndSizeObserver
{
public:
	virtual void IWndSizeObserver_Max(){};
	virtual void IWndSizeObserver_Min(){};
	virtual void IWndSizeObserver_Restore(){};
	virtual void IWndSizeObserver_Close(){};
};
