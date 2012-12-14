// ModifyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgModify.h"
#include "YL_StringUtil.h"
#include "./src/Core/AfxGlobals.h"
#include "../../../Gui/Bitmap/DibBitmap.h"
// CModifyDlg 对话框

IMPLEMENT_DYNAMIC(CModifyDlg, CDialog)
CModifyDlg::CModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyDlg::IDD, pParent)
{
	strTitle		=	"问题反馈";
	strOtherReason	=	"";
	m_isSubmit		=	false;
	memset( &m_ti, 0, sizeof(m_ti) );
	m_fName.CreateFont(12, 0, 0, 0, FW_BOLD , FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
}

CModifyDlg::~CModifyDlg()
{
}

void CModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODIFY_ERROR1, m_ckError1);
	DDX_Control(pDX, IDC_MODIFY_ERROR2, m_ckError2);
	DDX_Control(pDX, IDC_MODIFY_ERROR3, m_ckError3);
	DDX_Control(pDX, IDC_MODIFY_ERROR4, m_ckError4);
	DDX_Control(pDX, IDC_OtherReason,	m_strOtherReason);
	DDX_Text(pDX, IDC_EDIT_CHECK_WORDS,	m_strCkeckCode);
}


BEGIN_MESSAGE_MAP(CModifyDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_PAINT()
	ON_EN_SETFOCUS(IDC_EDIT_CHECK_WORDS, OnEnSetfocusEditCheckWords)
END_MESSAGE_MAP()


// CModifyDlg 消息处理程序

void CModifyDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(m_ckError1.GetCheck()==BST_CHECKED)
		strSelectResult+="1";
	else
		strSelectResult+="0";

	if(m_ckError2.GetCheck()==BST_CHECKED)
		strSelectResult+="1";
	else
		strSelectResult+="0";

	if(m_ckError3.GetCheck()==BST_CHECKED)
		strSelectResult+="1";
	else
		strSelectResult+="0";

	if(m_ckError4.GetCheck()==BST_CHECKED)
		strSelectResult+="1";
	else
		strSelectResult+="0";

	m_strOtherReason.GetWindowText(strOtherReason);
	strOtherReason = strOtherReason.Trim();

	strOtherReason.Replace("\r\n"," ");
	if(strSelectResult!="0000" || strOtherReason != "")
	{
		CWnd *pWnd = NULL;
		if(m_strCkeckCode != m_strRandowWords && m_strRandowWords != ""){
			pWnd = GetDlgItem(IDC_STATIC_CHECK_TIP);
			if(pWnd != NULL){
				pWnd->SetWindowText("输入的数字错误!");
				return;
			}
		}else{
			pWnd = GetDlgItem(IDC_STATIC_CHECK_TIP);
			if(pWnd != NULL){
				pWnd->ShowWindow(SW_HIDE);
			}
			pWnd = GetDlgItem(IDC_STATIC_CHECK_WORDS);
			if(pWnd != NULL){
				pWnd->ShowWindow(SW_HIDE);
			}
			pWnd = GetDlgItem(IDC_EDIT_CHECK_WORDS);
			if(pWnd != NULL){
				pWnd->ShowWindow(SW_HIDE);
			}
		}

		m_isSubmit = true;

		string temp;
		YL_StringUtil::Format( temp, "NAME:%s|TYPE:%d|ERR:%s|OTHER:%s",
			m_ti.strName.c_str(), m_ti.eumType,	strSelectResult.GetString(), strOtherReason.GetString() );

		LogRealMsg( "REPORTERR", temp );

		RedrawWindow();
		Sleep(1000);
	}
	OnOK();
}

BOOL CModifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(strTitle);
	m_strOtherReason.SetLimitText(100);
	// 生成验证码
	int n = rand()%10000;
	m_strRandowWords.Format("%d", n);
	CWnd *pWnd = GetDlgItem(IDC_STATIC_CHECK_WORDS);
	if(pWnd != NULL){
		pWnd->SetWindowText(m_strRandowWords);
	}else{
		m_strRandowWords = "";
	}

	YL_Log("AddDlg.txt",LOG_DEBUG,"add","ModifyDlg");
	AfxGetUIManager()->UIAddDialog(this);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CModifyDlg::SetPlayingData( TAB_ITEM &ti )
{
	m_ti = ti;
	strTitle.Format( "问题反馈--当前打开项:%s", ti.strName.c_str() );
}

void CModifyDlg::OnPaint()
{
	CPaintDC dc(this);
	if(m_isSubmit)
	{
		CRect rect;
		GetClientRect(rect);
		rect.top=rect.bottom-25;
		CDibBitmap*	m_bModifyOK = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp( "ModifyOK" );
		if( m_bModifyOK != NULL )
		{
			int width = m_bModifyOK->GetWidth();
			int height = m_bModifyOK->GetHeight();
			int bottom = m_bModifyOK->pos_Bottom;
			rect.left = rect.left+10;
			rect.top = rect.bottom-bottom-height;
			rect.right = rect.left+width;
			rect.bottom = rect.bottom-bottom;
			m_bModifyOK->SetCDibRect(rect);
			m_bModifyOK->Draw( &dc, TRUE );
		}else
		{
			char szTemp[MAX_PATH];
			memset( szTemp, 0, MAX_PATH );
			_snprintf( szTemp, MAX_PATH - 1, "%s", "上传成功" );
			dc.SelectObject( &m_fName );
			dc.DrawText( szTemp, -1, &rect, DT_LEFT );
		}
	}
}

void CModifyDlg::OnOK()
{
	YL_Log("AddDlg.txt",LOG_DEBUG,"remove","ModifyDlg");
	AfxGetUIManager()->UIRemoveDialog(this);
	__super::OnOK();
}

void CModifyDlg::OnCancel()
{
	YL_Log("AddDlg.txt",LOG_DEBUG,"remove","ModifyDlg");
	AfxGetUIManager()->UIRemoveDialog(this);
	__super::OnCancel();
}

void CModifyDlg::OnEnSetfocusEditCheckWords()
{
	CWnd *pWnd = GetDlgItem(IDC_STATIC_CHECK_TIP);
	if(pWnd != NULL){
		pWnd->SetWindowText("请输入右边的数字");
	}
}
