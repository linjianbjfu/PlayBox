#pragma once
#include "cdib.h"

class CDibBitmap :
	public CDib
{
private:
	CString m_path; //ͼƬ·��

	void GetShowRect(CRect &Imgrc,const CRect Destrc);
public:
	CDibBitmap(void);
	~CDibBitmap(void);

	HBITMAP CreateBitmap(CDC* pDC);
	BOOL LoadFromFile(CString path);
	BOOL LoadFromFile(CString path,P_STRUCT_CUTRECT pCutRect);
	BOOL LoadFromMem(CString path,void* pVoid, UINT nSize, P_STRUCT_CUTRECT pCutRect);
	BOOL LoadFromID(UINT ID);
	BOOL Draw(CDC *pDC,BOOL isBKTransparent, COLORREF color=RGB(0,0,0), int mode = CDib::STRETCH);
	BOOL Draw(CDC*pDC, int srcX , int srcY, int srcW, int srcH, int dstX, int dstY , int dstW, int dstH);
	BOOL DrawImageList(CDC*pDC, int nIndex, POINT pt, int nCount=1/*����СͼƬ��Ȼ��ƣ�����������*/); //������MFC CImageList �Ļ���
	BOOL ValidCheck();
	void SetCDibRect(CRect rect);
	void SetCount( int nCount); // ���ڰ���Ż��ƣ� ����ͼƬ����
	HRGN CreateRgnFromBitmap(COLORREF color,CDC* dc);
	int GetWidth();
	int GetHeight();
	CRect m_rect;
	CString GetPath();
	int pos_Top;
	int pos_Left;
	int pos_Right;
	int pos_Bottom;
	bool is_Loaded;
	bool is_ChangeColor;
	bool has_PopupError;
	HRGN CreateRgnFromBitmap(COLORREF color , CWnd * pWnd);
	CString m_strName;
	int  m_count;  // ���ڰ���Ż���
	int GetImageCount()const { return m_count; };

	BOOL DrawStrechFromCenter(CDC *pDC);
};
