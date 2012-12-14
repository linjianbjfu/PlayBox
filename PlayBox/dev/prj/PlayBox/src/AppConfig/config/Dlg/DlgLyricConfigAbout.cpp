#include "stdafx.h"
#include "DlgLyricConfigAbout.h"
#include "../ConfigSettingDef.h"
#include "YL_StringUtil.h"
#include "LhcImg.h"

#define INT_LENGTH           64

IMPLEMENT_DYNAMIC(LyricConfigAboutDlg, BaseDialog)

LyricConfigAboutDlg::LyricConfigAboutDlg()
{
	CDialog( LyricConfigAboutDlg::IDD, NULL );
}

LyricConfigAboutDlg::~LyricConfigAboutDlg()
{
}

BEGIN_MESSAGE_MAP(LyricConfigAboutDlg, BaseDialog)

END_MESSAGE_MAP()

void LyricConfigAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ABOUT_1, m_staticAbout1);
	DDX_Control(pDX, IDC_STATIC_ABOUT_2, m_staticAbout2);
	DDX_Control(pDX, IDC_STATIC_ABOUT_3, m_staticAbout3);
}

void LyricConfigAboutDlg::SetDefault()
{
}
void LyricConfigAboutDlg::LoadConf()
{
}
void LyricConfigAboutDlg::SaveConf( bool bSave2File )
{
}

bool LyricConfigAboutDlg::IsAllInputOK(string& strMsg)
{
	return true;
}
