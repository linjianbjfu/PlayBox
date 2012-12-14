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

//���ݹ�����
class CDataManager: public IDataManager
{
public:
	CDataManager();
	~CDataManager();

public:

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

	map<DWORD,IData*>			m_mapDataObject;

	CRITICAL_SECTION			m_csData;

};


#endif