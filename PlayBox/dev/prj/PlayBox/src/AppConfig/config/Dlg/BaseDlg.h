#pragma once

class BaseDialog : public CDialog
{
	DECLARE_DYNAMIC(BaseDialog)

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
public:
	string	m_strAlert;
	virtual void	SthChanged();			//控件值发生了变化

	virtual void	SetDefault() = 0;	
	virtual void	LoadConf() = 0;
	virtual void	SaveConf( bool bSave2File = true ) = 0;
	virtual bool	IsAllInputOK(string& strMsg) = 0;
};