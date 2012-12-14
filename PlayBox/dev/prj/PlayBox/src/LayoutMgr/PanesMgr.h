#pragma once
#include "ETSLayout.h"
#include "../util/MarkupArchive.h"
#include "../datainterface/ILayoutChangeObserver.h"
#include "../DataInterface/IPanelChangeObserver.h"
#include "ILayoutMgr.h"

class CSkinMgr;
class CPanesMgr : public ILayoutMgr, 
				  public IPanelChangeObserver
{
private:
	CMarkupArchive m_xml;
	Displaymode					m_modeDisplay;
	//换色时改标志设为true，此时调用RecursionCreatePane方法不创建pane，只会递归到各个paneitem并通过CreateControlFactory
	BOOL						m_bJustLoadColor;
	BOOL						m_bChangedSkin;
	//读取各属性
	ETSLayoutMgr::layResizeMode getModeSize();

	int getSizeX(){CString str = m_xml.GetAttrib("sizeX"); return atoi(str);};

	int getSizeY(){CString str = m_xml.GetAttrib("sizeY"); return atoi(str);};

	int getSizeXMin(){CString str = m_xml.GetAttrib("sizeXMin"); return (str == "") ? -1:atoi(str);};

	int getSizeYMin(){CString str = m_xml.GetAttrib("sizeYMin"); return (str == "") ? -1:atoi(str);};

	int getSplSecondSize(){CString str = m_xml.GetAttrib("splpane-size"); return atoi(str);};

	int getSecondSize(){ CString str = m_xml.GetAttrib("second-size"); return atoi(str);};

	int getClosedPos(){CString str = m_xml.GetAttrib("closed-pos"); return atoi(str);};

	int getClosedOri(){CString str = m_xml.GetAttrib("closed-ori"); return (str == "FIRST")? 0:1;};

	int getSplOpenSize(){CString str = m_xml.GetAttrib("open-size"); return atoi(str);};

	int getSplHideCloseBtn(){CString str = m_xml.GetAttrib("hide-closebtn"); return atoi(str);};

	int getSplSlimSign(){CString str = m_xml.GetAttrib("slim"); return atoi(str);};

	ETSLayoutMgr::itemType getItemType();

	ETSLayoutMgr::layOrientation getOrization(){CString str = m_xml.GetAttrib("ori"); if(str == "NO_POS") return ETSLayoutMgr::NO_ORI; return (str == "VERT") ? ETSLayoutMgr::VERTICAL: ETSLayoutMgr::HORIZONTAL;};

	BOOL	FindNode(CString tag, CString name);

	//保存panel
	void	SaveSplPane();

	int		RecursionSavePane(CString);

	BOOL	SaveMultiLayer(CString parentOri);

	CMapStringToOb	m_mapPanels;

	CMapStringToOb	m_mapCtrls;

	CMap<CString, LPCSTR ,ETSLayoutMgr::CPaneBase, ETSLayoutMgr::CPaneBase>  m_mapPaneMulti;

	CMap<HWND , HWND, ETSLayoutMgr *, ETSLayoutMgr*>	m_mapLayoutMgr;

	CMap<HWND , HWND, ETSLayoutMgr *, ETSLayoutMgr*>	m_mapBkLayoutMgr;

	CMap<CString, LPCSTR ,ETSLayoutMgr::CPaneBase, ETSLayoutMgr::CPaneBase>  m_mapPanes;

	ETSLayoutMgr::CPaneBase		CreatePaneItem(CString paneName, ETSLayoutMgr *pLayout);

	ETSLayoutMgr::CPane			CreatePaneSpl(CString paneName, ETSLayoutMgr *pLayout);

	ETSLayoutMgr::CPane			CreatePane(CString paneName, ETSLayoutMgr *pLayout);

	ETSLayoutMgr::CPaneMulti	CreatePaneMulti(CString paneName, ETSLayoutMgr *pLayout,CString current);

	BOOL						CreateMultiLayer(ETSLayoutMgr::CPaneMulti *parent, ETSLayoutMgr *pLayout, ETSLayoutMgr::CPane *createPane,CString current);

	BOOL						RecursionCreatePane(ETSLayoutMgr::CPane*, ETSLayoutMgr *pLayout,ETSLayoutMgr::CPane *createPane);

	void						SetPositionParam(ETSLayoutMgr::PaneBase *base);

	void						SetLayoutBorder(ETSLayoutMgr *pLayout);

	void						AddPaneMulti(CString name, ETSLayoutMgr::CPaneMulti *pane);

	BOOL						GetPaneMulti(CString name,  ETSLayoutMgr::CPaneMulti &pane);

	//控件layout
	void						AddWndLayoutMgr(HWND hWnd, ETSLayoutMgr *pLayout);  

	//背景layout
	void						AddBkLayoutMgr(HWND hWnd, ETSLayoutMgr *pLayout); 

	void						ClearBkLayoutMap();

	void						ClearWndLayoutMap();

	void						ChangeWndColor(CWnd *pParent, CString name, CString strControlName);

	void						SetWindowPos(CWnd *pWnd);

	void						SavePanes();

	void						AddPane(CString name, ETSLayoutMgr::CPaneBase pane);

	bool						GetPane(CString name, ETSLayoutMgr::CPaneBase &pane);

	CSkinMgr *					m_pSkin;
	COLORREF					m_skinColor;
	BOOL						m_bWaitOutFullScreen; //这两个变量表示等全屏退出时才换肤
	CString						m_strWaitSkin;
	HANDLE						m_SubEvent;
	void						IPanelChangeOb_OutFullScreen();
public:
	CPanesMgr(void);

	~CPanesMgr(void);

	bool						SetXmlPath(CString path);

	void						SetSkin(CSkinMgr *pSkin){m_pSkin = pSkin;};

	//窗口layout接口
	void						AppExit();

	void						SaveWndPanel(CString wndName = "", CString controlName = "");
	void						SaveSepPanel();

	void						ShowLayer(CString paneName, CString layerName);

	CString						GetCurrentLayer(CString paneName);

	void						RegisterWnd(CString name, CWnd *pWnd);

	CWnd *						GetWnd(CString name);
	//控件layout接口
	ETSLayoutMgr *				GetWndLayoutMgr(HWND hWnd);

	//
	ETSLayoutMgr *				GetBkLayoutMgr(HWND hWnd);

	void						PaintBkGround(HWND hWnd , CDC *pDC, BOOL bUseMemDC = true);

	void						RegisterCtrl(CWnd *pWnd, CString name, CWnd *pCtrl);

	CWnd*						GetCtrl(HWND hWnd, CString name);

	void						CreateControlPane(CWnd *pWnd, CString strWndName, CString mode);

	void						CreateBmpPane(CWnd *pParent, CString strWndName, CString mode);

	void						CreateWndPane(CWnd *pWnd, CString strPaneName, bool bAppStart);

	void						CreateSeperatepanel();
	void						AddBkParameter(HWND hWnd, CString parameter, CString value);

	CString						GetWndParameter(HWND hWnd, CString parameter);
	void						UpdateLayout(HWND hWnd);

	bool						AddLayer(CWnd* pMainWnd , CWnd *pWnd, CString paneName, CString layerName); 

	virtual bool				OnChangeSkinTemplate(CString strPath,CString strSub);

	virtual void				OnchangeColor( COLORREF acolCol, UINT anLightValue,bool bChangeTop );
	virtual void				OnChangeSubject(CString strSubName);

	void						LoadColors();

	void						LoadChangeColorData();
	void						InsertSubjectData(CString name);
	void						DeleteSubjectData(CString name);
	void                        LoadChangeSubjectData();
	BOOL						IsSplitterOpen(CString name);

	void						SetSplitterStatus(CString name, BOOL bClose);

	BOOL						GetPaneItemRect(CWnd *pWnd, CString subName ,CRect &rc);

	BOOL						SetBkAttrib(CWnd *pWnd, CString paneName, CString name, CString value, bool bRedraw);
	BOOL						SetPaneSize(CWnd *pWnd, CString paneName, CRect rc, bool bAffactParent = true);

	void						SetNodeValue(CString tag, CString nameValue ,CString attrib, CString attribvalue);
	void						GetNodeValue(CString tag, CString nameValue ,CString attrib, CString& attribvalue);
	bool						IsChangeColor(){return m_bJustLoadColor;};
	COLORREF					GetSkinColor();

	void						CreateLoadSubjectTread();
	static LPVOID				LoadSubjectProc(LPVOID lparg);

	void						WaitforLoadSub();
private:
	void						NotifyLayerChanged(CString paneName, CString oldLayerName, CString newLayerName);

	void						NotifySkinChanged(CString oldPath, CString newPath);

	void						NotifyColorChanged();

	void						NotifyColorChanging();

	void						NotifyUpdateLayout(HWND m_hWnd);

	bool						LocateNode(CString wndName, CString controlName, CString strKey = "", CString strValue="");
	void						NotifyLayoutInitFinished();

	void						NotifhyLayoutInitBegin();

	Displaymode					GetDisplayMode();

	CString						GetWndPanelMode(CWnd *pWnd);

	BOOL						ShowPane(CWnd *pWnd, CString paneName, bool bShow);

};
