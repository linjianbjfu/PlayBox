#pragma once

#include "../SetBaseCtrls/SetBaseStatic.h"
#include "../SetBaseCtrls/SetBaseEdit.h"
#include "../SetBaseCtrls/SetBaseButton.h"
#include "BaseDlg.h"
#include "../GUI/CommonControl/xSkinButton.h"

class CSysDialog : public BaseDialog
{
	DECLARE_DYNAMIC(CSysDialog)

	BOOL			m_bAutoRun;			//开机自动启动
	CxSkinButton	m_checkedAutoRun;
	CxSkinButton	m_unChkedAutoRun;

	BOOL m_minOrExit;					// TRUE:exit	FALSE:min
	CxSkinButton	m_radMin_Checked;
	CxSkinButton	m_radMin_unChked;
	CxSkinButton	m_radExit_Checked;
	CxSkinButton	m_radExit_unChked;

	BOOL m_bNoAsk;
	CxSkinButton	m_checkedNoAsk;
	CxSkinButton	m_unChkedNoAsk;

	SetBaseStatic m_staticMinOrExit;

	BOOL		m_bUseBossKey;	// 是否启用老板键
	CxSkinButton	m_checkUseBossKey;
	CxSkinButton	m_unChkUseBossKey;
	CHotKeyCtrl m_BossKeyCtrl;	// 老板键输入控件

public:
	CSysDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSysDialog();

	virtual void	SetDefault();
	virtual void	LoadConf();
	virtual void	SaveConf( bool bSave2File = true );
	virtual bool	IsAllInputOK(string& strMsg);
// Dialog Data
	enum { IDD = IDD_DIALOG_SYS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadMin();
	afx_msg void OnBnClickedRadExit();
	afx_msg void OnBnClickedChkNoask();
	afx_msg void OnBnClickedCheckAutoRun();
	afx_msg void OnBnClickedChkBosskey();

	BOOL UpdateData(BOOL bSaveAndValidate  = TRUE );	//重载UpdateData;
private:
	HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc);
public:
	virtual BOOL OnInitDialog();
};
