#pragma once
#include <vector>
using namespace std;

class CDibBitmap;
// CHeadCtrol
#ifndef MOUSE_EXCURSION
#define MOUSE_EXCURSION (3)
#endif  

#ifndef SORT_MSG
#define SORT_MSG (WM_USER+10000)
#endif 

#ifndef CHANGE_SCROOL_MSG
#define CHANGE_SCROOL_MSG (WM_USER+10001)
#endif 

#ifndef SCROOL_HEAD
#define SCROOL_HEAD (WM_USER+10002)
#endif 

class CHeadCtrol : public CWnd
{
	DECLARE_DYNAMIC(CHeadCtrol)

public:
	CHeadCtrol();
	virtual ~CHeadCtrol();
private:
	void PaintRight(CDC* pdc );  //画表头尾部
	void PaintItem( CDC* pdc,CRect& rect);   //画表头项
	void PaintDragLine(int pos, bool bOne= false); //拖拽线条
	void SortArrow(int nItem, int nType);  //nType 2 上升图案 1：下降图案 0：空白覆盖
	void PaintSortArrow(CDC* pdc,int nItem, int nType);

	////////////////////////////////
	//hover状态
	int			m_nHoverItem;
	bool		m_bTrackMouse;
	bool		m_bLButtonDown;
	CDibBitmap*	m_pBitmapBkOver;
	CDibBitmap*	m_pBitmapMidLineOver;
	CDibBitmap*	m_pBitmapBkDown;
	afx_msg LPARAM OnMouseLeave(WPARAM wp, LPARAM lp);
	////////////////////////////////

	HWND		m_hListWnd;
	bool        m_bottonDown;
	int         m_nDragPos;               //鼠标拖拽的当前项

	CDibBitmap*	m_pBitmapBk;
	CDibBitmap*	m_pBitmapMidLine;
	CDibBitmap*	m_pBitmapAsc;
	CDibBitmap* m_pBitmapDesc;

	COLORREF    m_textCol;

	vector<CDibBitmap> m_arrayBmp;        // 列标题图片
	vector<CString>    m_arrayText;       // 列标题文字    
	vector<bool>       m_arrayBoolSort;   // 是否切换排序图标 

	struct ItemPos
	{
	public:
		int pos[3];
		ItemPos()
		{
			ZeroMemory(pos,3*sizeof(int) );
		}
		ItemPos& operator=(const ItemPos& anItemPos)
		{

			if( this == &anItemPos )
			{
				return *this;
			}
			else
			{
				pos[0] = anItemPos.pos[0];
				pos[1] = anItemPos.pos[1];
				pos[2] = anItemPos.pos[2];
				return *this;
			}
		}
	};
	vector<ItemPos>     m_arrayItemPos;     // 位置 pos1, 起点 pos2终点 pos3拖拽最小位置

	pair<int, int>     m_currentSortInfo;  // <排序的项编号, 上升/下降/其它项编号占用 2/1/0>

	int m_width;
	int m_hight;
	int m_top;
public:

	int  InsertItem( CString strTitle,int width=0, int minWidth=0,bool bSort= false ); //
	void DeleteAllItem();
	void ModifyItemText(int index, CString str);
	void ModifyItemData(int index, CString str,int width, int minWidth=5 );
	void GetItemText(int index,CString& str);
	void SetHight(int hight);
	void SetWidth(CRect& rect);
	void SetTop( int nTop);
	void AttachListWnd( HWND hwnd );
	int  GetHeadRight()const;
	int  GetItemRight(int pos)const;
	int  GetItemLeft(int pos)const;
	int  GetCount(){return m_arrayItemPos.size(); }
	void SetHeadBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc);
	void SetHeadTextColor( COLORREF testCol );

protected:
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnScroolHead( WPARAM wParam, LPARAM lParam );

};


