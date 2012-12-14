#include "StdAfx.h"
#include "createcontrolfactory.h"
#include "../gui/CommonControl/xSkinButton.h"
#include "../gui/CommonControl/SkinButton2.h"
#include "../gui/CommonControl/xStaticText.h"
#include "../gui/CommonControl/MyBtnTreeCtrl.h"
#include "../gui/CommonControl/MyListCtrl.h"
#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
#include "etssplitter.h"
#include "skinmgr.h"

CreateControlFactory::CreateControlFactory(void)
{
}

CreateControlFactory::~CreateControlFactory(void)
{
}

void CreateControlFactory::CreateControl(CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes)
{
	if(!::IsWindow(pWnd->m_hWnd) || pSkin == NULL)
		return;

//	if( pWnd->IsKindOf(RUNTIME_CLASS(CxSkinButton)))
	if( dynamic_cast<CSkinButton2*>(pWnd) )
	{
		CreateButton2( pWnd,pXml,pSkin,m_mgrPanes);
	}else 
	if( dynamic_cast<CxSkinButton*>(pWnd) )
	{
		CreateButton( pWnd,pXml,pSkin,m_mgrPanes );
	}else
	if( pWnd->IsKindOf( RUNTIME_CLASS(CMyTreeCtrl) ) )
	{
		CString strType = pXml->GetAttrib("type");

		if( strType == "BtnTree")
		{
			CreateBtnTree( pWnd,pXml,pSkin,m_mgrPanes);
		}
		else
		{
			CreateTree( pWnd,pXml,pSkin,m_mgrPanes);
		}
	}
	else if( pWnd->IsKindOf( RUNTIME_CLASS(CMyListCtrl)) ) 
	{
		CString strType = pXml->GetAttrib("type");
		if( strType == "list" )
		{
			CreateList( pWnd,pXml,pSkin,m_mgrPanes );
		}
	}
	else if(pWnd->IsKindOf(RUNTIME_CLASS(ETSSplitter) ) )
	{
		//((ETSSplitter*)pWnd)->LoadSkin(pSkin);
		CreateSplitter(pWnd, pXml, pSkin);
	}
	else if (pXml->GetAttrib("type") == "static")
	{
		CreateStatic(pWnd, pXml, pSkin,m_mgrPanes);
	}
}

void CreateControlFactory::CreateStatic( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes )
{
	CxStaticText *pStatic = (CxStaticText*) pWnd;

	//int top = atoi(pXml->GetAttrib("start_y_top"));
	//int left = atoi(pXml->GetAttrib("start_x_left"));
	//int width = atoi(pXml->GetAttrib("x_delta"));
	//int height = atoi(pXml->GetAttrib("y_delta"));

	//pStatic->MoveWindow(left,top,width,height);
	//
	pStatic->SetBackgroundColor(StringToRGB(pXml->GetAttrib("background_color")));

	//if(pStatic->IsEnableClick())
		pStatic->SetTextColor(StringToRGB( pXml->GetAttrib("text_color")));

	pStatic->SetSingleLine((bool)(atoi(pXml->GetAttrib("single_line"))));
	CString strTemp = pXml->GetAttrib("under_line");
	if(!strTemp.IsEmpty())
		pStatic->SetUnderLine((bool)(atoi(strTemp)));
	strTemp = pXml->GetAttrib("enable_click");
	if(!strTemp.IsEmpty() && !m_mgrPanes->IsChangeColor())
		pStatic->EnableClick((bool)(atoi(strTemp)));

	pStatic->SetFontFace(pXml->GetAttrib("font_face"));
	pStatic->SetFontSize(atoi(pXml->GetAttrib("font_size")));

}
void CreateControlFactory::SetButtonSkin(CxSkinButton *pBtn, CString name, ISkinMgr *pSkin)
{
	CDibBitmap *pDibNormal, *pDibDown, *pDibOver, *pDibDisable, *pDibMask;

	if(name == "")
		return;
	if((pDibNormal = pSkin->GetDibBmp(name + "normal")) == NULL)
		;//pDibNormal = pSkin->PutDibBmp(name + "normal", pSkin->m_strSkinPath);

	if(pDibNormal == NULL || !pDibNormal->is_Loaded)
		return;

	if((pDibDown = pSkin->GetDibBmp(name + "down", false)) == NULL){
		;//pDibDown = pSkin->PutDibBmp(name + "down", pSkin->m_strSkinPath);
		if(pDibDown == NULL || !pDibDown->is_Loaded)
			pDibDown = pDibNormal;
	}

	if((pDibOver = pSkin->GetDibBmp(name + "over", false)) == NULL){
		;//pDibOver = pSkin->PutDibBmp(name + "over", pSkin->m_strSkinPath);
		if(pDibOver == NULL || !pDibOver->is_Loaded)
			pDibOver = pDibDown;
		else if(pDibOver == NULL)
			pDibOver = pDibNormal;
	}

	if((pDibDisable = pSkin->GetDibBmp(name + "disable", false)) == NULL){
		;//pDibDisable = pSkin->PutDibBmp(name + "disable", pSkin->m_strSkinPath);
		if(pDibDisable == NULL || !pDibDisable->is_Loaded)
			pDibDisable = pDibNormal;
	}

	if((pDibMask = pSkin->GetDibBmp(name + "mask",false)) == NULL){
		;//pDibMask = pSkin->PutDibBmp(name + "mask", pSkin->m_strSkinPath);
	}

	if(pDibMask == NULL ||pDibMask->is_Loaded)
		pBtn->SetSkin(pDibNormal, pDibDown, pDibOver, pDibDisable,0,0,0,0,0);
	else
		pBtn->SetSkin(pDibNormal, pDibDown, pDibOver, pDibDisable,0,pDibMask,0,0,0);
}

void CreateControlFactory::CreateButton2( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes )
{
	CSkinButton2* pSkinBtn2 = (CSkinButton2*)pWnd;
	CString tooltip = pXml->GetAttrib("tooltip"); 
	
	if(tooltip != "")
		pSkinBtn2->SetToolTipText(tooltip);

	CString strText = pXml->GetAttrib("text");
	
	CString strToolText;
	pSkinBtn2->GetWindowText(strToolText);
	if(strToolText == "")
		pSkinBtn2->SetWindowsText(strText);

	CString bmpPath = pXml->GetAttrib("bmp");
	string  skinPath = pSkin->GetSkinPath();
}

void CreateControlFactory::CreateButton( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes )
{
	CxSkinButton *pBtn = (CxSkinButton*) pWnd;
	CDibBitmap *pDibNormal, *pDibDown, *pDibOver, *pDibDisable, *pDibMask;

	CString tooltip = pXml->GetAttrib("tooltip"); 

	bool bDontChangeColor = atoi(pXml->GetAttrib("dont_change_color")) == 1;
	if(bDontChangeColor == true)
		int i = 0;
	CString name = pXml->GetAttrib("bmp");

	if(name == "")
		return;
	if((pDibNormal = pSkin->GetDibBmp(name + "normal")) == NULL)
		pDibNormal = pSkin->PutDibBmp(name + "normal", pSkin->m_strSkinPath);

	if(pDibNormal == NULL || !pDibNormal->is_Loaded)
		return;

	if((pDibDown = pSkin->GetDibBmp(name + "down", false)) == NULL){
		pDibDown = pSkin->PutDibBmp(name + "down", pSkin->m_strSkinPath);
		if(pDibDown == NULL || !pDibDown->is_Loaded)
			pDibDown = pDibNormal;
	}

	if((pDibOver = pSkin->GetDibBmp(name + "over", false)) == NULL){
		pDibOver = pSkin->PutDibBmp(name + "over", pSkin->m_strSkinPath);
		if(pDibOver == NULL || !pDibOver->is_Loaded)
			pDibOver = pDibDown;
		else if(pDibOver == NULL)
			pDibOver = pDibNormal;
	}

	if((pDibDisable = pSkin->GetDibBmp(name + "disable", false)) == NULL){
		pDibDisable = pSkin->PutDibBmp(name + "disable", pSkin->m_strSkinPath);
		if(pDibDisable == NULL || !pDibDisable->is_Loaded)
			pDibDisable = pDibNormal;
	}

	if((pDibMask = pSkin->GetDibBmp(name + "mask",false)) == NULL){
		pDibMask = pSkin->PutDibBmp(name + "mask", pSkin->m_strSkinPath);
	}

	pSkin->SetChangeColorSign(name+"normal", bDontChangeColor);
	pSkin->SetChangeColorSign(name+"over", bDontChangeColor);
	pSkin->SetChangeColorSign(name+"down", bDontChangeColor);
	pSkin->SetChangeColorSign(name+"disable", bDontChangeColor);

	CString value;
	if(pXml->GetAttrib("x_delta") == "0")
	{
		value.Format("%d", pDibNormal->GetWidth());
		pXml->SetAttrib("x_delta", value);
	}
	if(pXml->GetAttrib("y_delta") == "0")
	{
		value.Format("%d", pDibNormal->GetHeight());
		pXml->SetAttrib("y_delta", value);
	}

	if(pDibMask == NULL ||pDibMask->is_Loaded)
		pBtn->SetSkin(pDibNormal, pDibDown, pDibOver, pDibDisable,0,0,0,0,0);
	else
		pBtn->SetSkin(pDibNormal, pDibDown, pDibOver, pDibDisable,0,pDibMask,0,0,0);

	CString strTooltip = pBtn->GetToolTipText();
	if(strTooltip == "")
		pBtn->SetToolTipText(tooltip);

	CString strColor;
	CString strNormalColor;
	strColor = pXml->GetAttrib("textcolor_normal");
	strNormalColor = strColor;
	if( strColor != "" )
	{
		pBtn->SetNormalTextColor( StringToRGB(strColor) );
	}

	strColor = pXml->GetAttrib("textcolor_over");
	if( strColor != "" )
	{
		pBtn->SetOverTextColor( StringToRGB(strColor) );
	}
	else
	{
		pBtn->SetOverTextColor( StringToRGB(strNormalColor) );
	}


	strColor = pXml->GetAttrib("textcolor_down");
	if( strColor != "" )
	{
		pBtn->SetDownTextColor( StringToRGB(strColor) );
	}
	else
	{
		pBtn->SetDownTextColor( StringToRGB(strNormalColor) );
	}

	strColor = pXml->GetAttrib("textcolor_disable");
	if( strColor != "" )
	{
		pBtn->SetDisableTextColor( StringToRGB(strColor) );
	}
	else
	{
		pBtn->SetDisableTextColor( StringToRGB(strNormalColor) );
	}

}



void CreateControlFactory::CreateSplitter( CWnd *pWnd, CMarkup *pXml, CSkinMgr *pSkin)
{
	ETSSplitter * pSpl = (ETSSplitter *) pWnd;

	CDibBitmap * vert = pSkin->GetDibBmp(pXml->GetAttrib("vert"), false);
	CDibBitmap * horz = pSkin->GetDibBmp(pXml->GetAttrib("horz"), false);
	CDibBitmap * down = pSkin->GetDibBmp(pXml->GetAttrib("btn_down"), false);
	CDibBitmap * up = pSkin->GetDibBmp(pXml->GetAttrib("btn_up"), false);
	CDibBitmap * right = pSkin->GetDibBmp(pXml->GetAttrib("btn_right"), false);
	CDibBitmap * left = pSkin->GetDibBmp(pXml->GetAttrib("btn_left"), false);
	CDibBitmap * down_over = pSkin->GetDibBmp(pXml->GetAttrib("btn_down_over"), false);
	CDibBitmap * up_over = pSkin->GetDibBmp(pXml->GetAttrib("btn_up_over"), false);
	CDibBitmap * right_over = pSkin->GetDibBmp(pXml->GetAttrib("btn_right_over"), false);
	CDibBitmap * left_over = pSkin->GetDibBmp(pXml->GetAttrib("btn_left_over"), false);

	if(vert == NULL && horz == NULL)
	{
		vert = pSkin->GetDibBmp("SpliterVertBK", false);
		horz = pSkin->GetDibBmp("SpliterHorzBK", false);
		down = pSkin->GetDibBmp("SpliterBarDown", false);
		up = pSkin->GetDibBmp("SpliterBarUp", false);
		right = pSkin->GetDibBmp("SpliterBarRight", false);
		left = pSkin->GetDibBmp("SpliterBarLeft", false);
		down_over = pSkin->GetDibBmp("SpliterBarDownOver", false);
		up_over = pSkin->GetDibBmp("SpliterBarUpOver", false);
		right_over = pSkin->GetDibBmp("SpliterBarRightOver", false);
		left_over = pSkin->GetDibBmp("SpliterBarLeftOver", false);
	}

	CString hide_btn = pXml->GetAttrib("hide-closebtn");

	if(hide_btn == "1")
		pSpl->m_bHideCloseBtn = true;
	else
		pSpl->m_bHideCloseBtn = false;

	pSpl->SetBitmap( vert,  horz, right, left, up, down, right_over, left_over, up_over, down_over);
}

void CreateControlFactory::CreateBtnTree( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes )
{
	CMyBtnTreeCtrl *pTree = (CMyBtnTreeCtrl*) pWnd;
	CDibBitmap *pDibOver,*pDibNormal,*pDibDown;

	CString name = pXml->GetAttrib("btnbmp");

	if((pDibNormal = pSkin->GetDibBmp(name + "normal")) == NULL)
		pDibNormal = pSkin->PutDibBmp(name + "normal", pSkin->m_strSkinPath);

	if(pDibNormal == NULL)
		return;

	if((pDibDown = pSkin->GetDibBmp(name + "down")) == NULL){
		pDibDown = pSkin->PutDibBmp(name + "down", pSkin->m_strSkinPath);
		if(pDibDown == NULL)
			pDibDown = pDibNormal;
	}

	if((pDibOver = pSkin->GetDibBmp(name + "over")) == NULL){
		pDibOver = pSkin->PutDibBmp(name + "over", pSkin->m_strSkinPath);
		if(pDibOver == NULL)
			pDibOver = pDibDown;
		if(pDibOver == NULL)
			pDibOver = pDibNormal;
	}

	pTree->SetButtonBitmap(pDibNormal,pDibOver,pDibDown);	

	pTree->SetNormalItemTextColor( StringToRGB(pXml->GetAttrib("nortext")) );
	pTree->SetSelectItemBKColor( StringToRGB(pXml->GetAttrib("selbk")) );
	pTree->SetSelectItemTextColor( StringToRGB( pXml->GetAttrib("seltext")) );
	pTree->SetBackColor( StringToRGB(pXml->GetAttrib("bk")) );
	pTree->SetScrollBitmap(pSkin->GetDibBmp("ScrollVert"), pSkin->GetDibBmp("ScrollHorz"));
}


COLORREF CreateControlFactory::StringToRGB(CString str, BOOL bChangeColor)
{
	CString r,g,b;	
	r = "0"+str.Mid(0,2);
	g = "0"+str.Mid(2,2);
	b = "0"+str.Mid(4,2);
	int nr,ng,nb;
	sscanf(r.GetBuffer(r.GetLength()),"%x",&nr);
	sscanf(g.GetBuffer(g.GetLength()),"%x",&ng);
	sscanf(b.GetBuffer(b.GetLength()),"%x",&nb);

	COLORREF color = RGB(nr,ng,nb);

	if(bChangeColor)
		::AfxGetUIManager()->UIGetChangeColorMgr()->ChangeColor(color);

	return color;	
}

void CreateControlFactory::CreateList( CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes )
{
	CMyListCtrl* pList = (CMyListCtrl*)pWnd;
	pList->SetColor(StringToRGB( pXml->GetAttrib("oddbk")),
					StringToRGB( pXml->GetAttrib("evenbk")),
					StringToRGB( pXml->GetAttrib("nortext")),
					StringToRGB( pXml->GetAttrib("selbk")),
					StringToRGB( pXml->GetAttrib("seltext")));

	pList->m_clrPlayItem=StringToRGB( pXml->GetAttrib("playbk"),false);

	CDibBitmap *H = pSkin->GetDibBmp(pXml->GetAttrib("scrollH"));
	if(H == NULL)
		H = pSkin->GetDibBmp("ScrollHorz");

	CDibBitmap *V = pSkin->GetDibBmp(pXml->GetAttrib("scrollV"));
	if(V == NULL)
		V = pSkin->GetDibBmp("ScrollVert");


	pList->SetScrollBitmap(V ,H);
}


void CreateControlFactory::CreateTree(CWnd *pWnd, CMarkup *pXml , CSkinMgr *pSkin, ILayoutMgr *m_mgrPanes)
{
	CMyTreeCtrl *pTree = (CMyTreeCtrl*) pWnd;
	pTree->SetNormalItemTextColor( StringToRGB(pXml->GetAttrib("nortext")) );
	pTree->SetSelectItemBKColor( StringToRGB(pXml->GetAttrib("selbk")) );
	pTree->SetSelectItemTextColor( StringToRGB( pXml->GetAttrib("seltext")) );
	pTree->SetBackColor( StringToRGB(pXml->GetAttrib("bk")) );
	pTree->SetScrollBitmap(pSkin->GetDibBmp("ScrollVert"), pSkin->GetDibBmp("ScrollHorz"));
}

