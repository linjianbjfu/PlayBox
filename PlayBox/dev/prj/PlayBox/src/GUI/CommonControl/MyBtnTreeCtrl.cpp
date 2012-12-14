#include "stdafx.h"
#include "MyBtnTreeCtrl.h"


IMPLEMENT_DYNAMIC(CMyBtnTreeCtrl, CMyTreeCtrl)
CMyBtnTreeCtrl::CMyBtnTreeCtrl():m_bMouseOverBtn(false),
								 m_pBitmapOver(NULL),
								 m_pBitmapNormal(NULL),
								 m_pBitmapDown(NULL),
								 m_bHideBtn(true)
{
	m_bTrackMouse = false;
}

CMyBtnTreeCtrl::~CMyBtnTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyBtnTreeCtrl, CMyTreeCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
END_MESSAGE_MAP()

BOOL CMyBtnTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}
/*
需求：1一级目录字粗体显示，后边跟随的字正常体显示
	  2非选中项时，鼠标hover，背景有hover状态
实现：需求1只针对写字，可判断当前item是否有父亲，如有父亲，则非粗体，如无，则粗体显示
	  需求2只针对背景，可判断当前item是否选中，如选中，画选中背景；如无却hover，则画hover背景，如两者都无，画正常背景
*/
void CMyBtnTreeCtrl::OnCustomdraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMTVCUSTOMDRAW* pLVCD = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;
	HTREEITEM item = GetSelectedItem();

	bool bDrop = false;
	if( item )
	{
		bDrop = (GetItemState( item,TVIS_DROPHILITED ) & TVIS_DROPHILITED);
	}
	//当前所画的item的状况
	HTREEITEM hCurTreeItem = (HTREEITEM)(pLVCD->nmcd.dwItemSpec);
	CRect rc, rctext;
	GetItemRect(hCurTreeItem, &rctext, true);
	GetItemRect(hCurTreeItem, &rc, false);
	CString text = GetItemText(hCurTreeItem);
	CRect newRc(rctext);

	bool bSelect = (item==hCurTreeItem);
	bool bHover = (hCurTreeItem==m_iHoverItem);
	CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
	COLORREF colorOld = pDC->GetTextColor();
	CFont* pOldFont = GetFont();
	int nImage, nSelectedImage;
	GetItemImage( hCurTreeItem, nImage, nSelectedImage );
	UINT nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;

	CRect rcDrawText;//写字的范围
	CRect rcDrawTextReal; //实际写出的字的范围
	CDibBitmap *pDibTopLine = NULL;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
		//画出每个item上边的横道
		pDibTopLine = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("ListBkTop");
		if( pDibTopLine != NULL )
		{
			CRect rcTop(rc);
			rcTop.bottom = rcTop.top + pDibTopLine->GetHeight();
			rcTop.left   = 8;
			rcTop.right  = rcTop.right - 8;
			pDibTopLine->SetCDibRect(rcTop);
			pDibTopLine->Draw(pDC, true);
		}
		//画背景/////////////////////////////////////////////////////////////
		if( bSelect )
		{
			pDC->FillSolidRect(&rc,m_colorSelectItemBK);
		}else
		if( bHover )
		{
			CDibBitmap *pDib = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("ListHoverBK");
			pDib->SetCDibRect(rc);
			pDib->Draw(pDC, true);
		}
		//字前的图/////////////////////////////////////////////////////////////
		IMAGEINFO imageInfo;
		m_ImageList.GetImageInfo( nImage, &imageInfo );
					
		SIZE size;
		size.cx = size.cy = imageInfo.rcImage.right-imageInfo.rcImage.left;
		POINT pDes, pSrc;
		pDes.x = rctext.left - GetItemHeight() + ( GetItemHeight()-size.cx )/2;
		pDes.y = rctext.top + ( GetItemHeight()-size.cy )/2;
		pSrc.x = pSrc.y = 0;
		m_ImageList.DrawIndirect(pDC, nImage, pDes, size, pSrc );
		//画字后面的btn/////////////////////////////
		m_BtnRect = rc;
		if( bSelect && !m_bHideBtn && pLVCD->iLevel == 1 )
		{
			m_BtnRect.left = m_BtnRect.right - 30;			
		}else
		{
			m_BtnRect.left = m_BtnRect.right;
		}
		newRc = rctext;
		if(pLVCD->iLevel == 1)
		{
			newRc.right = m_BtnRect.left;
			if( m_bMouseOverBtn )
			{
				DrawImage( m_pBitmapOver, m_BtnRect, *pDC );
			}else
			{
				DrawImage( m_pBitmapNormal, m_BtnRect, *pDC );
			}
		}
		//写字/////////////////////////////////////////////////////////////
		pDC->SetBkMode( TRANSPARENT );
		if( bSelect )//选中时的写字颜色
		{
			pDC->SetTextColor(m_colorSelectItemText);
		}
		else
		{
			pDC->SetTextColor(m_colorNormalItemText);
		}

		rcDrawText = newRc;
		pDC->DrawText(text,&rcDrawText,nFormat);
		rcDrawTextReal = rcDrawText;
		pDC->DrawText(text,&rcDrawTextReal,nFormat|DT_CALCRECT);

		pDC->SetTextColor(colorOld);
		pDC->SelectObject(pOldFont);
		/////////////////////////////////////////////////////////////
		*pResult = CDRF_SKIPDEFAULT;
		break;
	case CDDS_PREPAINT :
		if ( rc == pLVCD->nmcd.rc /*|| bDrop*/ )
		{
			pLVCD->clrTextBk = m_colorSelectItemBK;//RGB(0x0,0x0,0xFF);
			pLVCD->clrText   = m_colorSelectItemText;			
			m_BtnRect = rc;
			if(m_bHideBtn)
				m_BtnRect.left = m_BtnRect.right;// - 30;
			else
			m_BtnRect.left = m_BtnRect.right - 30;
		}
		else
		{
			pLVCD->clrText   = m_colorNormalItemText;
		}

		*pResult = CDRF_NOTIFYITEMDRAW  ;
		break ;
	default:
		*pResult = CDRF_DODEFAULT;
	}
}
void CMyBtnTreeCtrl::DrawImage( CDibBitmap* pDibBitmap, CRect &r, CDC & memDC )
{		
	if(r.Width() == 0)
		return ;
	CPoint pt = r.CenterPoint();
	pt.x = pt.x - pDibBitmap->GetWidth() /2;
	pt.y = pt.y - pDibBitmap->GetHeight() /2;
	pDibBitmap->SetCDibRect(CRect(pt.x, pt.y, pt.x + pDibBitmap->GetWidth() , pt.y + pDibBitmap->GetHeight()));
	pDibBitmap->Draw(&memDC, true);	
}

void CMyBtnTreeCtrl::SetButtonBitmap(CDibBitmap* pBitNormal,CDibBitmap* pBitOver,CDibBitmap* pBitDown)
{
	m_pBitmapNormal = pBitNormal;
	m_pBitmapDown	= pBitDown;
	m_pBitmapOver	= pBitOver;
}

LPARAM CMyBtnTreeCtrl::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bTrackMouse = FALSE;

	if( m_iHoverItem != NULL )
	{
		CRect rcHover;
		GetItemRect( m_iHoverItem, &rcHover, FALSE );
		InvalidateRect(rcHover);
		m_iHoverItem = NULL;
	}

	if( m_bMouseOverBtn )
	{
		m_bMouseOverBtn = false;
		InvalidateRect(m_BtnRect);
	}
	return 0;
}

void CMyBtnTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM htree = HitTest(point);
	
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bTrackMouse = TRUE;
	}

	if( htree == NULL )
	{
		if( m_bMouseOverBtn )
		{
			m_bMouseOverBtn = false;
			InvalidateRect(m_BtnRect);
		}
		if(	m_iHoverItem != NULL )
		{
			CRect rcHover;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
			m_iHoverItem = NULL;
		}
		return __super::OnMouseMove( nFlags,point );
	}else
	{
		if( m_iHoverItem != htree )
		{
			//更新原来的item
			CRect rcHover;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
			//更新现在的item
			m_iHoverItem = htree;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
		}
	}

	if(m_BtnRect.PtInRect(point) && point.x < m_BtnRect.right - 2)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		if( !m_bMouseOverBtn )
		{
			m_bMouseOverBtn = true;
			InvalidateRect(m_BtnRect);
		}
	}
	else 
	{
		if( m_bMouseOverBtn )
		{
			m_bMouseOverBtn = false;
			InvalidateRect(m_BtnRect);
		}

		if( ItemHasChildren( htree ) )
		{
			CRect rectText;
			CRect rectAll;
			GetItemRect( htree,rectText,true);
			GetItemRect( htree,rectAll,false);
			rectAll.right = rectText.left - 3;
			if( rectAll.PtInRect(point) )
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				return __super::OnMouseMove( nFlags,point );
			}
		}
		else
		{
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));			
		}
	}	
	__super::OnMouseMove(nFlags, point);
}
