#ifndef _C_DATA_MANAGER_H
#define _C_DATA_MANAGER_H

#include "../module/TabMan/TabBarData.h"
#include "../module/httpDown/LocalGameData.h"
#include "../module/TopPanel/CPanelChange_Data.h"

#include "IDataManager.h"
#include "windows.h"
#include <map>
using std::map;

#define  GLOBAL_PANELCHANGEDATA		((IPanelChange*)AfxGetDataManager2()->GetDataObject(ID_DATA_PANELCHANGE))
#define	 GLOBAL_TABBARDATA			((ITabBarData*)AfxGetDataManager2()->GetDataObject(ID_DATA_TABBAR))
#define	 GLOBAL_LOCALGAME			((ILocalGameData*)AfxGetDataManager2()->GetDataObject(ID_DATA_LOCALGAME))
#define	 GLOBAL_ADDATA				((IAdData*)AfxGetDataManager2()->GetDataObject(ID_DATA_AD))

//数据管理类
class CDataManager: public IDataManager
{
public:
	CDataManager();
	~CDataManager();

public:

	//由系统调用，通知数据管理类程序启动
	void	DataManAppStart();

	//由系统调用，通知数据管理类程序退出
	void	DataManAppExit();	

	//其他类通过此接口获得数据类对象
	IData*	GetDataObject( const DWORD& guidDataObject );

	IData*	GetExportDataObject(const DWORD& guidDataObject);

	//数据对象通过该函数将自己添加到数据管理类管理的接口中
	void	AddDataObject( const DWORD& guidDataObject,IData* pData );

	//数据类将自己从数据管理类管理的接口中移除
	void	RemoveDataObject( const DWORD& guidDataObject );
private:

	IData*	_GetMapObject(const DWORD& guidDataObject);

	map<DWORD,IData*>			m_mapDataObject;

	CRITICAL_SECTION			m_csData;

};


#endif