#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../gui/CommonControl/BasicSkinWnd.h"
#include "../../gui/CommonControl/QQBar/WndSlider.h"
#include "../../datainterface/IQQItemCountChangeObserver.h"
#include "../../DataInterface/ILayoutChangeObserver.h"

// CQQPanel
#include <vector>
#include <string>
using std::vector;
using std::string;

class MyGameTreeCtrl;

struct PANEL_DATA;

class MyGamePanelWnd : public CBasicWnd,
					public CBasicSkinWnd,
					public IQQItemCountChangeObserver,
					public ILayoutChangeObserver
{
	DECLARE_DYNAMIC(MyGamePanelWnd)

public:
	MyGamePanelWnd();
	virtual ~MyGamePanelWnd();
	void	LoadSkin();
	void	AddLayers();

	void	SelectQQView(const string& );
	virtual void IQQItemChangeOb_CountChange( unsigned int iNewCount );
	void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnWMQQShift(WPARAM wParam,LPARAM lParam);

private:
	void	CreateTreePanels( MyGameTreeCtrl*, int index);
	void	CreateControls( );

private:

	CWndSliderView			m_QQPanelWnd;
	MyGameTreeCtrl*			m_pQQTreeCtrl;	
	vector<HWND>			m_vecQQHwnd;

	int						m_iTreeSize;
	int						m_iItemIndex;
	int						m_iSelectIndex;
};
