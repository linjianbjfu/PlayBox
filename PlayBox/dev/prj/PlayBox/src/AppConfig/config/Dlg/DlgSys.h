#pragma once

#include "../SetBaseCtrls/SetBaseStatic.h"
#include "../SetBaseCtrls/SetBaseEdit.h"
#include "../SetBaseCtrls/SetBaseButton.h"
#include "BaseDlg.h"

class CSysDialog : public BaseDialog
{
	DECLARE_DYNAMIC(CSysDialog)

	BOOL			m_bAutoRun;			//开机自动启动
	SetBaseButton	m_checkAutoRun;

	BOOL m_minOrExit;
	BOOL m_noAsk;
	SetBaseButton m_minCtrl;
	SetBaseButton m_exitCtrl;
	SetBaseButton m_noAskCtrl;
	SetBaseStatic m_staticMinOrExit;

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
private:
	HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc);
};
