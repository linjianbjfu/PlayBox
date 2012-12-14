#ifndef ZHP_XSKINCOMBOX_KOOWO_H
#define ZHP_XSKINCOMBOX_KOOWO_H

#include <vector>
#include "xSkinButton.h"


#define XSKINCOMBOBOX_CLASSNAME    _T("MFCxSkinComboBoxCtrl")

#define DEFAULT_COMBOBOX_SIZE_WIDTH		100
#define DEFAULT_COMBOBOX_SIZE_HEIGHT	20

#define XSKINCOMBOBOX_EDIT			10001
#define XSKINCOMBOBOX_BUTTON		10002
#define XSKINCOMBOBOX_HISTORYWND	10003
#define XSKINCOMBOBOX_HISTORY_LIST	10004

#define MSG_LIST_CLICKED			WM_USER+101
#define MSG_EDIT_CLICKED			WM_USER+102

#define MSG_COMBOBOX_SELECTE_CHANGE	WM_USER+4678

class COMBOBOXSKIN
{
public:
	COMBOBOXSKIN();
	CDibBitmap *pBmpButtonNormal;
	CDibBitmap *pBmpButtonOver;
	CDibBitmap *pBmpButtonDown;
	CDibBitmap *pBmpButtonDisable;
	COLORREF colorEdge;
	COLORREF colorEditBkg;
	COLORREF colorListBkg;
	COLORREF colorEditText;
	COLORREF colorListText;
	COLORREF colorListSelect;
	int		iFontSize;
	CDibBitmap* pBmpScrollVert;
	CDibBitmap* pBmpScrollHorz;
};


class CComboBoxEdit:public CEdit
{
public:
	CComboBoxEdit();

	void SetSkin(COLORREF &colorBkg,COLORREF &colorText);
	
	void SetMsgWnd(CWnd *pMsgWnd);

private:
	CWnd *m_pMsgWnd;

	COLORREF m_colorBkg;

	COLORREF m_colorText;


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
};


class CComboBoxHistoryList:public CListCtrl
{
	DECLARE_DYNAMIC(CComboBoxHistoryList)
public:
	CComboBoxHistoryList();	

	virtual ~CComboBoxHistoryList();	

	void SetSkin(COLORREF &colorBkg,COLORREF &colorText,COLORREF &colorSelect,CDibBitmap* pBitmapVert=NULL,CDibBitmap* pBitmapHorz=NULL);

	void SetMsgWnd(CWnd *pMsgWnd);

private:
	CWnd *m_pMsgWnd;

	COLORREF m_colorBkg;

	COLORREF m_colorText;

	COLORREF m_colorSelect;

	BOOL m_bInit;

	CDibBitmap*	m_pBitmapScrollVert;

	CDibBitmap*	m_pBitmapScrollHorz;

	void OnInit();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnDestroy();
	afx_msg void OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


class CComboBoxHistory:public CWnd
{
	friend class CxSkinComboBox;
public:
	CComboBoxHistory();

	void SetSkin(COLORREF &colorEdge,COLORREF &colorListBkg,COLORREF &colorListText,COLORREF &colorListSelect,CDibBitmap* pBitmapVert=NULL,CDibBitmap* pBitmapHorz=NULL);

	void RefreshData(vector<string> &vecString);

	void SetMsgWnd(CWnd *pMsgWnd);

	int GetMaxItemTextWidth();

	int GetItemHeight();
private:
	CWnd *m_pMsgWnd;

	CComboBoxHistoryList m_pList;

	COLORREF m_colorEdge;

	COLORREF m_colorBkg;

	CFont m_fontList;

	int	m_iMaxWidth;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


class CxSkinComboBox:public CWnd
{
public:
	CxSkinComboBox();

	virtual ~CxSkinComboBox();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetSkin(COMBOBOXSKIN &skin);

	int AddString(LPCTSTR lpszString);

	void SetCurSel(int iSel);

	int GetCurSel();
	
	void SetEditSel(int iSel,int iLength);

	int GetCount();

	void GetLBText(int iSel,CString& strText);

	void ResetContent();

	void SetWindowText(LPCTSTR lpszString);

	void GetWindowText(CString &strText);

	void SetMaxShowLines(int iNum);

private:
	BOOL RegisterWindowClass();

	COMBOBOXSKIN m_pSkin;

	CComboBoxEdit m_pEdit;

	CComboBoxHistory m_pHistoryWnd;

	CxSkinButton m_pHistoryBtn;

	CFont m_pEditFont;

	int m_iBtnWidth;

	int m_iBtnHeight;

	CBrush m_brushBkg;

	CBrush m_brushEditAddBkg;

	vector<string> m_vecString;

	int m_iCurSel;

	int m_iMaxShowLines;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnListClicked(WPARAM wParam,LPARAM lParama);
	afx_msg LRESULT OnEditClicked(WPARAM wParam,LPARAM lParama);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

#endif