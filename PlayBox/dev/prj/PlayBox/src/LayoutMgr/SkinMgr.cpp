// Skin.cpp : 实现文件
//

#include "stdafx.h"
#include "SkinMgr.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"
#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
#include <algorithm>

#include "../util/MarkupArchive.h"
#include "SkinVersion.h"

// CSkin

CSkinMgr::CSkinMgr():m_ptr(NULL)
{	
}

CSkinMgr::~CSkinMgr()
{
	ClearMap();
	if(m_ptr != NULL)
	{
		delete m_ptr;
		m_ptr = NULL;
	}
	//if( m_pSkin != NULL )
	//{
	//	delete m_pSkin;
	//}
}

BOOL CSkinMgr::LoadSkinFromPackage( const char* pszSkinPath)
{
	CFile f;
	int len;

//	ClearMap();

	if(m_ptr != NULL)
	{
		delete m_ptr;
		m_ptr = NULL;
	}

	CString path = pszSkinPath;

	if(!f.Open(path + "\\skin.dat", CFile::modeRead))
		return false;

	PicPackHeader header;
	f.Read(&header, sizeof(header));

	len = (int)f.GetLength() - sizeof(header);

	if(len <=0)
		return false;
	m_ptr = new char[len];

	if(f.Read(m_ptr, len) != len)
	{
		f.Close();
		return false;
	}
	
	PicPack *pPack;
	int pos = 0;
	while(pos < len)
	{
		pPack = (PicPack *)(m_ptr + pos);

	
		CString key = pPack->name;
		key = key.MakeLower();
		if( (key.GetAt(0) == '_')&&  ( (key.GetAt(1)== '2')||(key.GetAt(1)== '3')||(key.GetAt(1)== '4')||(key.GetAt(1)== '5')) )
		{
			CString c, c1; 
			c = key.Mid(1,2);
			int nCount = atoi( c );
			
			if(nCount > 9)
				key.Delete(0,3);
			else
				key.Delete(0,2);

			c1 = key;
			for( int i = 0; i< nCount; i++ )
			{

				if(nCount > 5)
				{
					key.Format("%s%d", c1, i+1);
				}
				else if( i == 0 )
				{
					key = c1+"normal";
				}
				else if(  i == 1 )
				{
					key = c1+"over";
				}
				else if(  i == 2 )
				{
					key = c1+"down";
				}
				else if(  i == 3 )
				{
					key = c1+"disable";
				}
				else if(  i == 4 )
				{
					key = c1+"select";
				}

				CDibBitmap *pdib;//
				pdib = GetDibBmp(key, false);
				if(pdib == NULL)
				{
					pdib = new CDibBitmap();
				}


				CDibBitmap::STRUCT_CUTRECT cutRect;
				cutRect.rect = NULL;
				cutRect.count = nCount;
				cutRect.index = i;
				CString str =path;
				str +=  "\\";
				pdib->LoadFromMem(str+key, m_ptr + pos + sizeof(PicPack),pPack->piclength, &cutRect );
				m_dibbmpmap.insert(DIBBMP_MAP::value_type(key,pdib));
				pdib->is_Loaded = true;

				if(m_strSkinPath != pszSkinPath)
					pdib->is_ChangeColor = true;
			}



			pos += sizeof(PicPack) + pPack->piclength;
		}
		else
		{
		CDibBitmap *pdib;//
		pdib = GetDibBmp(pPack->name, false);
		if(pdib == NULL)
		{
			pdib = new CDibBitmap();
		}

		pdib->AttachMemory(m_ptr + pos + sizeof(PicPack) + sizeof(BITMAPFILEHEADER));
		pdib->is_Loaded = true;

		if(m_strSkinPath != pszSkinPath)
			pdib->is_ChangeColor = true;
		pdib->m_strName = pPack->name;
		m_dibbmpmap.insert(DIBBMP_MAP::value_type(pPack->name,pdib));
		pos += sizeof(PicPack) + pPack->piclength;
	}
	}
	f.Close();
	ClearMap(false);

	m_strSkinPath = pszSkinPath;
	return true;
}

void CSkinMgr::LoadPictures(CString path,BOOL bDontChangeColor)
{
	vector<string> vecFile;	
	vector<string> vecExe;
	vecExe.push_back("bmp");
	string strfilename;
	int count = 0;

	if( YL_DirInfo::FindFileAndDir( string(path),NULL,&vecFile,false,&vecExe ) )
	{
		for( size_t idx = 0;idx < vecFile.size();idx++ )
		{
			YL_FileInfo::GetFileNamePrefix( vecFile[idx],strfilename );	
			CString key = strfilename.c_str();
			key = key.MakeLower();

			if( (key.GetAt(0) == '_')&&  ( (key.GetAt(1)== '2')||(key.GetAt(1)== '3')||(key.GetAt(1)== '4')||(key.GetAt(1)== '5')) )
			{
				//const  char*  c = key.GetAt(1);
				CString c, c1; 
				c = key.Mid(1,2);
				int nCount = atoi( c );

				if(nCount > 9)
					key.Delete(0,3);
				else
					key.Delete(0,2);

				c1 = key;
				for( int i = 0; i< nCount; i++ )
				{

					if(nCount > 5)
					{
						key.Format("%s%d", c1, i+1);
					}
					else if( i == 0 )
					{
						key = c1+"normal";
					}
					else if(  i == 1 )
					{
						key = c1+"over";
					}
					else if(  i == 2 )
					{
						key = c1+"down";
					}
					else if(  i == 3 )
					{
						key = c1+"Disable";
					}
					else if(  i == 4 )
					{
						key = c1+"Select";
					}


					CDibBitmap *pdib;//
					pdib = GetDibBmp(key, false);
					if(pdib == NULL)
					{
						pdib = new CDibBitmap();
						count ++;
					}


					CDibBitmap::STRUCT_CUTRECT cutRect;
					cutRect.rect = NULL;
					cutRect.count = nCount;
					cutRect.index = i;
					pdib->LoadFromFile( vecFile[idx].c_str(),&cutRect );

					pdib->is_Loaded = true;
					if(m_strSkinPath != path)
						pdib->is_ChangeColor = true;
					if(bDontChangeColor)
						pdib->is_ChangeColor = false;
					pdib->m_strName = key.MakeLower();
					m_dibbmpmap.insert(DIBBMP_MAP::value_type( key.MakeLower(),pdib));
				}

			}
			else
			{

				//	CDibBitmap* pDibBitMap = new CDibBitmap();
				CDibBitmap *pdib;//
				pdib = GetDibBmp(key, false);
				if(pdib == NULL)
				{
					pdib = new CDibBitmap();
					count ++;
				}
				pdib->LoadFromFile( vecFile[idx].c_str() );
				pdib->is_Loaded = true;
				if(m_strSkinPath != path)
					pdib->is_ChangeColor = true;
				if(bDontChangeColor)
					pdib->is_ChangeColor = false;
				pdib->m_strName = key.MakeLower();
				m_dibbmpmap.insert(DIBBMP_MAP::value_type( key.MakeLower(),pdib));
			}
		}
	}
	if(count > 3)
		m_bCloseMsgBox = true;

}
void CSkinMgr::LoadCommonSkin()
{
	char			szHomePath[512];
	CString			strSkinPath;
	vector<string>	vecDir;

	CLhcImg::GetHomePath(szHomePath, 512);
	strSkinPath = szHomePath;

	strSkinPath += "\\skin\\CommonSkin\\";
	YL_DirInfo::GetSubDirNames(string(strSkinPath),vecDir);

	for(int i = 0 ; i<vecDir.size(); i++)
	{
		LoadPictures(strSkinPath + vecDir[i].c_str(), true);
	}
}

void CSkinMgr::LoadSkinFromPath( const char* pszSkinPath )
{
	m_bCloseMsgBox = false;
	ITER_DIBBMP_MAP iter = m_dibbmpmap.begin();
	while(iter!=m_dibbmpmap.end())
	{
		CDibBitmap * d = iter->second;
		((CDibBitmap*)iter->second)->is_Loaded = false;
		((CDibBitmap*)iter->second)->has_PopupError = false;
		iter++;
	}
	LoadSkinFromPackage(pszSkinPath);

	//读取通用图片
	//LoadCommonSkin();
		
	//读取皮肤目录中的零散图片
	LoadPictures(pszSkinPath);

	ClearMap(false);

	m_strSkinPath = pszSkinPath;
}

bool  CSkinMgr::SubjectToSkin(string strName)
{
	int nsize = AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->vecSubject.size();
	for( int index=0; index<nsize; index++)
	{
		string str =  AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->vecSubject[index].strSubName;
		if(str ==strName)
		{					
			bool bRet = GetDibBmpSubject("TopPanelBKMid",strName);
			if( !bRet )
			{
				return false;
			}

			bRet = GetDibBmpSubject("TopPanelBKLeft",strName);
			if( !bRet )
			{
				return true;
			}

			bRet = GetDibBmpSubject("TopPanelBKRight",strName);
			if( !bRet )
			{
				return true;
			}
				
			return true;
		}
	}

	return false;
}
void CSkinMgr::ClearMap(bool bDelete)
{
	if(!bDelete)
		return;
	////清空map中已经载入的对象
	ITER_DIBBMP_MAP iter = m_dibbmpmap.begin();
	while(iter!=m_dibbmpmap.end())
	{
		if(bDelete)
		{	
			((CDibBitmap*)iter->second)->Empty();
			delete iter->second;
		}
		else if(!iter->second->is_Loaded)
		{
			CDibBitmap * d = iter->second;
			((CDibBitmap*)iter->second)->Empty();
			delete iter->second;
			m_dibbmpmap.erase(iter++); 
			continue;
		}
		iter++;
	}
	if(bDelete)
		m_dibbmpmap.clear();
}

void CSkinMgr::PutDibBmp(CString name, CDibBitmap *pDib)
{
	pDib->m_strName = name.MakeLower();
	m_dibbmpmap.insert(DIBBMP_MAP::value_type(name.MakeLower(),pDib));
}

CDibBitmap * CSkinMgr::PutDibBmp(CString name, CString path)
{
	CDibBitmap *pdib = new CDibBitmap();
	if(pdib->LoadFromFile(path + name + ".bmp"))
	{
		pdib->m_strName = name.MakeLower();
		m_dibbmpmap.insert(DIBBMP_MAP::value_type(name.MakeLower(),pdib));
	}
	else{
		delete pdib;
		pdib = NULL;
	}
	return pdib;
}

CDibBitmap* CSkinMgr::GetDibBmp1(CString name , bool bCheckExist)
{
	ITER_DIBBMP_MAP iter = m_dibbmpmap.find(name.MakeLower());
	if(iter == m_dibbmpmap.end())
	{
		if(bCheckExist)
		{
#ifdef _DEBUG
			if(!m_bCloseMsgBox)
				;//AfxMessageBox("图片 " + name  + " 无效");
#endif
		}
		return NULL;
	} 
	else
	{
		return iter->second;
	}
}
bool CSkinMgr::GetDibBmpSubject( const char* pszBitmapName,const string& strSkinName)
{
	string strPathd = m_strSkinPath;
	strPathd += "\\SbujectSkin";
	strPathd += "\\";
	strPathd += strSkinName;
	strPathd += "\\";
	strPathd += pszBitmapName;
	strPathd += ".bmp";

	CDibBitmap *pBmp = new CDibBitmap();			
	if( !pBmp->LoadFromFile(strPathd.c_str()) )
	{
		delete pBmp;
		pBmp = NULL;

		return false;
	}

	ITER_DIBBMP_MAP iter = m_dibbmpmap.find(CString( pszBitmapName).MakeLower());
	if( iter != m_dibbmpmap.end() && pBmp !=NULL )
	{
		pBmp->is_ChangeColor	= (*iter).second->is_ChangeColor;
		pBmp->is_Loaded			= (*iter).second->is_Loaded;
		pBmp->m_strName			= (*iter).second->m_strName;
		delete (*iter).second;
		(*iter).second = pBmp;
	}
	else
	{
		return false;
	}

	return true;
}
CDibBitmap* CSkinMgr::GetDibBmp( const char* pszBitmapName, bool bCheckExist )
{
	return GetDibBmp1( CString( pszBitmapName).MakeLower() ,bCheckExist);
}

CDibBitmap* CSkinMgr::GetDibBmp( const char* pszBitmapName )
{
	return GetDibBmp1( CString( pszBitmapName).MakeLower() ,true);
}

void CSkinMgr::ChangeSkinColor( bool bChangeTop )
{
	ITER_DIBBMP_MAP iter = m_dibbmpmap.begin();
	int count = 0;
	while(iter!=m_dibbmpmap.end())
	{
		if ((CDibBitmap*)(iter->second)->is_ChangeColor && (CDibBitmap*)(iter->second)->is_Loaded)
		{
			CString name = (iter->first);

			if(name=="esc" ||
				name.Find("userinfolev") != -1)
			{
				iter++;
				count ++;
				continue;
			}

			if( !bChangeTop )
			{
				if( name == "toppanelbkleft" || 
					name == "toppanelbkmid" ||
					name == "toppanelbkright"
					)
				{
					iter++;
					count++;
					continue;
				}
			}

			CDibBitmap* d = (CDibBitmap*)(iter->second);
			AfxGetUIManager()->UIGetChangeColorMgr()->ChangeBmp(
				DIBINFO(((CDibBitmap*)iter->second)->m_lpvColorTable,
				(UINT)(((CDibBitmap*)iter->second)->m_lpBMIH->biWidth), 
				(UINT)(((CDibBitmap*)iter->second)->m_lpBMIH->biHeight),
				(UINT)(((CDibBitmap*)iter->second)->m_lpBMIH->biBitCount),
				0),
				0,
				RGB(255,0,0)
				);
		}
		else
			TRACE(" Don't change color: %s\r\n", (iter->first));
		iter++;
		count ++;
	}

	LoadWndsSkin();
}

void CSkinMgr::LoadWndsSkin()
{	
	for(size_t idx =0 ;idx < m_vecSkinWnd.size();idx++ )
	{
		CBasicSkinWnd* pWnd = m_vecSkinWnd[idx];
		m_vecSkinWnd[idx]->LoadSkin();
	}
}

void CSkinMgr::AddSkinWnd(CBasicSkinWnd* pWnd)
{
	m_vecSkinWnd.push_back(pWnd);
}

void CSkinMgr::RemoveSkinWnd(CBasicSkinWnd* pWnd)
{
//By Haiping,268,多次add只remove一次导致this->LoadWndsSkin()试图调用已经析构的窗口类成员函数而崩溃
	m_vecSkinWnd.erase(remove(m_vecSkinWnd.begin(),m_vecSkinWnd.end(),pWnd),m_vecSkinWnd.end());
/*
	vector<CBasicSkinWnd*>::iterator it;
	for(it = m_vecSkinWnd.begin(); it != m_vecSkinWnd.end(); ++it)
	{
		if((*it) == pWnd)
		{
			m_vecSkinWnd.erase(it);
			break;
		}
	}
*/
}

COLORREF CSkinMgr::GetColor(const char* pszColrName)
{
	CString name = pszColrName;
	ITER_COLOR_MAP iter = m_clrMap.find(name.MakeLower());

	if(iter == m_clrMap.end())
		return RGB(0,0,0);
	else
		return iter->second;
	return NULL;
}

void CSkinMgr::PutColor(CString name, COLORREF color)
{
	m_clrMap.insert(COLOR_MAP::value_type(name.MakeLower(),color));
}

void CSkinMgr::SetChangeColorSign(CString name, BOOL bDontChangeColor)
{
	CDibBitmap * bmp = this->GetDibBmp(name,false);

	if(bmp != NULL)
		bmp->is_ChangeColor = !bDontChangeColor;
}

HRGN CSkinMgr::GetFrameRGN(int cx,int cy,CDC* dc)
{	
	//左上角
	CDibBitmap* m_bFrameTopLeftMask = GetDibBmp("FrameTopLeftMask");
	m_bFrameTopLeftMask->SetCDibRect(CRect(0,0,m_bFrameTopLeftMask->GetWidth(),m_bFrameTopLeftMask->GetHeight()));
	HRGN rgnTopLeftRound=  m_bFrameTopLeftMask->CreateRgnFromBitmap( RGB(0xff,0x00,0x00),dc);
	int topLeftRountCx = m_bFrameTopLeftMask->GetWidth();
	int topLeftRountCy = m_bFrameTopLeftMask->GetHeight();	
	HRGN rgnTopLeftRect = ::CreateRectRgn(0,0,topLeftRountCx,topLeftRountCy);
	HRGN rgnTopLeft = ::CreateRectRgn(0,0,0,0);
	::CombineRgn(rgnTopLeft,rgnTopLeftRect,rgnTopLeftRound,RGN_DIFF);

	//右上角
	CDibBitmap* m_bFrameTopRightMask = GetDibBmp("FrameTopRightMask");
	m_bFrameTopRightMask->SetCDibRect(CRect(0,0,m_bFrameTopRightMask->GetWidth(),m_bFrameTopRightMask->GetHeight()));
	HRGN rgnTopRightRound=  m_bFrameTopRightMask->CreateRgnFromBitmap( RGB(0xff,0x00,0x00),dc);
	int topRightRountCx = m_bFrameTopRightMask->GetWidth();
	int topRightRountCy = m_bFrameTopRightMask->GetHeight();
	HRGN rgnTopRightRect = ::CreateRectRgn(0,0,topRightRountCx,topRightRountCy);
	HRGN rgnTopRight= ::CreateRectRgn(0,0,0,0);
	::CombineRgn(rgnTopRight,rgnTopRightRect,rgnTopRightRound,RGN_DIFF);

	//左下角
	CDibBitmap* m_bFrameBottomLeftMask = GetDibBmp("FrameBottomLeftMask");
	m_bFrameBottomLeftMask->SetCDibRect(CRect(0,0,m_bFrameBottomLeftMask->GetWidth(),m_bFrameBottomLeftMask->GetHeight()));
	HRGN rgnBottomLeftRound=  m_bFrameBottomLeftMask->CreateRgnFromBitmap( RGB(0xff,0x00,0x00),dc);
	int bottomLeftRountCx =m_bFrameBottomLeftMask->GetWidth();
	int bottomLeftRountCy =m_bFrameBottomLeftMask->GetHeight();
	//HRGN rgnBottomLeftRound = ::CreateRoundRectRgn(0,0,bottomLeftRountCx,bottomLeftRountCy,bottomLeftRountCx,bottomLeftRountCy);
	HRGN rgnBottomLeftRect =::CreateRectRgn(0,0,bottomLeftRountCx,bottomLeftRountCy); 
	HRGN rgnBottomLeft= ::CreateRectRgn(0,0,0,0);
	::CombineRgn(rgnBottomLeft,rgnBottomLeftRect,rgnBottomLeftRound,RGN_DIFF);	

	//右下角
	CDibBitmap* m_bFrameBottomRightMask = GetDibBmp("FrameBottomRightMask");
	m_bFrameBottomRightMask->SetCDibRect(CRect(0,0,m_bFrameBottomRightMask->GetWidth(),m_bFrameBottomRightMask->GetHeight()));
	HRGN rgnBottomRightRound=  m_bFrameBottomRightMask->CreateRgnFromBitmap( RGB(0xff,0x00,0x00),dc);
	int bottomRightRountCx = m_bFrameBottomRightMask->GetWidth();
	int bottomRightRountCy = m_bFrameBottomRightMask->GetHeight();
	HRGN rgnBottomRightRect = ::CreateRectRgn(0,0,bottomRightRountCx,bottomRightRountCy);
	HRGN rgnBottomRight= ::CreateRectRgn(0,0,0,0);
	::CombineRgn(rgnBottomRight,rgnBottomRightRect,rgnBottomRightRound,RGN_DIFF);

	OffsetRgn(rgnTopRight,cx-topRightRountCx,0);
	OffsetRgn(rgnBottomLeft,0,cy-bottomLeftRountCy);
	OffsetRgn(rgnBottomRight,cx-bottomRightRountCx,cy-bottomRightRountCy);
	
	HRGN rgn = CreateRectRgn(0,0,cx,cy);
	::CombineRgn(rgn,rgn,rgnTopLeft,RGN_DIFF);
	::CombineRgn(rgn,rgn,rgnTopRight,RGN_DIFF);
	::CombineRgn(rgn,rgn,rgnBottomLeft,RGN_DIFF);
	::CombineRgn(rgn,rgn,rgnBottomRight,RGN_DIFF);

	DeleteObject(rgnTopLeft);
	DeleteObject(rgnTopLeftRound);
	DeleteObject(rgnTopLeftRect);

	DeleteObject(rgnTopRight);
	DeleteObject(rgnTopRightRound);
	DeleteObject(rgnTopRightRect);

	DeleteObject(rgnBottomLeft);
	DeleteObject(rgnBottomLeftRound);
	DeleteObject(rgnBottomLeftRect);

	DeleteObject(rgnBottomRightRound);
	DeleteObject(rgnBottomRightRect);
	DeleteObject(rgnBottomRight);

	return rgn;
}

bool CSkinMgr::IsToppanelBtnWhite()
{
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName == m_subjectData.strCurSubject )
		{
			return oneSub.bShowWhite;
		}
	}
	return false;
}

OneSubject* CSkinMgr::GetCurSubject()
{
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName == m_subjectData.strCurSubject )
		{
			return &oneSub;
		}
	}
	return NULL;
}

OneSubject* CSkinMgr::GetOneSubject(const CString& str)
{
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName.c_str() == str )
		{
			return &oneSub;
		}
	}
	return NULL;
}

COLORREF CSkinMgr::GetSubjectColor(const CString& strSubName)
{
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName == m_subjectData.strCurSubject )
		{
			return oneSub.subColor;
		}
	}
	return 0;
}


bool CSkinMgr::IsNeedDownSub(const CString& strSubName)
{	
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName.c_str() == strSubName )
		{
			return oneSub.bNeedDown;
		}
	}
	return true;
}

void CSkinMgr::SubDownloaded(const CString& strSubName)
{
	for(size_t idx = 0;idx < m_subjectData.vecSubject.size();idx++ )
	{
		OneSubject& oneSub = m_subjectData.vecSubject[idx];
		if( oneSub.strSubName.c_str() == strSubName )
		{
			oneSub.bNeedDown = false;
			break;
		}
	}	
}

static string find_SkinName(const string& strName)
{
//	static const s_iSkinSize = sizeof(SKIN_VERSION) / sizeof(SKIN_VERSION[0]);
	
	int iS = sizeof( SKIN_VERSION );
	int iOneS = sizeof( SKIN_VERSION[0] );

	if( iOneS > 0 )
	{
		for(int i = 0 ;i < iS / iOneS ;i=i+2 )
		{
			if( SKIN_VERSION[i] == strName )
			{
				return SKIN_VERSION[i+1];
			}
		}
	}

	return "";
}

void CSkinMgr::GetSkinNames(vector<string>& vecSkinNames,bool bCanChange)
{
}

bool CSkinMgr::IsSkinVersionValid(const CString& strSkinName)
{
	char home[512];
	string strSkinPath;
	CLhcImg::GetHomePath(home, 512);

	strSkinPath = string(home) + "\\skin\\" + (LPCSTR)strSkinName;
	
	char szVersion[64];
	CLhcImg::GetSoftwareVersion(szVersion,64);
	string strSoftVersion = szVersion;
	string strver2 = "";

	size_t first = strSoftVersion.find("_");
	if( first != string::npos )
	{
		size_t end	 = strSoftVersion.find("_",first+1);
		if( end != string::npos )
		{
			strver2 = strSoftVersion.substr( first + 1,end-first-1);
		}
	}

	if( strver2 != "" )
	{
		size_t nPos = strver2.find_last_of(".");
		if( nPos != string::npos )
		{
			strver2 = strver2.substr(0,nPos);
		}
	}
	else
	{
		strver2 = strSoftVersion;
	}

	CMarkupArchive xml;
	string strXMLFile;
	GlobalFunc::GetSkinConfPath( strXMLFile );

	if( !xml.Open( strXMLFile.c_str() ) )
	{
		return false;
	}

	xml.ResetPos();
	if( xml.FindElem("root") )
	{
		string strVerion = xml.GetAttrib("ver");
		string strSupportVer;
		if( (strSupportVer = find_SkinName( string(strSkinName) ) ) != "" )
		{
			if( strSupportVer == strVerion )
			{
				return true;
			}
		}	
		else
		{
			if( strVerion == strver2 )
			{
				return true;
			}
		}
	}

	xml.Close();

	return false;
}
