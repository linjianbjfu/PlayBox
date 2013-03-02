#include "stdafx.h"
#include "LMCDataMgr.h"
#include <algorithm>

CLMCDataMgr::CLMCDataMgr()
{

}

CLMCDataMgr::~CLMCDataMgr()
{

}

int CLMCDataMgr::InsertItem(CDibBitmap* pImg,
							CString strItemName,
							CString strdetail, 
							string strGID, 
							string strAddTime,
							int nGameType,
							BOOL blSel)
{
	LMC_ItemInfo lmci;
	lmci.strGID = strGID;
	lmci.iIndex = m_vItem.size();
	lmci.blSel = blSel;
	lmci.pImg = pImg;
	lmci.strItemName = strItemName;
	lmci.strItemdDetailinfo = strdetail;
	lmci.nGameType = nGameType;
	lmci.strAddTime = strAddTime;
	m_vItem.push_back(lmci);
	return lmci.iIndex;
}

int CLMCDataMgr::InsertItem(CString strImgPath,
							CString strItemName,
							CString strdetail,
							string strGID,
							string strAddTime,
							int nGameType,
							BOOL blSel/* =FALSE */)
{
	LMC_ItemInfo lmci;
	lmci.strGID = strGID;
	lmci.iIndex = m_vItem.size();
	lmci.strImgPath = strImgPath.GetBuffer();
	lmci.strItemName = strItemName;
	lmci.strItemdDetailinfo = strdetail;
	lmci.blSel = blSel;
	lmci.nGameType = nGameType;
	lmci.strAddTime = strAddTime;
	m_vItem.push_back(lmci);
	return lmci.iIndex;
}

void CLMCDataMgr::ClearData()
{
	m_vItem.clear();
}

BOOL CLMCDataMgr::RemoveItem(int iItem)
{
	if (iItem>m_vItem.size()) return FALSE;
	m_vItem.erase(remove_if(m_vItem.begin(),m_vItem.end(),CLMCDataOper(iItem)),m_vItem.end());	
}

void CLMCDataMgr::UpDataItem()
{
	for (int i=0;i<m_vItem.size();i++)
		m_vItem[i].iIndex = i;
}
