#pragma once
#include   "stdafx.h"  
#include "../ISkinMgr.h"
#include <string>
#include "BitMapOperate.h"
#include "MainSkinWnd.h"
#include "SkinWnd.h"
#include "SubjectChildWnd.h"
#include "SkinBotton.h"
#include "IEvent.h"
#include "DownloadPercentWnd.h"
#include "YL_P2PInt.h"
#include "ILayoutChangeObserver.h"
#include "WebSubWnd.h"

using namespace std;

#define WM_SKIN_DOWNLOAD_FINISHED	WM_USER + 2328
#define WM_SKIN_DOWNLOAD_FAILED 	WM_USER + 2329

#define ID_WND_WEBSUBJECT			10040

class IChangeColorMgr :public IWndEventListener,public IEventListener
{
public:
	virtual bool	CloseSkinWnd()=0;
	virtual BOOL	PopSkinWnd(CWnd* apParent, UINT anID, POINT aPoint) = 0;
	virtual CWnd*	GetSkinWnd() = 0;
	virtual	bool	InsertSmallSkin(string astrName,CBitmap* apBmp,string strpath,bool bHasSkin=false)           = 0;
	virtual	bool	InsertSmallSubjectSkin(string astrName,CBitmap* apBmp,string strpath,bool bHasSkin=false)    = 0;
	
	virtual DIBINFO* ChangeBmp(DIBINFO ahSourceDib,UINT aColorGroup, COLORREF aMaskColor) = 0;
	virtual void     ChangeColor(COLORREF &aColor) = 0 ;
	virtual void     SetTipText( CString astrName,CString strTip) = 0;

	virtual void	SetListener( IChangeSkinEventListener* apListtener ) = 0;	
	virtual void	RemoveListener( IChangeSkinEventListener* apListtener ) = 0;	
	
	// 下面接口是为了换肤事件不带参数， 音乐盒改动小而加的
	virtual void	  OnClose() = 0;
	virtual void	  BeforeChangeColor() = 0;
	virtual void	  AfterChangeColor() = 0;
	//增加通用接口
	virtual void	SetTopMost(bool bTopMost) = 0;
	virtual void    SetSelectedColor(COLORREF aColor) = 0;
	virtual void    DeleteObject() = 0;
	//下载进度窗口
	virtual void    DownloadStart() =0;
	virtual void    SetDownloadPercentage(int percentage,CString strSkinName="") = 0;
	virtual void    DownloadFinish(CString strSkinName) = 0;
	virtual void	NotifyChangeSubject(const CString& strSbName) = 0;
	virtual void	ClickSub(const CString& str) = 0;	
	virtual void	DownloadWebSub(const CString& strUrl,const CString& strid) = 0;
};

class CSkinMgrV1 :	public IChangeColorMgr, 
					public P2PObserver,
					public ILayoutChangeObserver
{
private:

	CMainSkinWnd*			mp_mainSkinWnd;
	CSkinWnd*				mp_skinWnd;	
	CSubjectChildWnd*		mp_SubjectWnd;
	CWebSubWnd*				mp_WebSubWnd;

	CDownloadPercentWnd*	mp_downloadSkin;
	CDownloadPercentWnd*	mp_downloadSubject;		

	COLORREF      m_BaseColor;
	COLORREF      m_DefaultColor;
	UINT          m_nLightPos;
	CString		  m_strSkinPath;

	vector<IChangeSkinEventListener*>  mp_changeSkinEven;

	static IChangeColorMgr* mp_IsHadObject;
	bool   mb_HasPopWnd;

	bool	mb_isDownSkin;
	bool	mb_isDownSubject;
	bool	mb_isDownWebSub;
	bool	mb_isFirstPop;

	Sign m_sgDownload;
	CString m_strDownSkin;
	CString m_strDownPath;
	CString m_strDownSubName;
	bool	m_bShowSkinWnd;
	CRITICAL_SECTION	m_csDownInfo;
	int m_iCheckTab;
	
public:
	static IChangeColorMgr* GetObject();
	virtual CWnd*	GetSkinWnd(){return mp_mainSkinWnd;};
public:
	
	CSkinMgrV1();
	virtual ~CSkinMgrV1();

	bool	CloseSkinWnd();
	BOOL	PopSkinWnd(CWnd* apParent, UINT anID, POINT aPoint);
	bool InsertSmallSkin(string astrName,CBitmap* apBmp,string strPath,bool bHasSkin=false);
    bool InsertSmallSubjectSkin(string astrName,CBitmap* apBmp,string strpath,bool bHasSkin=false);	
	void SetTipText( CString astrName,CString strTip);

	DIBINFO* ChangeBmp(DIBINFO ahSourceDib,UINT aColorGroup, COLORREF aMaskColor) ;	
	void  ChangeColor(COLORREF &aColor);
	
	void	SetListener( IChangeSkinEventListener* apListtener );
	void	RemoveListener( IChangeSkinEventListener* apListtener );
	void	OnClose();

	void	SetSelectedColor(COLORREF aColor);
	void	BeforeChangeColor();
	void    AfterChangeColor();
	//增加通用接口
	void	SetTopMost(bool bTopMost);
	void    DeleteObject();
	//下载进度窗口
	void    DownloadStart();
	void    SetDownloadPercentage(int percentage,CString strSkinName="");	
	void    DownloadFinish(CString strSkinName);

	void	DownloadSubStart(const CString& strSub);
	void	DownloadSubFinish(const CString& strSub,BOOL bSuc);
	void	DownloadWebSubFinish(const CString& strSub,BOOL bSuc);

	void	NotifyChangeSubject(const CString& strSbName);
	void	GetSkinWndRect(const POINT& aPoint,CRect& rect);

	void	OnTabSelChange( UINT anIndex);
	void	ClickSub(const CString& str);
	void	DownloadWebSub(const CString& strUrl,const CString& strid);

	void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);

	void	InstallWebSubject(string strSubName);
	void	InsertSubject(string strSubName);
public:
	void OnSendChangeColor( COLORREF acolCol, UINT anLightValue,bool bChange );
	void OnSendChangeSkin( CString strPath);
	void OnSendChangeSubject(CString strPath, CString subjectName);
	void OnChangeSkinAndSub(const CString& strPath,const CString& strSub);	
	void OnStartDownSubject(const CString& strSubName);
	void OnFinishDownSubject(BOOL bSuc);

	void P2POb_DownStart(const Sign& sign, const char* lpszFilePath, unsigned int unFileSize);
	void P2POb_DownFinish(const Sign& sign);
	void P2POb_DownFailed(const Sign& sign, P2P_DOWN_FAILED_REASON pdfr);
	void P2POb_DownProgress(const Sign& sign, unsigned int unContinue, unsigned int unFinished, unsigned int unSpeed);
};