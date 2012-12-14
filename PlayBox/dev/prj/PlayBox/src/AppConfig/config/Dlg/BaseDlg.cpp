#include "stdafx.h"
#include "BaseDlg.h"

IMPLEMENT_DYNAMIC(BaseDialog, CDialog)

BEGIN_MESSAGE_MAP(BaseDialog, CDialog)
	//ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL BaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadConf();
	UpdateData( FALSE );
	return TRUE;
}

void BaseDialog::OnCancel()
{
	CWnd* pWnd = GetParent();
	if( pWnd != NULL )
	{
		::PostMessage( pWnd->GetSafeHwnd(), WM_COMMAND, MAKELPARAM(IDCANCEL,BN_CLICKED), 0 );
	}	
}

void BaseDialog::OnOK()
{
}

HBRUSH BaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	hbr = ::CreateSolidBrush( RGB(255,255,255) );
	return hbr;
}

void BaseDialog::SthChanged()
{
	CWnd* pWnd = GetParent();
	if( pWnd != NULL )
	{
		::EnableWindow(::GetDlgItem(pWnd->GetSafeHwnd(), IDC_BUTTON_APPLY), TRUE);
	}
}