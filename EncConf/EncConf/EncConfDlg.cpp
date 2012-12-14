// EncConfDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EncConf.h"
#include "EncConfDlg.h"
#include "./common/YL_EncFileReg.h"
#include "./common/YL_FileInfo.h"
#include "./common/YL_StringUtil.h"
#include <string>
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEncConfDlg 对话框



CEncConfDlg::CEncConfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEncConfDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEncConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEncConfDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CEncConfDlg 消息处理程序

BOOL CEncConfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEncConfDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CEncConfDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CEncConfDlg::GetSection( string& str, string& strSection )
{
	size_t idx1 = str.find( "[" );
	size_t idx2 = str.find( "]" );
	if( idx1 != string::npos
		&& idx2 != string::npos
		&& idx1 < idx2 )
	{
		strSection = str.substr( idx1+1, idx2-idx1-1 );
		return true;
	}else
	{
		return false;
	}
}

bool CEncConfDlg::GetKeyValue( string& str, string& strKey, string& strValue )
{
	size_t idx1 = str.find( "=" );
	if( idx1 != string::npos )
	{
		strKey = str.substr( 0, idx1 );
		strValue = str.substr ( idx1+1 );
		return true;
	}else
	{
		return false;
	}
}

void CEncConfDlg::OnBnClickedButton1()
{
	CString cstrConfPath;
	GetDlgItem( IDC_EDIT1 )->GetWindowText(cstrConfPath);
	//一行一行读，然后生成enc后的字符串输出到同目录下的文件
	string strNormalConfPath = string( cstrConfPath );
	string strFileName = strNormalConfPath + ".dat";
	string strContent = YL_FileInfo::GetFileContent( strNormalConfPath );

	vector<string>  vec;
	string strDelims = "\r\n";
	YL_StringUtil::Tokenize( strContent, vec, strDelims );
	string strSection, strKey, strValue;
	string strOutPut;
	for( vector<string>::iterator it1 = vec.begin(); it1 != vec.end(); ++it1 )
	{
		if( GetSection( *it1, strSection ) )
		{
			string strEncSection = YL_EncFileReg::StrHash( strSection );
			strOutPut += "[" + strEncSection + "]\r\n";
		}else
		{
			bool b2 = GetKeyValue( *it1, strKey, strValue );
			if( b2 )
			{
				string strEncKey     = YL_EncFileReg::StrHash( strKey );
				string strEncValue   = YL_EncFileReg::Enc( strValue );
				strOutPut += strEncKey + "=" + strEncValue + "\r\n";
			}
		}

	}
	YL_FileInfo::SetFileContent( strFileName, strOutPut );
	
}
