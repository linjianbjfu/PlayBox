#ifndef _I_SKIN_MGR_H
#define _I_SKIN_MGR_H

#include "../gui/Bitmap/DibBitmap.h"
#include "../gui/CommonControl/BasicSkinWnd.h"
#include  <vector>
using namespace std;

struct basecolor{
	string name;
	int hue;
	int sat;
	int light;
};

struct _ChangeColor{
	int baseHue;
	vector<basecolor> vecBaseColors;
};

struct OneSubject
{
	string  strDirName;
	string  strSubName;
	string  strShowName;
	bool	bShowWhite;		
	COLORREF subColor;	
	int		iLight;
	double	dSystemPos;
	int		iPos;
	bool	bNeedDown;
	POINT	ptLeftTop;
	POINT	ptRightTop;
	POINT	ptLeftBottom;
	POINT	ptRightBottom;
	unsigned int nID;
};
struct SubjectData
{
	bool bHasSub;
	string strCurSubject;
	vector<OneSubject> vecSubject;
};

class IChangeSkinEventListener
{
public:
	virtual bool OnChangeSkinTemplate(CString path,CString strSub=""){return false;};
	virtual void OnchangeColor( COLORREF acolCol, UINT anLightValue,bool bChangeTop ){};
	virtual void OnChangeSubject(){};
};

class CDibBitmap;
class ISkinMgr
{
public:
	virtual		CDibBitmap*		GetDibBmp( const char* pszBitmapName ) = 0;
	virtual		CDibBitmap*		GetDibBmp( const char* pszBitmapName , bool bCheckExist) = 0;
	virtual		COLORREF		GetColor( const char* pszColrName) = 0;
	virtual		void			SetSkinPath(CString path) = 0;
	virtual		void			ChangeSkinColor(bool bChangeTop = true)=0;
	virtual		void			AddSkinWnd(CBasicSkinWnd*)=0;
	virtual		void			RemoveSkinWnd(CBasicSkinWnd* pWnd)=0;
	virtual		HRGN			GetFrameRGN(int cx,int cy,CDC* dc)=0;
	virtual		_ChangeColor* 	GetBaseColors()=0;
	virtual     SubjectData*    GetSubject()=0;  
	virtual		OneSubject*		GetCurSubject()=0;
	virtual		OneSubject*		GetOneSubject(const CString& str) = 0;
	virtual     COLORREF		GetSubjectColor(const CString&) =  0;
	virtual		void			LoadWndsSkin()=0;
	
	virtual     bool			SubjectToSkin(string strName)=0;
	virtual     bool			IsToppanelBtnWhite() = 0;

	virtual		CString			GetSkinPath()=0;
	virtual		bool			IsNeedDownSub(const CString& strSubName) = 0;
	virtual		void			SubDownloaded(const CString& strSubName) = 0;
	virtual		void			GetSkinNames(vector<string>& vecSkinNames,bool bCanChange) = 0;
	virtual		bool			IsSkinVersionValid(const CString& strSkinName) = 0;
};
#endif