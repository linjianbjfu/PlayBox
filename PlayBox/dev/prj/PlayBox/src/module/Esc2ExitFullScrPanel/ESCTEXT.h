#pragma once

#include "resource.h"
// ESCTEXT �Ի���

class ESCTEXT : public CDialog
{
	DECLARE_DYNAMIC(ESCTEXT)

public:
	ESCTEXT(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ESCTEXT();

// �Ի�������
	enum { IDD = IDD_DIALOG_ESCTEXT };


	void DrawBar(CDC* dc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
