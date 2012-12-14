#pragma once

#include "resource.h"
#include "BaseDlg.h"
#include "../SetBaseCtrls/SetBaseStatic.h"

class LyricConfigAboutDlg : public BaseDialog
{
	DECLARE_DYNAMIC( LyricConfigAboutDlg )
public:
	SetBaseStatic		m_staticAbout1;
	SetBaseStatic		m_staticAbout2;
	SetBaseStatic		m_staticAbout3;

	LyricConfigAboutDlg();
	virtual ~LyricConfigAboutDlg();
	enum { IDD = IDD_LYRIC_ABOUT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	virtual void	SetDefault();
	virtual void	LoadConf();
	virtual void	SaveConf( bool bSave2File = true );
	virtual bool	IsAllInputOK(string& strMsg);
};
