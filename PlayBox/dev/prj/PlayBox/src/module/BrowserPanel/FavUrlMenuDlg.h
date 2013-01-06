// FavUrlMenuDlg.h : 头文件
//

#pragma once

#define RGB_BORDER_DEFAULT			RGB(0x93, 0xCC, 0xFC)
#define RGB_TEXT_DEFAULT			RGB(0, 0, 0)
#define RGB_BACKGROUND_DEFAULT		RGB(0xFF, 0xFF, 0xFF)
#define RGB_CURSEL_DEFAULT			RGB(0xE5, 0xF6, 0xFD)

#define BORDER_WIDTH_DEFAULT		1
#define SPACING_HEIGHT_DEFAULT		7

#define IDC_ITEM_ID_BEGIN		2560
#define IDC_ITEM_ID_END			4095

#define ITEM_TYPE_URL			0
#define ITEM_TYPE_DIRECTORY		1
#define ITEM_TYPE_BTN_ADD		2
#define ITEM_TYPE_BTN_OPEN		3
#define ITEM_TYPE_BTN_REMOE		4
#define ITEM_TYPE_SEPARATOR		5

#define WM_MENU_DESTROYED		(WM_USER+119)

// WPARAM:item type LPARAM:url file path
#define WM_MENU_CICKED		(WM_USER+120)

typedef struct _ITME
{
	int type;
	CString strName;
	CString strPath;
}ITEM, *PITEM;

#include <afxtempl.h>

// CFavUrlMenuDlg 对话框
class CFavUrlMenuDlg : public CDialog
{
protected:
	CFont		m_font;
	COLORREF	m_rgbBorder;
	COLORREF	m_rgbText;
	COLORREF	m_rgbBackGnd;
	COLORREF	m_rgbCursel;

	int	m_nBorderWidth;
	int	m_nFontHeight;
	int	m_nSpacingHeigth;
	BOOL m_bAddFav;

	CFavUrlMenuDlg*	m_pChildDlg;
	int			m_nCurSel;

	CRect		m_rctFirsItem;
	//CSize		m_sizeItem;
	CString		m_strParentPath;
	CArray<ITEM, ITEM&>		m_arrItems;

	UINT	m_IDD;
	CWnd*	m_pParentWnd;
	HRGN	m_hRgn;

// 构造
public:
	CFavUrlMenuDlg(UINT uIDD, char*szParentPath=NULL, BOOL bAddFav=TRUE, CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	int GetUrls(CString strPath, CArray<ITEM, ITEM&> &arrItems);
	int IsPtInItem(CPoint pt);	// 判断鼠标在那个item里,不在任何一个item则返回-1



	void	LineAtoB(CDC *pDC, CPoint ptA, CPoint ptB);


// 实现
protected:

	afx_msg LRESULT OnChildClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickedItem(WPARAM wParam, LPARAM lPram);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:

	virtual void SetUIParam(LOGFONT font,
			unsigned uBorderWidth = BORDER_WIDTH_DEFAULT,
			unsigned uSpacingWidth = SPACING_HEIGHT_DEFAULT,
			COLORREF rgbText = RGB_TEXT_DEFAULT,
			COLORREF rgbBackGround = RGB_BACKGROUND_DEFAULT,
			COLORREF rgbCurSel = RGB_CURSEL_DEFAULT,
			COLORREF rgbBorder = RGB_BORDER_DEFAULT);

	virtual BOOL ShowMenu(CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
