#pragma once


// CFileDialogEx

class CFileDialogEx : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialogEx)

public:
	CFileDialogEx(BOOL bOpenFileDialog, // ���� FileOpen Ϊ TRUE������ FileSaveAs Ϊ FALSE
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CFileDialogEx();
	INT_PTR DoModal();
private:
	CString m_ImgstrPath;
protected:
	DECLARE_MESSAGE_MAP()
	LRESULT OnSetImgPath(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
};


