#ifndef _I_QQ_DATA_H
#define _I_QQ_DATA_H

#include "IData.h"
#include "IQQDataDef.h"

//QQ������
class IQQData : 
	public IData
{

public:	
	//���PanelData���ֵ���Ϣ����������Ҫ���PANEL_TREE������
	virtual void	IQQData_GetAllPanelNames( OUT PANEL_DATAS& PanelDatas ) = 0;

	//���PanelTree����
	virtual void	IQQData_GetPanelData( IN const string& strPanelName,OUT PANEL_DATA** PanelData ) = 0;

	//�ı�item ��old;
	virtual void	IQQData_ChangeItem2Old( IN int iItemIndex ) = 0;

	//���һ�����͸����б�
	virtual void	IQQData_GetItemData( IN int iItemIndex,ITEM_DATA& itemData) = 0;

	virtual void	IQQData_GetPanelName( IN int iItemIndex,string& strPanelName) = 0;

	virtual void	IQQData_GetCategoryName( IN int iItemIndex,string& strCategoryName) = 0;

	virtual void	IQQData_UpdatePanelData() = 0;

	virtual BOOL	IQQData_UpdateSuc()		  = 0;
};

#endif