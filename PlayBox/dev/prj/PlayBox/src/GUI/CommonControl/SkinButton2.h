#pragma once

// zhou peng 2009.4 
// zhp21@163.com


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
//
class CSkinButton2 : public CButton
{
	DECLARE_DYNAMIC(CSkinButton2)

public:
	enum BTN_DRAW_TYPE { NO_FRAME=0, TOP_ARC=1, BOTTON_ARC=2, LEFT_ARC=4,RIGHT_ARC=8,NO_FRAME_SELECT=16 };
	enum BTN_DRAW_HUE  { DRAW_NORMAL =0, DRAW_LIGHT=1, DRAW_GRAY=2 };

	BTN_DRAW_TYPE m_enmuDrawType;
	BTN_DRAW_HUE  m_enumDrawHue;
	BOOL          m_bHandMouse;
	CSkinButton2();
	virtual ~CSkinButton2();

	BOOL SetSkin(CString str,BOOL bDisable = FALSE);
	void SetBkGround(CDC* pDC);
	void SetWindowsText(CString str);
	void GetWindowsText(CString& str);
	void SetCheck(int nCheck);
	int  GetCheck()const;
	CSize GetSkinSize();
		
	void SetMouseResponseRect(CRect rc){m_rcResponse = rc;m_bUseResponseRect = true;};
	void SendMouseMessageToParent(UINT msg, UINT nFlags, CPoint point);

	void SetFrameColor1( BYTE a, BYTE r, BYTE g, BYTE b);
	void SetFrameColor2( BYTE a, BYTE r, BYTE g, BYTE b);
	void SetFrameBrush(  BYTE a, BYTE r, BYTE g, BYTE b);
	void SetTextWidth(UINT width){ m_nWidth = width;}

	void SetToolTipText(CString sText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	void DeleteToolTip();

	void SetBKColor(BYTE a, BYTE r, BYTE g, BYTE b);
	COLORREF GetBKColor()const;

protected:
	void	InitToolTip();
	
private:
		CRect			m_rcResponse;
		BOOL			m_bUseResponseRect;
		CToolTipCtrl*	m_pToolTip;
		CString			m_tooltext;
		CGdiPlusBitmap* m_pPngImage;
		BOOL         m_bMouseOnButton;
		BOOL         m_bSelected;
		BOOL         m_bMouseDown;
		CDC			 m_dcBk;	

		ColorMatrix  m_LightMat;
		ColorMatrix  m_GrayMat;

		CString      m_strText;
		UINT         m_nWidth;

		Gdiplus::Color m_colBK;     
	
		Gdiplus::Color m_colFrame1;
		Gdiplus::Color m_colFrame2;
		Gdiplus::Color m_colBrush1;
		Gdiplus::Color m_colBrush2;
		Gdiplus::Font*  mp_font;
		BOOL		 m_bDisable;
		void    DrawImage(Gdiplus::Graphics& gdi,BTN_DRAW_HUE hue=DRAW_NORMAL);
		void    DrawFrame(Gdiplus::Graphics& gdi,CRect& rect);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg LRESULT OnMouseHover(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	virtual void PreSubclassWindow();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


