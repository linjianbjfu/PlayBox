// EncConfDlg.h : ͷ�ļ�
//

#pragma once

#include <string>
using namespace std;
// CEncConfDlg �Ի���
class CEncConfDlg : public CDialog
{
// ����
public:
	CEncConfDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ENCCONF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	bool GetSection( string& str, string& strSection );
	bool GetKeyValue( string& str, string& strKey, string& strValue );
};
