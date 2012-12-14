// Skin.cpp : 实现文件
//

#include "stdafx.h"
#include "Skin.h"
// CSkin

CSkin::CSkin()
{
	m_bFromPackage = false;
}

CSkin::~CSkin()
{
	ClearMap();
}

int CSkin::GetBitmapWidth (CDibBitmap* pBitmap)
{	
	 return pBitmap->m_lpBMIH->biWidth;
}
int CSkin::GetBitmapHeight (CDibBitmap* pBitmap)
{
	return pBitmap->m_lpBMIH->biHeight;
}
CDibBitmap* CSkin::GetDibBmp(CString name)
{
	ITER_DIBBMP_MAP iter = m_dibbmpmap.find(name);
	if(iter == m_dibbmpmap.end())
		return NULL;
	else
		return iter->second;
	return NULL;
}

void CSkin::ClearMap()
{
	////清空map中已经载入的对象
	ITER_DIBBMP_MAP iter = m_dibbmpmap.begin();
	while(iter!=m_dibbmpmap.end())
	{
		((CDibBitmap*)iter->second)->Empty();
		delete iter->second;
		iter++;
	}
	m_dibbmpmap.clear();
}
//void CSkin::ChangeSkinColor( )
//{
//	ITER_DIBBMP_MAP iter = m_dibbmpmap.begin();
//	while(iter!=m_dibbmpmap.end())
//	{
//		if ((CDibBitmap*)(iter->second)->is_ChangeColor)
//		{
//			CString name = (iter->first);
//
//		CDibBitmap* d = (CDibBitmap*)(iter->second);
//		if(name == "LMDirPanelBtnByDirDown")
//			name = name;
//
//	   theApp.mp_skinMgr->ChangeBmp(DIBINFO(
//											  (CDibBitmap*)(iter->second)->m_lpvColorTable,
//			                                  (UINT)((CDibBitmap*)(iter->second)->m_lpBMIH->biWidth), 
//											  (UINT)((CDibBitmap*)(iter->second)->m_lpBMIH->biHeight),
//											  24,
//											  0),
//											  0,
//											  RGB(255,0,0)
//											  );
//		}
//
//	  iter++;			                         
//	}
//}