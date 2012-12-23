#pragma once
#include <vector>
#include <string>
#include "../../datainterface/IConfUpdateObserver.h"

struct InfoAd
{
	string url;
	string name;
	string id;
	CRect  rect;
};

class CBottomAdPanel : public CWnd,	
						public IConfUpdateObserver
{
	DECLARE_DYNAMIC(CBottomAdPanel)
private:
	vector<InfoAd>	m_vInfoAd;
	CFont			m_Font;
public:
	CBottomAdPanel();
	virtual ~CBottomAdPanel();
	void DrawAd(CDC* dc);
	void IConfUpdate_Update();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	void ReadADConf();
private:
	bool m_bTrackMouse;
};
