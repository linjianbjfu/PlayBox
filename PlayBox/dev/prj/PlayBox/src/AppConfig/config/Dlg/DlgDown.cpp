#include "stdafx.h"
#include "DlgDown.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "./src/Core/AfxGlobals.h"
#include "../ConfigAppDef.h"
#include "../ConfigParaDefault.h"
#include "PlayBoxDlg.h"
#include "Global/GlobalSwfPath.h"

IMPLEMENT_DYNAMIC(CDownDialog, BaseDialog)
CDownDialog::CDownDialog(CWnd* pParent /*=NULL*/)
{
	CDialog( CDownDialog::IDD, pParent );

}

CDownDialog::~CDownDialog()
{
}

BEGIN_MESSAGE_MAP(CDownDialog, BaseDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_BRO_SAVE_PATH, OnBtnBroPicSavePath)
END_MESSAGE_MAP()

void CDownDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strPicSavePath);
	DDX_Control(pDX, IDC_EDIT_SAVE_PATH, m_EditPicSavePath);

	DDX_Text(pDX, IDC_EDIT1, m_strSwfPath);
	DDX_Control(pDX, IDC_EDIT1, m_EditSwfPath);

	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_STATIC_PIC_SAVE_PATH, m_staticPicSavePath);
}

void CDownDialog::SaveConf( bool bSave2File )
{
	UpdateData();
	AfxUserConfig*	pUserConfig = AfxGetUserConfig();

	string strValue = string(m_strSwfPath);
	YL_DirInfo::MakeDir( strValue );
	pUserConfig->SetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strValue, bSave2File );
	// 
	// added by wang
	//	
	string strPicSavePath= (CString)m_strPicSavePath;
	if (strPicSavePath.length()<=0)
	{
		MessageBox("ͼƬ����·������ȷ,�뵥���������ѡ��");
	}
	pUserConfig->SetConfigStringValue(CONF_APP_MODULE_NAME, CONF_APP_PIC_SAVE_PATH, strPicSavePath, bSave2File);
}

void CDownDialog::LoadConf()
{
	AfxUserConfig*	pUserConfig = AfxGetUserConfig();
	string strValue;
	GlobalSwfPath::GetConfigSwfPath(strValue);
	m_strSwfPath = CString( strValue.c_str() );
	//
	// added by wang
	//
	string strPictSavePath ;
	pUserConfig->GetConfigStringValue(CONF_APP_MODULE_NAME, CONF_APP_PIC_SAVE_PATH, strPictSavePath);
	m_strPicSavePath = CString(strPictSavePath.c_str());
	// �ж������е�Ŀ¼�Ƿ���û���ǰ������·����һ�µ�? ��Ϊ��ͬ���û�������·���ǲ�һ����.
	// �����б�Ҫ�ж�һ��Ŀ¼�Ƿ����ǰ�û�������·��һ��.��һ�·��ʾ���Ĭ��Ŀ¼
	string strDefaultPath = ConfigParaDefault::GetDefaultAPP_PicSave_Path();
	if( strPictSavePath != strDefaultPath ) {
		m_strPicSavePath = (CString)(strDefaultPath.c_str());
	}
}

void CDownDialog::SetDefault()
{
	m_strSwfPath = CString( ConfigParaDefault::GetDefaultAPP_SWF_PATH().c_str() );
	m_strPicSavePath =CString( ConfigParaDefault::GetDefaultAPP_PicSave_Path().c_str());
	UpdateData( FALSE );
}

bool CDownDialog::IsAllInputOK(string& strMsg)
{
	if( UpdateData() == FALSE )
		return false;

	//�ж��û����õ�·���Ƿ���Ч
	string strSwfPath = string( m_strSwfPath );
	if( ConfigParaDefault::IsPathValid( strSwfPath ) )
	{
		return true;
	}else
	{
		strMsg = "����Ŀ¼�޷�����";
		return false;
	}
}

void CDownDialog::OnBnClickedButton1()
{
	//��Ŀ¼ѡ��Ի���
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = this->m_hWnd;//m_hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = "ѡ����Ϸ���Ŀ¼";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	BOOL bRet = FALSE;

	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
		{
			if( YL_DirInfo::MakeDir(szFolder) )
			{
				m_strSwfPath = szFolder;
			}
			else
			{
				if( MessageBox("����Ĵ��Ŀ¼, ����������!", "����", MB_OK) == 0 )
					return;
			}
			m_EditSwfPath.SetWindowText( m_strSwfPath );
			m_EditSwfPath.SetFocus();
			bRet = TRUE;
			SthChanged();
		}

		IMalloc *pMalloc = NULL; 
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
		{
			pMalloc->Free(pidl);  
			pMalloc->Release(); 
		}
	}
}

//
// added by wang
//
void CDownDialog::OnBtnBroPicSavePath()
{
	//��Ŀ¼ѡ��Ի���
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = this->m_hWnd;//m_hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = "ѡ����Ϸ��ͼ���Ŀ¼";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	BOOL bRet = FALSE;

	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
		{
			if( YL_DirInfo::MakeDir(szFolder) )
			{
				m_strPicSavePath = szFolder;
			}
			else
			{
				if( MessageBox("Ŀ¼����, ����������!", "����", MB_OK) == 0 )
					return;
			}
			m_EditPicSavePath.SetWindowText( m_strPicSavePath );
			m_EditPicSavePath.SetFocus();
			bRet = TRUE;
			SthChanged();
		}

		IMalloc *pMalloc = NULL; 
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
		{
			pMalloc->Free(pidl);  
			pMalloc->Release(); 
		}
	}
}