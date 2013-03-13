#ifndef _C_DATA_MANAGER_H
#define _C_DATA_MANAGER_H

#include <windows.h>
#include <map>
#include "IDataManager.h"
#include "../module/TabMan/TabBarData.h"
#include "../module/httpDown/GameDataImp.h"
#include "../module/TopPanel/CPanelChange_Data.h"

#define  GLOBAL_PANELCHANGEDATA		((IPanelChange*)AfxGetDataManager2()->GetDataObject(ID_DATA_PANELCHANGE))
#define	 GLOBAL_TABBARDATA			((ITabBarData*)AfxGetDataManager2()->GetDataObject(ID_DATA_TABBAR))
#define	 GLOBAL_GAME				((IGameData*)AfxGetDataManager2()->GetDataObject(ID_DATA_GAME))

//���ݹ�����
class CDataManager: public IDataManager
{
public:
	CDataManager();
	~CDataManager();

	//��ϵͳ���ã�֪ͨ���ݹ������������
	void	DataManAppStart();
	//��ϵͳ���ã�֪ͨ���ݹ���������˳�
	void	DataManAppExit();	
	//������ͨ���˽ӿڻ�����������
	IData*	GetDataObject( const DWORD& guidDataObject );
	IData*	GetExportDataObject(const DWORD& guidDataObject);
	//���ݶ���ͨ���ú������Լ���ӵ����ݹ��������Ľӿ���
	void	AddDataObject( const DWORD& guidDataObject,IData* pData );
	//�����ཫ�Լ������ݹ��������Ľӿ����Ƴ�
	void	RemoveDataObject( const DWORD& guidDataObject );

private:
	IData*	_GetMapObject(const DWORD& guidDataObject);
	std::map<DWORD,IData*>			m_mapDataObject;
	CRITICAL_SECTION			m_csData;
};
#endif