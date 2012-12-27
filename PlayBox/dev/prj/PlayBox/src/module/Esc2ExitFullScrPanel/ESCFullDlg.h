#pragma once
#include "ESCTEXT.h"

class CDibBitmap;
class ESCTEXT;

// ESCFullDlg 对话框

class ESCFullDlg : public CDialog
{
	DECLARE_DYNAMIC(ESCFullDlg)

public:
	ESCFullDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ESCFullDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ESCFULLSCR };

	void DrawBar(CDC* dc);
	void LoadSkin();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

	void		ShowDlg(BOOL bShow);

private:
	CRect	m_btnRect;

	CDibBitmap*		m_pBitmap;
	ESCTEXT*		m_pEscTextDlg;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#pragma comment( lib, "gdiplus.lib" ) 
