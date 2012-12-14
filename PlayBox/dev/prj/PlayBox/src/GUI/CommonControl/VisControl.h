#pragma once

#include "stdafx.h"
#include "../../DataInterface/ILayoutChangeObserver.h"
class CDibBitmap;

struct visDib{
	CDibBitmap *pDibBack;
	CDibBitmap *pDibBar;
};
class CVisControl: public CStatic,  public  ILayoutChangeObserver
{
private:

	int m_Top;
	int m_Left;
	int m_Width;
	int m_Height;

	HBITMAP	m_hBmp;
	std::string m_BmpName;
	visDib	m_visDib;
public:

	CVisControl(void);

	~CVisControl(void);

	void ILayoutChangeOb_ColorChanging();

	void SetConfig(int left, int top,int width, int height, const char* bmpName);

	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

	void LoadSkin();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

};
