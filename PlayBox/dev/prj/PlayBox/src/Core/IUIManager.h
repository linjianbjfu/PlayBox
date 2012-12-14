#ifndef _I_UI_MANAGER_H
#define _I_UI_MANAGER_H

#include "../LayoutMgr/ILayoutMgr.h"
//#include "../LayoutMgr/ISkinMgr.h"
#include "../LayoutMgr/ISkinMgr.h"
//#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
class IChangeColorMgr;
class ISkinMgr;
//���������
class IUIManager
{

public:
	virtual	~IUIManager(){};

public:

	//��ϵͳ���ã�֪ͨ����������������
	virtual bool		UIManAppStart() = 0;

	//��ϵͳ���ã�֪ͨ�������������˳�
	virtual void		UIManAppExit() = 0;		

	virtual bool		UICanExit()	   = 0;

	virtual int			UIDlgSize()	   = 0;

	virtual void		UIAddDialog(DWORD dwPtr) = 0;

	virtual void		UIRemoveDialog(DWORD dwPtr) = 0;

	virtual void		UIAddDialog(CDialog* pDialog) = 0;

	virtual void		UIRemoveDialog(CDialog* pDialog) = 0;

	//��ò��ֹ�����
	virtual ILayoutMgr*	UIGetLayoutMgr() = 0;

	//���Ƥ���������
	virtual ISkinMgr*	UIGetSkinMgr()	 = 0;

	virtual IChangeColorMgr*	UIGetChangeColorMgr() = 0;

	//�����ڵ���OnPaint;
	virtual void		UIOnPaint( CDC* pDC )	= 0;

	virtual void		UIOnMove(int xPos,int yPos ) = 0;

	virtual void		CreateUIWindow(const char* pszWindowName ) = 0;
};


#endif