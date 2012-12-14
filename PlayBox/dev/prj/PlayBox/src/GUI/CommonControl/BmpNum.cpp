// BmpNum.cpp : implementation file
//

#include "stdafx.h"
#include "BmpNum.h"
#include ".\bmpnum.h"



// CBmpNum

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBmpNum, CWnd)
CBmpNum::CBmpNum():
m_ctrolType(0),
m_nShowStyle(ILD_TRANSPARENT ),
m_num(0),
m_pNumBitmap(NULL),
m_pBKBitmap(NULL)
{
	memset( &m_armIntg,-1, sizeof(short)*20 );
}

CBmpNum::~CBmpNum() 
{

}


BEGIN_MESSAGE_MAP(CBmpNum, CWnd)
	ON_WM_PAINT()
	//	ON_WM_ERASEBKGND()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CBmpNum message handlers

void CBmpNum::SetBackImage(const char* name)
{
	ISkinMgr* pSkinMgr	= AfxGetUIManager()->UIGetSkinMgr();

	m_pBKBitmap = pSkinMgr->GetDibBmp( name );
}


void CBmpNum::SetImage(const char* name)
{
	ISkinMgr* pSkinMgr	= AfxGetUIManager()->UIGetSkinMgr();

	m_pNumBitmap = pSkinMgr->GetDibBmp( name );

	if( m_pNumBitmap )
	{
		m_pNumBitmap->SetCount(11);  // 图片中包含两个小图片， 类似于CImgList
	}
}



void CBmpNum::OnPaint()
{

	CPaintDC dc(this); // device context for painting
	if( m_pNumBitmap == NULL ) return ;
	CRect lrect;
	this->GetClientRect( &lrect );

	m_pNumBitmap->SetCDibRect(lrect);
	m_pBKBitmap->SetCDibRect(lrect);


	//  IMAGEINFO img;  // using MFC
	//m_Image.GetImageInfo(1, &img);  // using MFC
	//img.rcImage.right = img.rcImage.left;  //// using MFC


	int lwidth = lrect.Width()/m_digit;

	int intg = 19;
	int drawNum = 1;
	if( (m_ctrolType&TIME)==TIME )
	{
		while( m_armIntg[intg] != -1 )
		{
			if( drawNum == 3) drawNum++;
			m_pNumBitmap->DrawImageList( &dc, m_armIntg[intg], CPoint( lrect.right-drawNum*lwidth ,0),m_digit);
			drawNum ++;
			intg --;
		}

		if (m_armIntg[19] != -1)
		{
			m_pNumBitmap->DrawImageList( &dc,10, CPoint( lrect.right-3*lwidth ,0),m_digit);
		}
		else
		{
			if (m_pBKBitmap)
			{
				m_pBKBitmap->Draw(&dc,FALSE);
			}
		}
	}
	else
	{
		if(( (m_ctrolType&RIGHG) == RIGHG )&& (!( (m_ctrolType&ABITMAP) == ABITMAP)) )
		{
			while( m_armIntg[intg] != -1 )
			{
				m_pNumBitmap->DrawImageList( &dc, m_armIntg[intg], CPoint( lrect.right-drawNum*lwidth ,0),m_digit);
				drawNum ++;
				intg --;
			}
		}
		else if(( (m_ctrolType&LEFT) == LEFT )&& (!((m_ctrolType&ABITMAP) == ABITMAP)) )
		{
			while( m_armIntg[intg] != -1 )
			{
				m_pNumBitmap->DrawImageList( &dc, m_armIntg[intg], CPoint( lrect.left +(drawNum-1)*lwidth ,0),m_digit );
				drawNum ++;
				intg --;
			}
		}
		else if( (m_ctrolType&ABITMAP) == ABITMAP )
		{
			m_pNumBitmap->DrawImageList( &dc, m_num, CPoint( 0 ,0) );
		}
	}
}
void CBmpNum::SetNum( int anum)
{	
	if( (m_ctrolType&ABITMAP) == ABITMAP )
	{
		m_num = anum ;
		this->Invalidate();
		return ;
	}

	memset( &m_armIntg,-1, sizeof(short)*20 );
	bool lbMinus = false;
	if( anum<0 )
	{
		anum = -anum;
		lbMinus = true;
	}
	int intg = 19;
	do
	{

		if( anum>9)
		{
			m_armIntg[intg] = anum%10;
		}
		else
		{
			m_armIntg[intg] = anum;
			break;
		}
		anum = anum/10;
		intg --;
	}while(intg>=0);
	if( lbMinus )	m_armIntg[--intg] = 10;

	Invalidate();
	//this->UpdateWindow();
}
void CBmpNum::SetNum( CString strTime)
{
	strTime.Delete(2,1);

	memset( &m_armIntg,-1, sizeof(short)*20 );

	if( strTime.GetLength() == 0 )
	{
		return;
	}
	int intg = 19;

	for( int i=3; i>=0; i-- )
	{
		char c = strTime.GetAt(i);
		m_armIntg[intg] = atoi(&c);
		intg--;
	}
	//this->UpdateWindow();
	Invalidate();

}
BOOL CBmpNum::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CWnd::OnEraseBkgnd(pDC);
}
