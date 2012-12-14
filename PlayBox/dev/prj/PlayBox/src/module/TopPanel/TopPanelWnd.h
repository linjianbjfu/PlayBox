#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../DataInterface/ILayoutChangeObserver.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

static const int   s_INT_SKINMENU_ID	= 100;
static const int   s_INT_SUBJECT_ID		= 150;

#define MAX_SHOW_NEW_SKIN        3

class IFlashHolderData;
class CShowMenu;
class IPanelChange;

class CTopPanelWnd : public CBasicWnd,
					 public ILayoutChangeObserver,
					 public IPanelChangeObserver
{
	DECLARE_DYNAMIC(CTopPanelWnd)
public:
	CTopPanelWnd();
	virtual ~CTopPanelWnd();

	void	LoadSkin();
	void	AddLayers();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnBnClickedClose();
	afx_msg void	OnBnClickedMin();
	afx_msg void	OnBnClickedLarge();
	afx_msg void	OnBnClickedNormal();
	afx_msg void	OnBnClickedSetting();
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void	OnDestroy();
	afx_msg void	OnBnClickedHold();

	afx_msg LRESULT OnSaveLayout(WPARAM,LPARAM);
	afx_msg LRESULT OnGetLayout(WPARAM,LPARAM);
	afx_msg void	OnAppExit();
	afx_msg void	OnMini();
	afx_msg void	OnShowResource();
	afx_msg void	OnSetting();
	bool			CanShowTooltip();
	void			InitTips();	
public:
	CxSkinButton	m_btnMin;
	CxSkinButton	m_btnClose;
	CxSkinButton	m_btnNormal;
	CxSkinButton	m_btnLarge;
	CxSkinButton	m_btnSetting;

	bool			m_bNormal;
	int				m_iDesktopWidth;
	int				m_iDesktopHeight;
	int				m_iXPos;
	int				m_iYPos;
	int				m_iWndNormalWidth;
	int				m_iWndNormalHeigth;

	bool			m_bMainWndHold;
	vector<string>  m_vecSkins;
	vector<string>	m_vecSubjects;
	vector<string>	m_vecSubjectShowNames;

	int				m_mincx;
	int				m_mincy;
	bool			m_bHideMainWindow;

public:
	virtual void	ILayoutChangeOb_InitFinished();
	virtual void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);

	void	IPanelChangeOb_WindowMax();
	void	IPanelChangeOb_WindowMin();
	void	IPanelChangeOb_WindowRestore();
	void	IPanelChangeOb_WindowClose();
};