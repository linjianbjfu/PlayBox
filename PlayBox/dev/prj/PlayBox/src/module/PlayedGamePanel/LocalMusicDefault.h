#pragma once
#include "../../../gui/CommonControl/xSkinButton.h"
#include "../Scan/IScanObserver.h"

class CLocalMusicDefault : public CWnd, public IScanObserver
{
public:
	CLocalMusicDefault();
	~CLocalMusicDefault();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);
	void SetInfoText();
private:
	CxSkinButton*	m_btnChoose;
	CxSkinButton*	m_btnAuto;
	CFont			m_font;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnBtnClickChoose();
	afx_msg void OnBtnClickAuto();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	void IScanObserver_StartScan();
	void IScanObserver_EndScan();

};