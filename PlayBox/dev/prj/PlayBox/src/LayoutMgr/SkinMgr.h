#pragma once
#include "skin.H"
#include "../gui/CommonControl/BasicSkinWnd.h"

#include <string>
#include <vector>
#include <map>
using std::map;
using std::string;
using std::vector;

class CSkinMgr : public CObject,
				 public ISkinMgr
{
private:
	struct PicPackHeader
	{
		char version[32];
	};
	struct PicPack
	{
		char name[128];
		char fileName[128];
		int  piclength;
	};

public:
	CSkinMgr();
	virtual ~CSkinMgr();

	CString				m_strSkinPath;
	string              m_strSub;
	CMarkupArchive		m_skinxml;
	DIBBMP_MAP			m_dibbmpmap;	
	DIBBMP_MAP			m_dibOrgBmpMap;	
	BOOL				m_bFromPackage;
	_ChangeColor		m_ChangeColorData;
	SubjectData         m_subjectData;
	void				ClearMap(bool bDelete = true);

private:
	//CSkin*	m_pSkin;	
	char*	m_ptr;	
	vector<CBasicSkinWnd*> m_vecSkinWnd;
	CDibBitmap*		GetDibBmp1(CString name, bool bCheckExist);
public:
	COLOR_MAP		m_clrMap;
	BOOL			m_bCloseMsgBox;
	BOOL			LoadSkinFromPackage( const char* pszSkinPath );
	void			LoadCommonSkin();
	void			LoadSkinFromPath( const char* pszSkinPath );
	void			LoadPictures(CString path, BOOL bDontChangeColor=false);
	void			PutDibBmp(CString name, CDibBitmap *pDib);
	bool            SubjectToSkin(string strName);
	bool			IsToppanelBtnWhite();

	CString			GetSkinPath()
	{
		return m_strSkinPath;
	};

	void			SetSkinPath(CString path)
	{ 
		m_strSkinPath = path; 
	};

	CDibBitmap *	PutDibBmp(CString name, CString path);
	CDibBitmap*		GetDibBmp( const char* pszBitmapName, bool bCheckExist );
	CDibBitmap*		GetDibBmp( const char* pszBitmapName);
	bool			GetDibBmpSubject( const char* pszBitmapName,const string& strSkinName);
	COLORREF		GetColor(const char* pszColrName);
	void			ChangeSkinColor(bool bChangeTop = true );
	void			AddSkinWnd(CBasicSkinWnd* pWnd);
	void			RemoveSkinWnd(CBasicSkinWnd* pWnd);
	COLOR_MAP *		GetColorMap(){return &m_clrMap;};
	void			PutColor(CString name, COLORREF color);
	void			SetChangeColorSign(CString name, BOOL bDontChangeColor);
	HRGN			GetFrameRGN(int cx,int cy,CDC* dc);
	_ChangeColor*	GetBaseColors(){return &this->m_ChangeColorData;};
	SubjectData*    GetSubject(){return &this->m_subjectData; }  
	OneSubject*		GetCurSubject();
	OneSubject*		GetOneSubject(const CString& str);
	COLORREF		GetSubjectColor(const CString& strSubName);
	void			LoadWndsSkin();
	bool			IsNeedDownSub(const CString& strSubName);
	void			SubDownloaded(const CString& strSubName);
	void			GetSkinNames(vector<string>& vecSkinNames,bool bCanChange);
	bool			IsSkinVersionValid(const CString& strSkinName);	
};