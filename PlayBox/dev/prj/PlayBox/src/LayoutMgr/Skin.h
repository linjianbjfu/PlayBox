#pragma once
#include "../gui/Bitmap/DibBitmap.h"
#include "../util/MarkupArchive.h"
#include <map>
using std::map;

typedef map<CString,CDibBitmap*> DIBBMP_MAP;
typedef DIBBMP_MAP::iterator ITER_DIBBMP_MAP;

typedef map<CString  ,COLORREF> COLOR_MAP;
typedef COLOR_MAP::iterator ITER_COLOR_MAP;


#define BorderColor RGB(0x8E,0x95,0x9B)
class CSkin : public CObject
{
public:
	CSkin();
	virtual ~CSkin();
	
	CString m_strSkinPath;

	CMarkupArchive m_skinxml;
	DIBBMP_MAP m_dibbmpmap;	
	DIBBMP_MAP m_dibOrgBmpMap;	
	BOOL	m_bFromPackage;

	//¹«ÓÃ
	CDibBitmap* GetDibBmp(CString name);
	static int GetBitmapWidth (CDibBitmap* pBitmap);
	static int GetBitmapHeight (CDibBitmap* pBitmap);
	virtual int GetPanelHeight(CString panelname){return 0;}
	virtual int GetPanelWidth(CString panelname){return 0;}
	void CSkin::ClearMap();
//	void ChangeSkinColor();

	void PutDibBmp(CString name, CDibBitmap *pDib)
	{
		pDib->m_strName = name;
		m_dibbmpmap.insert(DIBBMP_MAP::value_type(name,pDib));
	}

	CDibBitmap * PutDibBmp(CString name, CString path)
	{
		CDibBitmap *pdib = new CDibBitmap();
		if(pdib->LoadFromFile(path + name + ".bmp"))
		{
			pdib->m_strName = name;
			m_dibbmpmap.insert(DIBBMP_MAP::value_type(name,pdib));
		}
		else{
			delete pdib;
			pdib = NULL;
		}
		return pdib;
	}

	HFONT CreateBoldFont(HFONT hFont = NULL, int Weight = FW_BOLD) 
	{ 
		if(hFont == NULL) 
			hFont =  (HFONT)::GetStockObject(DEFAULT_GUI_FONT); 

		HFONT hFontBold = NULL; 
		LOGFONT lf = { 0 }; 
		if(::GetObject(hFont, sizeof(LOGFONT), &lf) == sizeof(LOGFONT)) 
		{ 
			lf.lfWeight = Weight; 
			hFontBold = ::CreateFontIndirect(&lf); 
		} 
		else 
		{ 
		} 
		return hFontBold; 
	} 
};


