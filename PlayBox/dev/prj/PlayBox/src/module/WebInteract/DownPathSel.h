#pragma once
#include "afxwin.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/EditGlow.h"

// CDownPathSel 对话框

class CDownPathSel : public CDialog
{
	DECLARE_DYNAMIC(CDownPathSel)

public:
	CDownPathSel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownPathSel();

// 对话框数据
	enum { IDD = IDD_DLG_DOWNPATH_SEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	CxSkinButton m_btnBrowse;
	CxSkinButton m_btnCancel;
	CxSkinButton m_btnOk;
	
	//
	// added by wang
	//
	//CCheckListBox m_chkPopUpDialog;
	CxSkinButton m_chkPopUpDialog;
	CxSkinButton m_ChkUnPopUpDialog;
	BOOL m_bChkIsPopUp;
	//
	
	void CheckBox();
	void UnCheckBox();
	BOOL m_chkBox;
	


	CEditGlow	 m_edit;
	CString		 m_strPath;

	CDibBitmap*  m_bkg;
	CDibBitmap*	 m_bSepLine;

	CFont		 m_Font;
	CFont*		 m_oldFont;

	string		 m_strName;
	string		 m_strSize;
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg UINT OnNcHitTest(CPoint point);
protected:
	virtual void OnOK();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void SetParam (string strName, string strSize);
	CString GetPath () { return m_strPath; }
};
