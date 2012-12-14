#pragma once

#include "SkinBotton.h"
class CShowMenu;
class CSubSkinButton : public CSkinButton
{
protected:
	//CGdiPlusBitmap* m_pPngHoverImage;

	CGdiPlusBitmap*	m_pCurDelete;
	CGdiPlusBitmap*	m_pPngDeleteDown;
	CGdiPlusBitmap*	m_pPngDeleteOver;
	CGdiPlusBitmap*	m_pPngDeleteNormal;
	CGdiPlusBitmap*	m_pPngFrame;
	BOOL m_bClickPng;
	BOOL m_bShowDel;

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawHoverImage(Gdiplus::Graphics& gdi);
	void DrawBtnFrame(Gdiplus::Graphics& gdi);

private:
	CGdiPlusBitmap * CreatePngBitmap(CString str);
	BOOL IsMouseInDeleteButton();
	CString m_strLastToolTip;
public:
	CSubSkinButton();
	virtual ~CSubSkinButton();
	BOOL SetPngHoverSkin(CString str);
	BOOL SetPngFrame(CString str);
	virtual BOOL IsClickPng();
	virtual void SetClickPng(BOOL bclick);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT aiIDEvent);
	afx_msg LRESULT OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) ;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	void OnApplySkin();
	void OnDeleteSkin();
	void OnMoreSkin();
private:
	CShowMenu*		m_pShowMenu;
};