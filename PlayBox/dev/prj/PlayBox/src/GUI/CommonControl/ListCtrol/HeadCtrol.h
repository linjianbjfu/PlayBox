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
	void PaintRight(CDC* pdc );  //����ͷβ��
	void PaintItem( CDC* pdc,CRect& rect);   //����ͷ��
	void PaintDragLine(int pos, bool bOne= false); //��ק����
	void SortArrow(int nItem, int nType);  //nType 2 ����ͼ�� 1���½�ͼ�� 0���հ׸���
	void PaintSortArrow(CDC* pdc,int nItem, int nType);

	////////////////////////////////
	//hover״̬
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
	int         m_nDragPos;               //�����ק�ĵ�ǰ��

	CDibBitmap*	m_pBitmapBk;
	CDibBitmap*	m_pBitmapMidLine;
	CDibBitmap*	m_pBitmapAsc;
	CDibBitmap* m_pBitmapDesc;

	COLORREF    m_textCol;

	vector<CDibBitmap> m_arrayBmp;        // �б���ͼƬ
	vector<CString>    m_arrayText;       // �б�������    
	vector<bool>       m_arrayBoolSort;   // �Ƿ��л�����ͼ�� 

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
	vector<ItemPos>     m_arrayItemPos;     // λ�� pos1, ��� pos2�յ� pos3��ק��Сλ��

	pair<int, int>     m_currentSortInfo;  // <���������, ����/�½�/��������ռ�� 2/1/0>

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


