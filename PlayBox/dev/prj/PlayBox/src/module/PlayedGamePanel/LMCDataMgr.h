#pragma once
#include "LMCStructs.h"

class CLocalMusicCoverList;
class CLMCDataMgr 
{
public:
	friend class CLocalMusicCoverList;
	typedef vector<LMC_ItemInfo>	vLMCItem;
	typedef vLMCItem::iterator		iterLMCItem;
public:
	CLMCDataMgr();
	~CLMCDataMgr();
	
	int InsertItem(CDibBitmap* pImg, CString strItemName, CString strdetail, string strGID, string strSrvID, string strAddTime, int nGameType, BOOL blSel=FALSE);
	int InsertItem(CString strImgPath,CString strItemName, CString strdetail, string strGID, string strSrvID, string strAddTime, int nGameType, BOOL blSel=FALSE);
	void ClearData();
	BOOL RemoveItem(int iItem);
	void UpDataItem();
public:
	vLMCItem m_vItem;
};

class CLMCDataOper
{
public:
	CLMCDataOper(int delItem):m_delItem(delItem){}

	~CLMCDataOper(){}

	BOOL operator()(LMC_ItemInfo LMItem)
	{
		return m_delItem==LMItem.iIndex;
	}
private:
	int m_delItem;
};