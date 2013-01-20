#ifndef _I_DATA_MANAGER_H
#define _I_DATA_MANAGER_H
#include "./src/core/AfxGlobals.h"

class IData;

//数据管理类
class IDataManager
{
public:
	virtual ~IDataManager(){};
	//由系统调用，通知数据管理类程序启动
	virtual void	DataManAppStart() = 0;

	//由系统调用，通知数据管理类程序退出
	virtual void	DataManAppExit() = 0;	

	//其他类通过此接口获得数据类对象
	virtual IData*	GetDataObject( const DWORD& guidDataObject ) = 0;

	//数据对象通过该函数将自己添加到数据管理类管理的接口中
	virtual void	AddDataObject( const DWORD& guidDataObject,IData* pData ) = 0;

	//数据类将自己从数据管理类管理的接口中移除
	virtual void	RemoveDataObject( const DWORD& guidDataObject ) = 0;
};

#endif