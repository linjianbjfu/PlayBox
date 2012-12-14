#pragma once

#include "IWndSizeObserver.h"
#include <set>

class WndSizeControl
{
public:
	WndSizeControl(void);
	~WndSizeControl(void);
	//---------------------Singleton------------//
public:
	static WndSizeControl*	GetInstance();
	static void			DelInstance();
	static WndSizeControl*	m_pCtr;
	//---------------------Observer------------//
public:
	void AddObserver( IWndSizeObserver* pOb );
	void LeaveObserver( IWndSizeObserver* pOb );
private:
	set<IWndSizeObserver*>	m_ob;

public:
	void Max();
	void Min();
	void Restore();
	void Close();
};
