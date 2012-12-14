#ifndef _I_UI_MANAGER_H
#define _I_UI_MANAGER_H

#include "../LayoutMgr/ILayoutMgr.h"
//#include "../LayoutMgr/ISkinMgr.h"
#include "../LayoutMgr/ISkinMgr.h"
//#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
class IChangeColorMgr;
class ISkinMgr;
//界面管理类
class IUIManager
{

public:
	virtual	~IUIManager(){};

public:

	//由系统调用，通知界面管理类程序启动
	virtual bool		UIManAppStart() = 0;

	//由系统调用，通知界面管理类程序退出
	virtual void		UIManAppExit() = 0;		

	virtual bool		UICanExit()	   = 0;

	virtual int			UIDlgSize()	   = 0;

	virtual void		UIAddDialog(DWORD dwPtr) = 0;

	virtual void		UIRemoveDialog(DWORD dwPtr) = 0;

	virtual void		UIAddDialog(CDialog* pDialog) = 0;

	virtual void		UIRemoveDialog(CDialog* pDialog) = 0;

	//获得布局管理器
	virtual ILayoutMgr*	UIGetLayoutMgr() = 0;

	//获得皮肤管理对象
	virtual ISkinMgr*	UIGetSkinMgr()	 = 0;

	virtual IChangeColorMgr*	UIGetChangeColorMgr() = 0;

	//主窗口调用OnPaint;
	virtual void		UIOnPaint( CDC* pDC )	= 0;

	virtual void		UIOnMove(int xPos,int yPos ) = 0;

	virtual void		CreateUIWindow(const char* pszWindowName ) = 0;
};


#endif