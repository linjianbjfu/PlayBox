#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class CDibBitmap;

class CMyTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CMyTreeCtrl)

protected:

	CImageList		m_ImageList;
	vector<CBitmap*> m_vecBitmap;

	map<string,int>	m_mapBitmap2Index;

	COLORREF		m_colorSelectItemBK;
	COLORREF		m_colorSelectItemText;
	COLORREF		m_colorNormalItemText;
	CFont			m_treeFont;

	CDibBitmap*		m_pBitmapScrollVert;
	CDibBitmap*		m_pBitmapScrollHorz;
	bool			m_bInit;
	bool			m_bTrackMouse;
	HTREEITEM		m_iHoverItem;

public:
	CMyTreeCtrl( CDibBitmap* = NULL ,CDibBitmap* = NULL );
	virtual ~CMyTreeCtrl();

	HTREEITEM	InsertItem( const string& strText,const string& strImageName,HTREEITEM hParent,HTREEITEM hAfter = TVI_LAST);
	void		SetSelectItemBKColor(COLORREF color);
	void		SetSelectItemTextColor(COLORREF color);
	void		SetNormalItemTextColor(COLORREF color);
	void		SetBackColor(COLORREF color);
	void		SetScrollBitmap( CDibBitmap* pBitVert,CDibBitmap* pBitHorz );
	
	void		Init();
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void	OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnDestroy();
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
public:
	virtual void	ClickTreeItem( HTREEITEM ){};
	virtual void	RClickTreeItem( HTREEITEM ){};
	virtual void	DClickTreeItem( HTREEITEM ){};
	virtual void	ClearData();
};


