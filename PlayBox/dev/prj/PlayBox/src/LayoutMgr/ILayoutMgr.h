#pragma once
#include "./ISkinMgr.h"
//#include "./changecolor/IChangeColorMgr.h"
class  CSkinMgr;
enum Displaymode{
	DISPLAY_SEPERATE_MODE,
	DISPLAY_FULL_MODE,
	DISPLAY_MINI_MODE,
	WAIT_FOR_EXIT_MODE
};
class ILayoutMgr : public IChangeSkinEventListener
{
public:
	virtual bool			SetXmlPath(CString path)=0;

	virtual void			SetSkin(CSkinMgr *pSkin)=0;	//设置皮肤对象

	virtual void			UpdateLayout(HWND hWnd)=0;	//更新布局管理(在OnSize里调用)

	virtual void			AppExit()=0;
	/**
	 *	窗口布局管理接口
	 */
	virtual void			RegisterWnd(CString name, CWnd *pWnd)=0;//注册窗口

	virtual void			CreateWndPane(CWnd *pWnd, CString strPaneName, bool bAppStart)=0; 	//创建窗口panel

	virtual void			ShowLayer(CString paneName, CString layerName)=0;	//显示MultiPanel的层

	virtual CString			GetCurrentLayer(CString paneName)=0;	//得到MultiPane当前显示的层

	virtual CWnd *			GetWnd(CString name)=0;	//得到名字为name的窗口

	virtual void			SaveWndPanel(CString strWndName, CString strControlName)=0;
	/**
	 *	控件布局管理接口
	 */
	virtual void			RegisterCtrl(CWnd *pWnd, CString name, CWnd *pCtrl)=0;	//注册控件

	virtual void			CreateControlPane(CWnd *pWnd, CString strWndName, CString mode)=0;	//创建窗口的控件Panel

	virtual CWnd*			GetCtrl(HWND hWnd, CString name)=0;	//得到名字为name的控件

	virtual BOOL			IsSplitterOpen(CString name) = 0;

	virtual void			SetSplitterStatus(CString name, BOOL bClose) = 0;

	virtual BOOL			GetPaneItemRect(CWnd *pWnd, CString subName ,CRect &rc)=0;

	virtual BOOL			SetPaneSize(CWnd *pWnd, CString paneName, CRect rc, bool bAffactParent = true)=0;

	/**
	 *	背景布局管理接口
	 */
	virtual void			AddBkParameter(HWND hWnd, CString parameter, CString value)=0;

	virtual CString			GetWndParameter(HWND hWnd, CString parameter)=0;
	virtual void			CreateBmpPane(CWnd *pParent, CString strWndName, CString mode)=0;	//创建窗口的背景Panel

	virtual void			PaintBkGround(HWND hWnd , CDC *pDC, BOOL bUseMemDC = true)=0;	//绘制窗口背景（在OnPaint里调用）

	/**
	 *	 插件接口
	 */
	virtual bool			AddLayer(CWnd* pMainWnd, CWnd *pWnd, CString paneName, CString layerName)=0; 

	/**
	 *	换肤接口
	 */
//	virtual void			ChangeSkin(CString strPath)=0;

	virtual bool			OnChangeSkinTemplate(CString strPath,CString strSub="" ) = 0;
	virtual void			OnchangeColor( COLORREF acolCol, UINT anLightValue,bool bChangeTop ) = 0;
	virtual void			OnChangeSubject(CString strSubName) = 0;
	virtual void			LoadColors() = 0;
	virtual void			LoadChangeColorData() = 0;

	virtual void            LoadChangeSubjectData() =0;
	virtual void			InsertSubjectData(CString name) = 0;
	virtual void			DeleteSubjectData(CString name) = 0;
	virtual Displaymode		GetDisplayMode() = 0;

	virtual CString			GetWndPanelMode(CWnd *pWnd) = 0;

	virtual BOOL			ShowPane(CWnd *pWnd, CString paneName, bool bShow) = 0;
	virtual bool			IsChangeColor() = 0;
	virtual COLORREF		GetSkinColor() = 0;	

	virtual void			GetNodeValue(CString tag, CString nameValue ,CString attrib, CString& attribvalue) = 0;
	virtual	void			WaitforLoadSub() = 0;
};