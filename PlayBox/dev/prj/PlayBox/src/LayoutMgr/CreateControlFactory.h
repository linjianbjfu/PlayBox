#pragma once

#include  "../util/MarkupArchive.h"
#include "ILayoutMgr.h"

class CSkinMgr;
class CxSkinButton;

class CreateControlFactory
{
public:
	CreateControlFactory(void);
	~CreateControlFactory(void);

	static void CreateControl( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes);	
	static void CreateStatic( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes );
	static void CreateButton( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes );
	static void CreateButton2( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes );
	static void CreateTree(CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes);
	static void CreateBtnTree( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes );
	static void CreateList( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes );
	static void CreateSplitter( CWnd *pWnd, CMarkup *pXml, CSkinMgr *pSkin);
	static void CreateSlider( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin);
	static COLORREF StringToRGB(CString str, BOOL bChangeColor = true);
	static void SetButtonSkin(CxSkinButton *pBtn, CString name, ISkinMgr *pSkin);
};
