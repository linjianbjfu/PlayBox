// EncConfDlg.h : 头文件
//

#pragma once

#include <string>
using namespace std;
// CEncConfDlg 对话框
class CEncConfDlg : public CDialog
{
// 构造
public:
	CEncConfDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ENCCONF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
