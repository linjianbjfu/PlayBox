#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include "coolscroll.h"
#include "../../Bitmap/dibbitmap.h"

UINT CALLBACK CoolSB_DrawProc(HDC hdc, UINT uCmdId, UINT uButflags, RECT *rect);
extern HWND hwndScroll;

BOOL fCustomDraw = true ;
extern BOOL fButtons     ;
extern BOOL fThumbAlways ;
HDC			hdcSkin;
HBITMAP		hSkinBmp;


HPEN hpen, oldpen;
HPEN whitepen;
HFONT hfont;
HENHMETAFILE hemf=0;

extern "C" void InitTest(void)
{
	hdcSkin  = CreateCompatibleDC(0);
	hSkinBmp = (HBITMAP)LoadImage(0, "c:\\skin01.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	SelectObject(hdcSkin, hSkinBmp);

//	DeleteEnhMetaFile(hemf);
}

typedef struct 
{
	int x, y;
	int width, height;
} CustomDrawTable;

//
//	Define a set of structures which describe
//	where-abouts the source "textures" are in the
//	custom draw bitmap. We need to know x, y, width and height
//	for each scrollbar segment.
//

CustomDrawTable cdt_horz_normal[] = 
{
	{ 0,  0,  18, 18 },	//left arrow  NORMAL
	{ 0,  19, 18, 18 }, //right arrow NORMAL
	{ 0,  83, 1,  18 }, //page left   NORMAL
	{ 0,  83, 1,  18 }, //page right  NORMAL
	
	{ -1, -1, -1, -1 },	//padding

	{ 54, 0,  9,  18 }, //horz thumb (left)
	{ 54+9, 0, 1, 18 }, //horz thumb (middle)
	{ 54+9, 0, 9, 18 }, //horz thumb (right)
};

CustomDrawTable cdt_horz_hot[] = 
{
	{ 18, 0,  18, 18 }, //left arrow  ACTIVE
	{ 18, 19, 18, 18 }, //right arrow ACTIVE
	{ 4,  83, 1,  18 }, //page left   ACTIVE
	{ 4,  83, 1,  18 }, //page right  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 54,   19, 9, 18 }, //horz thumb (left)
	{ 54+9, 19, 1, 18 }, //horz thumb (middle)
	{ 54+9, 19, 9, 18 }, //horz thumb (right)
};

CustomDrawTable cdt_horz_active[] = 
{
	{ 36, 0,  18, 18 }, //left arrow  ACTIVE
	{ 36, 19, 18, 18 }, //right arrow ACTIVE
	{ 4,  83, 1,  18 }, //page left   ACTIVE
	{ 4,  83, 1,  18 }, //page right  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 54,   38, 9, 18 }, //horz thumb (left)
	{ 54+9, 38, 1, 18 }, //horz thumb (middle)
	{ 54+9, 38, 9, 18 }, //horz thumb (right)
};

CustomDrawTable cdt_vert_normal[] = 
{
	{ 72, 0,  18, 18 }, //up arrow   NORMAL
	{ 72, 19, 18, 18 }, //down arrow NORMAL
	{ 0,  112, 18, 1 }, //page up	 NORMAL
	{ 0,  112, 18, 1 }, //page down  NORMAL

	{ -1, -1, -1, -1 },	//padding

	{ 126, 0,  18, 9  }, //vert thumb (left)
	{ 126, 9,  18, 1  }, //vert thumb (middle)
	{ 126, 9,  18, 9  }, //vert thumb (right)
};

CustomDrawTable cdt_vert_hot[] = 
{
	{ 90, 0,  18, 18 }, //up arrow   ACTIVE
	{ 90, 19, 18, 18 }, //down arrow ACTIVE
	{ 4,  83, 18, 1  }, //page up	 ACTIVE
	{ 4,  83, 18, 1  }, //page down  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 126, 19,  18, 9  }, //vert thumb (left)
	{ 126, 28,  18, 1  }, //vert thumb (middle)
	{ 126, 28,  18, 9  }, //vert thumb (right)
};

CustomDrawTable cdt_vert_active[] = 
{
	{ 108, 0,  18, 18 }, //up arrow   ACTIVE
	{ 108, 19, 18, 18 }, //down arrow ACTIVE
	{ 4,  83, 18, 1  }, //page up	 ACTIVE
	{ 4,  83, 18, 1  }, //page down  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 126, 38,  18, 9  }, //vert thumb (left)
	{ 126, 47,  18, 1  }, //vert thumb (middle)
	{ 126, 47,  18, 9  }, //vert thumb (right)
};

LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm, CDibBitmap * bmpV, CDibBitmap * bmpH)
{
	RECT *rc;
	//CustomDrawTable *cdt;
	UINT code = NM_CUSTOMDRAW;

	CDC* pDC = CDC::FromHandle(nm->hdc);

	UNREFERENCED_PARAMETER(ctrlid);

	if(bmpV == NULL || bmpH == NULL)
		return CDRF_DODEFAULT;

	// inserted buttons do not use PREPAINT etc..
	if(nm->nBar == SB_INSBUT)
	{
		//CoolSB_DrawProc(nm->hdc, nm->uItem, nm->uState, &nm->rect);
		return CDRF_SKIPDEFAULT;
	}
	
	if(!fCustomDraw) return CDRF_DODEFAULT;

	if(nm->dwDrawStage == CDDS_PREPAINT)
	{
		if(fCustomDraw)
			return CDRF_SKIPDEFAULT;
		else
			return CDRF_DODEFAULT;
	}


	if(nm->dwDrawStage == CDDS_POSTPAINT)
	{
		
	}

	//the sizing gripper in the bottom-right corner
	if(nm->nBar == SB_BOTH)	
	{
		RECT *rc = &nm->rect;
		//StretchBlt(nm->hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
		//	hdcSkin, 100, 100, 18, 18, SRCCOPY);
		COLORREF clr = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("ScrollBarCornerColor");
		if(clr == 0)
			clr = RGB(255,255,255);
		pDC->FillSolidRect(	rc, clr);
		return CDRF_SKIPDEFAULT;
	}

	rc = &nm->rect;
	if(rc->left == 0 && rc->right == 0)
		return CDRF_SKIPDEFAULT;

	else if(nm->nBar == SB_HORZ)
	{
		int w = bmpH->GetWidth() / 3;
		int h = bmpH->GetHeight() / 6;
		int delta = 0;
		rc = &nm->rect;

		if(nm->uState == CDIS_HOT)	
			delta = w;
		else if(nm->uState == CDIS_SELECTED) 
			delta = w * 2;
		
		if(nm->uItem == HTSCROLL_THUMB)
		{
			bmpH->Draw(pDC, rc->left, rc->top, 2 ,h, delta, h * 4, 2, h);
			bmpH->Draw(pDC, rc->left + 2, rc->top, (rc->right-rc->left)-4, rc->bottom-rc->top, delta, h*6, 1, h);
			bmpH->Draw(pDC, rc->right-2, rc->top, 2, h, delta, h * 5, 2,h);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_LEFT)
		{
			bmpH->Draw(pDC, rc->left, rc->top, w,h, delta, h, w, h);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_RIGHT)
		{
			bmpH->Draw(pDC, rc->left, rc->top, w ,h, delta, h * 2, w, h);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_PAGELEFT || nm->uItem == HTSCROLL_PAGERIGHT)
		{
			bmpH->Draw(pDC, rc->left, rc->top, rc->right - rc->left ,h, w, h*3, 1, h);
			return CDRF_SKIPDEFAULT;
		}

	}
	else if(nm->nBar == SB_VERT)
	{
		rc = &nm->rect;
		int w = bmpV->GetWidth() / 6;
		int h = bmpV->GetHeight() / 3;
		int delta = 0;
		if(nm->uState == CDIS_HOT)		
			delta = h;
		else if(nm->uState == CDIS_SELECTED)  
			delta = h * 2;

		if(nm->uItem == HTSCROLL_THUMB)
		{
			bmpV->Draw(pDC, rc->left, rc->top, w , 2, w * 3, 2 + delta, w, 2);
			bmpV->Draw(pDC, rc->left, rc->top + 2, (rc->right-rc->left), rc->bottom-rc->top - 4, w * 5, 1 + delta, w, 1);
			bmpV->Draw(pDC, rc->left, rc->bottom-2, w, 2, w * 4, 2 + delta, w,2);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_UP)
		{
			bmpV->Draw(pDC, rc->left, rc->top, w ,h, 0, h + delta, w, h);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_DOWN)
		{
			bmpV->Draw(pDC, rc->left, rc->top, w, h, w * 1, h + delta, w, h);
			return CDRF_SKIPDEFAULT;
		}
		if(nm->uItem == HTSCROLL_PAGEGUP || nm->uItem == HTSCROLL_PAGEGDOWN)
		{
			bmpV->Draw(pDC, rc->left, rc->top, w, rc->bottom - rc->top , w * 2, 1 + delta, w, 1);
			return CDRF_SKIPDEFAULT;
		}
	}
	//INSERTED BUTTONS are handled here...
	else if(nm->nBar == SB_INSBUT)
	{
		//CoolSB_DrawProc(nm->hdc, nm->uItem, nm->uState, &nm->rect);
		return CDRF_SKIPDEFAULT;
	}
	else
	{
		return CDRF_DODEFAULT;
	}

	return CDRF_SKIPDEFAULT;

}
