// MyButton.cpp : 实现文件
//

#include "stdafx.h"
#include "MyButton.h"
#include ".\mybutton.h"



// CMyButton

IMPLEMENT_DYNAMIC(CMyButton, CButton)
CMyButton::CMyButton()
{
	m_nType = 0;
	m_targetRect.SetRectEmpty();
}

CMyButton::~CMyButton()
{
}


BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMyButton 消息处理程序


void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC); 
	CRect rect(lpDrawItemStruct->rcItem); 
	COLORREF oc = pDC->GetTextColor(); 
	int iObk = pDC->SetBkMode(TRANSPARENT); 

	//初始化按钮状态 
	UINT state = lpDrawItemStruct->itemState; 

	CFont * pOldFont = NULL; 

	int iYOffset = 0, iXOffset = 0; 

	CString strText; 

	GetWindowText(strText); 

	rect.top = iYOffset; 

	rect.left = iXOffset; 

	int nTextOffset = 20;

	CRect picRs = rect;
	picRs.left += 2;
	picRs.right = picRs.left + 16;
	picRs.top = picRs.Height()/2-8;
	picRs.bottom = picRs.top + 16;

	//CSize sz = pDC->GetTextExtent(strText); 

	//int x = rect.left + (rect.Width() - sz.cx)/2; 

	//int y = rect.top + (rect.Height() - sz.cy)/2; 
	CRect testRs = rect;
	testRs.left += nTextOffset;
	if (state & ODS_DISABLED) 

	{ 

	//按钮置灰状态（DISABLED） 
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT)); 
		pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
		/*CBrush grayBrush; 

		grayBrush.CreateSolidBrush (GetSysColor (COLOR_GRAYTEXT)); 

		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT)); 

		pDC->DrawText(strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); */

	} 

	else 
	{
		if (state & ODS_SELECTED) 
		{ 
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHILIGHT));
		} 
		else 
		{
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
		} 
	} 
	pDC->DrawText(strText, testRs, DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	if(pOldFont) 
		pDC->SelectObject(pOldFont); 
	pDC->SetTextColor(oc); 

	pDC->Draw3dRect(picRs,RGB(100,100,100),RGB(100,100,100));
	picRs.left += 1;
	picRs.right -= 1;
	picRs.top += 1;
	picRs.bottom -= 1;
	//pDC->FillSolidRect(picRs,RGB(255,0,0));
	m_targetRect = picRs;
	Draw();
}

int CMyButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	CRect rect; 
	GetClientRect(&rect);
	rect.left += 2;
	rect.right = rect.left + 16;
	rect.top = rect.Height()/2-8;
	rect.bottom = rect.top + 16;

	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	rect.bottom -= 1;
	m_targetRect = rect;

	return 0;
}

BOOL CMyButton::OnEraseBkgnd(CDC* pDC)
{
	//CRect rs;
	//GetClientRect(&rs);
	//pDC->FillSolidRect(rs,RGB(255,0,0));
	return true;
	//return CButton::OnEraseBkgnd(pDC);
}

void CMyButton::Update(string &str)
{
	m_imagePath = str;
	m_nType = 2;
	Draw();
}

void CMyButton::Update(vector<COLORREF> &src,BOOL bGradient)
{
	m_colorSrc.clear();
	copy(src.begin(),src.end(),back_inserter(m_colorSrc));
	m_bGradient = bGradient;
	m_nType = 1;
	Draw();
}

void CMyButton::Draw()
{
	if(m_nType == 0)
		return;
	else if(m_nType == 1)
		DrawPreview(m_colorSrc,m_bGradient);
	else if(m_nType == 2)
		DrawPreview(m_imagePath);
}

HBITMAP LoadAnImage(const char* FileName,int x,int y)
{
	// Use IPicture stuff to use JPG / GIF files
	IPicture* p;
	IStream* s;
	//IPersistStream* ps;
	HGLOBAL hG;
	void* pp;
	FILE* fp;

	// Read file into memory
	fp = fopen(FileName, "rb");
	if (!fp)
	{
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	int fs = ftell(fp);
	fseek(fp,0,SEEK_SET);
	hG = GlobalAlloc(GPTR,fs);
	if(!hG)
	{
		fclose(fp);
		return NULL;
	}
	pp = (void*)hG;
	fread(pp,1,fs,fp);
	fclose(fp);

	// Create an IStream so IPicture can
	CreateStreamOnHGlobal(hG, false, &s);
	if(!s)
	{
		GlobalFree(hG);
		return NULL;
	}

	OleLoadPicture(s,0,false,IID_IPicture,(void**)&p);

	if(!p)
	{
		s->Release();
		GlobalFree(hG);
		return NULL;
	}
	s->Release();
	GlobalFree(hG);

	HBITMAP hB = 0;
	p->get_Handle((unsigned int*)&hB);

	// Copy the image. Necessary, because upon p's release,
	// the handle is destroyed.
	HBITMAP hBB = NULL;
	
	if(x == 0 || y == 0)
		hBB = (HBITMAP)CopyImage(hB, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
	else
		hBB = (HBITMAP)CopyImage(hB, IMAGE_BITMAP, x, y, LR_COPYRETURNORG);
	p->Release();
	return hBB;
}

void DrawImage(HDC hdc, HBITMAP hBitmap, RECT *rc)
{
	BITMAP bm;
	int cx;
	int cy;   
	HDC memdc;
	HBITMAP hOldBM;
	RECT  rcDest = *rc;   
	POINT p;
	SIZE  delta;
	COLORREF colorOld;

	if(hBitmap == NULL) 
		return;

	// center bitmap in caller's rectangle   
	GetObject(hBitmap, sizeof bm, &bm);   
	
	cx = bm.bmWidth;
	cy = bm.bmHeight;

	delta.cx = (rc->right-rc->left - cx) / 2;
	delta.cy = (rc->bottom-rc->top - cy) / 2;
	
	if(rc->right-rc->left > cx)
	{
		SetRect(&rcDest, rc->left+delta.cx, rc->top + delta.cy, 0, 0);   
		rcDest.right = rcDest.left + cx;
		rcDest.bottom = rcDest.top + cy;
		p.x = 0;
		p.y = 0;
	}
	else
	{
		p.x = -delta.cx;   
		p.y = -delta.cy;
	}
   
	// select checkmark into memory DC
	memdc = CreateCompatibleDC(hdc);
	hOldBM = (HBITMAP)SelectObject(memdc, hBitmap);
   
	// set BG color based on selected state   
	colorOld = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));

	BitBlt(hdc, rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, memdc, p.x, p.y, SRCCOPY);

	// restore
	SetBkColor(hdc, colorOld);
	SelectObject(memdc, hOldBM);
	DeleteDC(memdc);
}

void CMyButton::DrawPreview(string &str)
{
	if(str.length() == 0)
		return;
	CClientDC dc( this );
	HDC preHDC = dc.GetSafeHdc();
	CRect rcPreview = m_targetRect;
	
	rcPreview.left += 1;
	rcPreview.right -= 1;
	rcPreview.top += 1;
	rcPreview.bottom -= 1;
	HBITMAP hBmp = LoadAnImage(str.c_str(),rcPreview.Width(),rcPreview.Height());
	if(!hBmp)
		return;
	DrawImage(preHDC,hBmp,&rcPreview);
}

void CMyButton::DrawPreview(vector<COLORREF> &src,BOOL bGradient)
{
	CClientDC dc( this );
	HDC preHDC = dc.GetSafeHdc();
	CRect rcPreview = m_targetRect;
	
	rcPreview.left += 1;
	rcPreview.right -= 1;
	rcPreview.top += 1;
	rcPreview.bottom -= 1;

	int nHeight = rcPreview.Height();
	int nCount = src.size();
	Graphics graphics(preHDC);

	vector<PointF> verPoint;
	if(nCount >1)
	{
		int h = 0;
		if(bGradient)
		{
			h = nHeight/(nCount - 1);
			for(int i = 0 ;i<nCount;i++)
			{
				PointF p(rcPreview.left,rcPreview.top + i*h);
				verPoint.push_back(p);
			}
		}
		else
		{
			h = nHeight/nCount;
			for(int i = 0 ;i<=nCount;i++)
			{
				PointF p(rcPreview.left,rcPreview.top + i*h);
				verPoint.push_back(p);
			}
		}

		vector<Color> verColor;
		for(int i = 0 ;i<nCount;i++)
		{
			BYTE r = GetRValue(COLORREF(src.at(i)));
			BYTE g = GetGValue(COLORREF(src.at(i)));
			BYTE b = GetBValue(COLORREF(src.at(i)));

			Color col = Color(255,r,g,b);
			verColor.push_back(col);
		}
		if(bGradient)
		{
			for(int i = 0 ;i<nCount-1;i++)
			{
				LinearGradientBrush lgb( verPoint.at(i), verPoint.at(i+1), verColor.at(i), verColor.at(i+1));
				graphics.FillRectangle(&lgb, rcPreview.left, (int)verPoint.at(i).Y, (int)rcPreview.Width(),(int)(verPoint.at(i+1).Y - verPoint.at(i).Y));	
			}
		}
		else
		{
			for(int i = 0 ;i<nCount;i++)
			{
				SolidBrush br(verColor.at(i));
				graphics.FillRectangle(&br, rcPreview.left, (int)verPoint.at(i).Y, (int)rcPreview.Width(),(int)(verPoint.at(i+1).Y - verPoint.at(i).Y));	
			}
		}
	}
	else
	{
		BYTE r = GetRValue(COLORREF(src.at(0)));
		BYTE g = GetGValue(COLORREF(src.at(0)));
		BYTE b = GetBValue(COLORREF(src.at(0)));
		Color col = Color(255,r,g,b);
		SolidBrush br(col);
		graphics.FillRectangle(&br, rcPreview.left, (int)rcPreview.top, (int)rcPreview.Width(),(int)rcPreview.Height());	
	}
}