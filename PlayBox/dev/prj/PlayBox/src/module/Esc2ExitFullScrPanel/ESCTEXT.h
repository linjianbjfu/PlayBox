#pragma once

#include "resource.h"
// ESCTEXT 对话框

class ESCTEXT : public CDialog
{
	DECLARE_DYNAMIC(ESCTEXT)

public:
	ESCTEXT(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ESCTEXT();

// 对话框数据
	enum { IDD = IDD_DIALOG_ESCTEXT };


	void DrawBar(CDC* dc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
