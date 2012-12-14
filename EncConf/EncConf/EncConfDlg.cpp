// EncConfDlg.cpp : ʵ���ļ�
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


// CEncConfDlg �Ի���



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


// CEncConfDlg ��Ϣ�������

BOOL CEncConfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CEncConfDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	//һ��һ�ж���Ȼ������enc����ַ��������ͬĿ¼�µ��ļ�
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
