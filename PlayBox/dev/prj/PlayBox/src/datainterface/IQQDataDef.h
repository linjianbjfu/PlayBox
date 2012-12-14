#ifndef _I_QQ_DATA_DEF_H
#define _I_QQ_DATA_DEF_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "../Global/GlobalVariable.h"

struct ITEM_DATA
{
	ITEM_DATA():strName(""),
				iCount(0),
				strIconName(""),
				strProvider(""),
				bIsNew(false),
				iItemIndex(0),
				strListID(""),
				strCat("")
	{
	}
	~ITEM_DATA()
	{
		Clear();
	}
	string			strName;
	string			strIconName;
	string			strListID;
	TAB_ITEM		ti;
	string			strProvider;
	bool			bIsNew;
	int				iCount;
	int				iItemIndex;
	int				iCategoryIndex;
	int				iPanelIndex;
	string			strCat;

	ITEM_DATA& operator=(const ITEM_DATA& itemData )
	{
		iCount			= itemData.iCount;
		strName			= itemData.strName;
		strIconName		= itemData.strIconName;
		strProvider		= itemData.strProvider;
		ti				= itemData.ti;
		bIsNew			= itemData.bIsNew;
		iItemIndex		= itemData.iItemIndex;
		iCategoryIndex	= itemData.iCategoryIndex;
		iPanelIndex		= itemData.iPanelIndex;
		strListID		= itemData.strListID;
		strCat			= itemData.strCat;		
		return *this;
	}

	void	Clear()
	{
		strName			= "";
		strIconName		= "";
		strProvider		= "";
		ti.eumType		= TAB_HOME;
		ti.strName.clear();
		ti.strParam.clear();
		bIsNew			= false;
		iItemIndex		= 0;
		iCategoryIndex	= 0;
		iPanelIndex		= 0;
		iCount			= 0;
		strListID		= "";
		strCat			= "";
	}
};

struct PANEL_TREE
{
	PANEL_TREE():strCategory(""),
				 iCount(0)
	{}
	~PANEL_TREE()
	{
		Clear();
	}

	string					strCategory;	
	int						iCount;
	vector<ITEM_DATA*>		vecPItemData;

	PANEL_TREE&	operator=(const PANEL_TREE& panelTree)
	{
		strCategory = panelTree.strCategory;
		iCount		= panelTree.iCount;
		for( size_t idx =0;idx < panelTree.vecPItemData.size();idx++ )
		{
			ITEM_DATA* itemData = new ITEM_DATA;
			(*itemData) = (*panelTree.vecPItemData[idx]);
			vecPItemData.push_back( itemData );
		}
		return *this;
	}

	void	Clear()
	{
		for( size_t idx = 0;idx < vecPItemData.size();idx++ )
		{
			vecPItemData[idx]->Clear();
			delete vecPItemData[idx];
		}
		vecPItemData.clear();
		strCategory = "";
		iCount		= 0;
	}
};

struct PANEL_DATA
{
	PANEL_DATA():strName(""),
				 strIconName(""),
				 iCount(0)
	{
	}
	~PANEL_DATA()
	{
		Clear();
	}

	string					strName;	
	string					strIconName;
	int						iCount;
	vector<PANEL_TREE*>		vecpPanelTree;

	PANEL_DATA& operator=(const PANEL_DATA& panelData)
	{
		strName			= panelData.strName;
		strIconName		= panelData.strIconName;
		iCount			= panelData.iCount;

		for( size_t idx =0;idx <  panelData.vecpPanelTree.size();idx++ )
		{
			PANEL_TREE* panelTree = new PANEL_TREE;
			(*panelTree) = (*panelData.vecpPanelTree[idx]);
			vecpPanelTree.push_back( panelTree );
		}
		return *this;
	}

	void Clear()
	{
		for( size_t idx = 0;idx < vecpPanelTree.size();idx++ )
		{
			vecpPanelTree[idx]->Clear();
			delete vecpPanelTree[idx];
		}
		vecpPanelTree.clear();
		strName		= "";
		strIconName	= "";
	}

	void GetAllLeafItem(vector<ITEM_DATA*> & vecItemData)
	{
		for( size_t idx1 = 0; idx1 < vecpPanelTree.size(); idx1 ++)
		{
			PANEL_TREE * paneTree = vecpPanelTree[idx1];

			for( size_t idx2 = 0; idx2 < paneTree->vecPItemData.size(); idx2 ++)
			{
				vecItemData.push_back(paneTree->vecPItemData[idx2]);
			}
		}
	}
};

struct PANEL_DATAS
{
	vector<PANEL_DATA*>		vecpPanelData;

	~PANEL_DATAS()
	{
		Clear();
	}

	PANEL_DATAS& operator=(const PANEL_DATAS& panelDatas)
	{
		for( size_t idx =0;idx < panelDatas.vecpPanelData.size();idx++ )
		{
			PANEL_DATA* panelData = new PANEL_DATA;
			(*panelData) = (*panelDatas.vecpPanelData[idx]);
			vecpPanelData.push_back( panelData );
		}
		return* this;
	}

	void	Clear()
	{
		for( size_t idx = 0;idx < vecpPanelData.size();idx++ )
		{
			vecpPanelData[idx]->Clear();
			delete vecpPanelData[idx];
		}
		vecpPanelData.clear();
	}
};

//QQ面板中选中项的属性
struct QQDATA_ITEM
{
	QQDATA_ITEM():strQQDataCategory(""),
				  strSubCategoryName(""),
				  strItemName("")
	{}

	string			strQQDataCategory;
	string			strSubCategoryName;
	string			strItemName;
};

#endif
