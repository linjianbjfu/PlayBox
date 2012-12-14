#pragma once

#ifndef MSG_BTN_MOUSEDOWN
#define MSG_BTN_MOUSEDOWN (1000+9)
#endif

class IEventListener;

class CButtonDraw
{
public:

	
	CButtonDraw();
	virtual ~CButtonDraw();
	enum { LEFT_X_COL_SPACE = 7, /*颜色块在BUTTON左上角X轴的间距 */
		   LEFT_Y_COL_SPACE = 3, /*颜色块在BUTTON左上角Y轴的间距 */
		   LEFT_X_TEXT_SPACE  = 3, /*text块在BUTTON左上角x轴的间距 */
		   LEFT_Y_TEXT_SPACE = 20   /*text块在BUTTON左上角Y轴的间距 */
    	};
	enum { LEFT_X_COL_NOTEXT_SPACE = 3,
		   LEFT_Y_COL_NOTEXT_SPACE = 3
	     };
	enum {COL_HIGHT = 18,/*颜色快高度*/
		 TEXT_HIGHT = 15, /*文本高度*/
		 TEXT_WIDTH = 27, /*文本宽度*/
	}; 

public:
	void DrawButton(CWnd* apWnd, LPDRAWITEMSTRUCT lpDrawItemStruct,BOOL m_bottonType,BOOL abMouseOnButton, 
	 CString &astrTitle,COLORREF acolBegain,COLORREF acolEnd,CBitmap* aBitMap);
protected:
	/*void DrawSkinBorderEnter();
	void DrawSkinBorderNormal();
	void DrawSkinBorderFocused();
	void DrawColorBorderEnter();
	void DrawColorBorderNormal();
	void DrawColorBorderFocused();*/

	void DrawSkinEnter (HDC &aHDC, RECT &aRect,CString &astr,CBitmap* aBitMap);
	void DrawSkinNormal(HDC &aHDC, RECT &aRect,CString &astr,CBitmap* aBitMap);
	void DrawSkinFocused();
	void DrawColorEnter(HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd);
	void DrawColorNormal(HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd);
	void DrawColorFocused();

	void DrawColorNoTextEnter(HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd);
	void DrawColorNoTextNormal(HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd);
	void DrawColorNoTextFocused(){;}

	void DrawDefaultButtonNormal( HDC &aHDC, RECT &aRect,CString &astr );
	void DrawDefaultButtonEnter ( HDC &aHDC, RECT &aRect,CString &astr );

	void DrawExitButtonNormal( HDC &aHDC, RECT &aRect );
	void DrawExitButtonEnter(  HDC &aHDC, RECT &aRect );

	Gdiplus::Color m_colBK; 
	Gdiplus::Color m_colShadow; 
	Gdiplus::Color m_colShadow2; 
	Gdiplus::Color m_colShadow3; 

	
	void DrawBmpShade(CDC* pDc, CRect& rcBmp);
};

class CSkinButton : public CButton
{
	DECLARE_DYNAMIC(CSkinButton)

public:
	class CGdiPlusBitmap
{
public:
	Gdiplus::Bitmap* m_pBitmap;

	

public:
	CGdiPlusBitmap()							{ m_pBitmap = NULL; }
	CGdiPlusBitmap(LPCWSTR pFile)				{ m_pBitmap = NULL; Load(pFile); }
	virtual ~CGdiPlusBitmap()					{ Empty(); }

	void Empty()								{ delete m_pBitmap; m_pBitmap = NULL; }

	bool Load(LPCWSTR pFile)
	{
		
		Empty();
		m_pBitmap = Gdiplus::Bitmap::FromFile(pFile);
		return m_pBitmap->GetLastStatus() == Gdiplus::Ok;
	}

	operator Gdiplus::Bitmap*() const			{ return m_pBitmap; }
};
	CSkinButton();
	virtual ~CSkinButton();
	enum        {SKIN_BOTTON = 0, COLLOR_BOTTON, DEFAULT_BOTTON,/*默认颜色按扭*/ CLOSE_BOTTON,COLOR_NOTEXT_BOTTON };

	enum{ ERASE_BK, NOERASE_BK };
	bool m_bEraseBK;
	void SetErase(bool bErase){m_bEraseBK =bErase;}
	CButtonDraw m_ButtonDraw;
	friend class CButtonDraw;
public:
	void SetBitmap( CBitmap* apBitmap){m_BitMap = apBitmap;}
	void SetSkinPath(string strPath){ m_strSkinPath = strPath; }
	void SetColor( COLORREF acolBegain = 0,COLORREF acolEnd = 0 ){m_colBegain = acolBegain,m_colEnd= acolEnd;}
	void SetBottonType(UINT aBtnType){ m_bottonType = aBtnType; } 
	void SetText(CString astr){  m_string = astr;}
	CString GetText()const { return m_string; }
	COLORREF GetCorlor()const { return m_colBegain;}
	CBitmap* GetBitmapEx()const { return m_BitMap;}
	CString GetSkinPath()const { return m_strSkinPath.c_str(); }
	void SetListener( IEventListener* apListtener ){ m_pbtnEventListerner = apListtener;}	
	void SetSelected(bool bSelect){m_bSelected = bSelect;}
	bool GetSelected()const { return m_bSelected; }

	void SetToolTipText(CString s);
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

	BOOL SetPngSkin(CString str);
	virtual BOOL SetPngHoverSkin(CString str){return TRUE;}
	virtual BOOL SetPngFrame(CString str){return TRUE;}
	virtual BOOL IsClickPng(){return FALSE;}
	virtual void SetClickPng(BOOL bclick){}
	void DrawImage(Gdiplus::Graphics& gdi);

	CBitmap*    m_BitMap;

	CToolTipCtrl m_tooltip;
	CString m_strToolTip;
	float		hue;
	float		sat;
	float		light;

	
	
	bool		m_bSelected;
	bool        m_bshowTip; // 是否显示提示
	void SetUseTip(bool bshowTip);

	COLORREF	m_outColor;
 protected:
	BOOL        mbMouseOnButton;
	string      m_strSkinPath;
	CString     m_string;
	COLORREF    m_colBegain;
	COLORREF    m_colEnd;
	UINT        m_bottonType;
		
	CGdiPlusBitmap* m_pPngImage;
	
public:
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg LRESULT OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg LRESULT OnMouseHover(WPARAM   wParam,   LPARAM   lParam) ;
	DECLARE_MESSAGE_MAP()

	private:
	IEventListener* m_pbtnEventListerner; 
	afx_msg void OnDestroy();
};

