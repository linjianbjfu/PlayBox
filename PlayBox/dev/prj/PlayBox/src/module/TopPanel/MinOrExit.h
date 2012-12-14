#pragma once
#include "afxwin.h"
#include "../../gui/CommonControl/xSkinButton.h"


// CMinOrExit 对话框

class CMinOrExit : public CDialog
{
	DECLARE_DYNAMIC(CMinOrExit)

public:
	CMinOrExit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMinOrExit();

// 对话框数据
	enum { IDD = IDD_DLG_MINOREXIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	CxSkinButton m_btnClose;
	CxSkinButton m_btnCancle;
	CxSkinButton m_btnOk;
	CxSkinButton m_rad1unCheck;
	CxSkinButton m_rad1Checked;
	CxSkinButton m_rad2unCheck;
	CxSkinButton m_rad2Checked;
	CxSkinButton m_boxunCheck;
	CxSkinButton m_boxChecked;
	BOOL m_chkRad;
	BOOL m_chkBox;
	void CheckRad1();
	void CheckRad2();
	void CheckBox();
	void UnCheckBox();
	CDibBitmap*  m_bkg;
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
protected:
	virtual void OnOK();
};
