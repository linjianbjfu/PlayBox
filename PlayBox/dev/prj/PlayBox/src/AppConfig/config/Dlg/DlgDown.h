#pragma once

#include "../SetBaseCtrls/SetBaseStatic.h"
#include "../SetBaseCtrls/SetBaseEdit.h"
#include "../SetBaseCtrls/SetBaseButton.h"
#include "BaseDlg.h"

class CDownDialog : public BaseDialog
{
	DECLARE_DYNAMIC(CDownDialog)

	SetBaseStatic m_static1;
	SetBaseStatic m_staticPicSavePath;

	CString	m_strSwfPath;	//flash���Ŀ¼
	SetBaseEdit	m_EditSwfPath;	//flash���Ŀ¼

	CString m_strPicSavePath; // ��ͼ����·��
	SetBaseEdit m_EditPicSavePath; // ��ͼ����·��
public:
	CDownDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDownDialog();

	virtual void	SetDefault();
	virtual void	LoadConf();
	virtual void	SaveConf( bool bSave2File = true );
	virtual bool	IsAllInputOK(string& strMsg);
// Dialog Data
	enum { IDD = IDD_DIALOG_DOWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBtnBroPicSavePath();
};
