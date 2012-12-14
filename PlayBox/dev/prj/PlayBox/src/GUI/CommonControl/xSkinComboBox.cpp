#include "stdafx.h"
#include "xSkinComboBox.h"
#include "SkinScroll/coolsb_detours.h"
#include ".\xskincombobox.h"


/////////////////////////////////////COMBOBOXSKIN
COMBOBOXSKIN::COMBOBOXSKIN()
:pBmpButtonNormal(NULL)
,pBmpButtonOver(NULL)
,pBmpButtonDown(NULL)
,pBmpButtonDisable(NULL)
,colorEdge(RGB(0,0,0))
,colorEditBkg(RGB(255,255,255))
,colorListBkg(RGB(255,255,255))
,colorListSelect(RGB(49,106,197))
,iFontSize(12)
{
}

/////////////////////////////////////CComboBoxEdit

BEGIN_MESSAGE_MAP(CComboBoxEdit, CEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
ON_WM_PAINT()
END_MESSAGE_MAP()

CComboBoxEdit::CComboBoxEdit()
:m_colorBkg(RGB(255,255,255))
,m_colorText(RGB(0,0,0))
,m_pMsgWnd(NULL)
{
}


void CComboBoxEdit::SetSkin(COLORREF &colorBkg,COLORREF &colorText)
{
	m_colorBkg=colorBkg;

	m_colorText=colorText;

}

void CComboBoxEdit::SetMsgWnd(CWnd *pMsgWnd)
{
	m_pMsgWnd=pMsgWnd;
}


void CComboBoxEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pMsgWnd->SendMessage(MSG_EDIT_CLICKED);
	
	CEdit::OnLButtonDown(nFlags, point);
}

void CComboBoxEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetSel(0,-1);

	CEdit::OnLButtonDblClk(nFlags, point);
}

void CComboBoxEdit::OnPaint()
{
	CPaintDC dc(this); 

	CRect rectEdit;
	GetClientRect(&rectEdit);
	dc.FillSolidRect(&rectEdit,m_colorBkg);

	rectEdit.OffsetRect(3,0);
	rectEdit.right -= 6;
	dc.SelectObject( GetFont() );
	dc.SetTextColor(m_colorText);
	CString strText;
	GetWindowText(strText);
	dc.DrawText(strText, strText.GetLength(), rectEdit, DT_NOPREFIX|DT_VCENTER);

}

BOOL CComboBoxEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (message != WM_CTLCOLOREDIT) 
	{
		return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
	}

	HDC hdcChild = (HDC)wParam;

	SetTextColor(hdcChild, m_colorText);
	SetBkColor(hdcChild, m_colorBkg);
	return TRUE;
}



/////////////////////////////////////CComboBoxHistoryList
extern LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm,CDibBitmap * bmpV, CDibBitmap * bmpH);
IMPLEMENT_DYNAMIC(CComboBoxHistoryList, CListCtrl)

CComboBoxHistoryList::CComboBoxHistoryList()
:m_colorBkg(RGB(255,255,255))
,m_colorText(RGB(0,0,0))
,m_colorSelect(RGB(49,106,197))
,m_bInit(FALSE)
,m_pBitmapScrollHorz(NULL)
,m_pBitmapScrollVert(NULL)
{
}

CComboBoxHistoryList::~CComboBoxHistoryList()
{

}

BEGIN_MESSAGE_MAP(CComboBoxHistoryList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_COOLSB_CUSTOMDRAW, OnCoolSBCustomdraw )
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CComboBoxHistoryList::SetSkin(COLORREF &colorBkg,COLORREF &colorText,COLORREF &colorSelect,CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz)
{
	m_colorBkg=colorBkg;
	m_colorText=colorText;
	m_colorSelect=colorSelect;
	m_pBitmapScrollHorz = pBitmapHorz;
	m_pBitmapScrollVert = pBitmapVert;
}

void CComboBoxHistoryList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW   lplvcd   =   (LPNMLVCUSTOMDRAW)pNMHDR;   

	switch(lplvcd->nmcd.dwDrawStage)   
	{   
	case CDDS_PREPAINT:   
		*pResult=CDRF_NOTIFYITEMDRAW;   
		break;   

	case CDDS_ITEMPREPAINT:   
		*pResult=CDRF_NOTIFYSUBITEMDRAW;   
		break;   

	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:   
		{   
			int nitem=static_cast<int>(lplvcd->nmcd.dwItemSpec);   
			int nsubitem=lplvcd->iSubItem;   
			POSITION pos=GetFirstSelectedItemPosition();   
			if(nitem==GetNextSelectedItem(pos))
			{ 
				CDC* pDC=CDC::FromHandle(lplvcd->nmcd.hdc);   
				CString str;   
				CRect rect;   
				UINT nFormat=DT_VCENTER|DT_SINGLELINE;   
				GetSubItemRect(nitem,nsubitem,LVIR_BOUNDS,rect);   
				str=GetItemText(nitem,nsubitem);   
				pDC->SetTextColor(m_colorBkg);
				rect.left+=2;
				rect.right-=1;
				pDC->FillSolidRect(&rect,m_colorSelect);
				pDC->SetBkColor(m_colorBkg);
				rect.left+=2;
				CSize msize;   
				msize=pDC->GetTextExtent(str);	   
				pDC->DrawText(str,&rect,nFormat);

				*pResult=CDRF_SKIPDEFAULT;

			}  
			else
			{
				lplvcd->clrTextBk=m_colorBkg;
				lplvcd->clrText=m_colorText;
				*pResult=CDRF_DODEFAULT;   
			}
			break;   
		}
	}
}


BOOL CComboBoxHistoryList::OnEraseBkgnd(CDC* pDC)
{
	CRect rectList;
	GetClientRect(&rectList);
	pDC->FillSolidRect(&rectList,m_colorBkg);
	return FALSE;
}

void CComboBoxHistoryList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect(&rect);

	CPoint pt;
	GetCursorPos(&pt);
	if (!PtInRect(&rect,pt))
	{
		m_pMsgWnd->SendMessage(MSG_EDIT_CLICKED);
		return;
	}

	POSITION pos=GetFirstSelectedItemPosition();
	if (pos)
	{
		int iItem=GetNextSelectedItem(pos);
		CString strItemText=GetItemText(iItem,0);
		m_pMsgWnd->SendMessage(MSG_LIST_CLICKED,(WPARAM)&strItemText,(LPARAM)iItem);
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}


void CComboBoxHistoryList::OnMouseMove(UINT nFlags, CPoint point)
{

	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	int ret = SubItemHitTest(&lvhti);
	if( ret != -1 )
	{
		POSITION pos=GetFirstSelectedItemPosition();
		while(pos)
		{
			int iItem=GetNextSelectedItem(pos);
			SetItemState(iItem,~LVIS_SELECTED,LVIS_SELECTED);
		}
		int iCurrentCursorItem	= lvhti.iItem;
		SetItemState(iCurrentCursorItem,LVIS_SELECTED,LVIS_SELECTED);
	}

	CListCtrl::OnMouseMove(nFlags, point);

	HWND hWnd=::GetCapture();
	if (hWnd!=m_hWnd)
	{
		::SetCapture(m_hWnd);
	}
}


void CComboBoxHistoryList::SetMsgWnd(CWnd *pMsgWnd)
{
	m_pMsgWnd=pMsgWnd;
}


int CComboBoxHistoryList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnInit();

	ShowScrollBar(SB_HORZ,FALSE);
	ShowScrollBar(SB_VERT,FALSE);

	return 0;
}


void CComboBoxHistoryList::OnInit()
{
	if(!m_pBitmapScrollVert)
	{
		return;
	}
	int w,h;
	if(!m_bInit )
	{
		if( m_pBitmapScrollVert  != NULL )
		{
			h = m_pBitmapScrollVert->GetHeight()/3;
			w = m_pBitmapScrollVert->GetWidth()/6;
		}
		InitializeCoolSB(m_hWnd);
		CoolSB_SetStyle(m_hWnd, SB_VERT, CSBS_HOTTRACKED);
		CoolSB_SetSize(m_hWnd, SB_VERT,  h,w);
		::CoolSB_SetMinThumbSize(m_hWnd, SB_VERT, 8);
		m_bInit = true;
	}
}


void CComboBoxHistoryList::OnDestroy()
{
	if( m_bInit )
	{
		UninitializeCoolSB( m_hWnd );
	}
	__super::OnDestroy();
}

void CComboBoxHistoryList::OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult )
{
	NMCSBCUSTOMDRAW* pNMCD = reinterpret_cast<NMCSBCUSTOMDRAW*>(pNMHDR);
	if(m_pBitmapScrollVert && m_pBitmapScrollHorz)
	{
		*pResult = HandleCustomDraw(0,(NMCSBCUSTOMDRAW*)pNMHDR,m_pBitmapScrollVert,m_pBitmapScrollHorz );
	}
}


void CComboBoxHistoryList::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	ShowScrollBar(SB_HORZ,FALSE);
	ShowScrollBar(SB_VERT,FALSE);
}


/////////////////////////////////////CComboBoxHistory
CComboBoxHistory::CComboBoxHistory()
:m_colorEdge(RGB(0,0,0))
,m_colorBkg(RGB(255,255,255))
,m_iMaxWidth(0)
{
}

BEGIN_MESSAGE_MAP(CComboBoxHistory, CWnd)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CComboBoxHistory::SetSkin(COLORREF &colorEdge,COLORREF &colorListBkg,COLORREF &colorListText,COLORREF &colorListSelect,CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz)
{
	m_colorEdge=colorEdge;
	m_colorBkg=colorListBkg;
	m_pList.SetSkin(colorListBkg,colorListText,colorListSelect,pBitmapVert,pBitmapHorz);
}

int CComboBoxHistory::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pList.SetMsgWnd(m_pMsgWnd);
	m_pList.Create(WS_VISIBLE|LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_SHOWSELALWAYS,CRect(0,0,0,0),this,XSKINCOMBOBOX_HISTORY_LIST);

	LOGFONT   lf;   
	memset(&lf,0,sizeof(LOGFONT));   
	strcpy(lf.lfFaceName,   "宋体");   
	lf.lfHeight   =   12;   

	m_fontList.CreateFontIndirect(&lf);

	m_pList.SetFont(&m_fontList);

	m_pList.SetExtendedStyle( m_pList.GetExtendedStyle()| LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT );

	::SendMessage(m_pList.GetSafeHwnd(), LVM_SETHOVERTIME, 0, 10);

	m_pList.InsertColumn(0, "");

	return 0;
}


BOOL CComboBoxHistory::OnEraseBkgnd(CDC* pDC)
{
	CRect rectWnd;
	GetClientRect(&rectWnd);
	CPen newPen(PS_DASHDOTDOT,10,m_colorEdge);   
	CPen *pOldPen=pDC->SelectObject(&newPen);
	pDC->MoveTo(rectWnd.left,rectWnd.top);
	pDC->LineTo(rectWnd.right-1,rectWnd.top);
	pDC->LineTo(rectWnd.right-1,rectWnd.bottom-1);
	pDC->LineTo(rectWnd.left,rectWnd.bottom-1);
	pDC->LineTo(rectWnd.left,rectWnd.top);
	pDC->SelectObject(pOldPen);
	rectWnd.top+=1;
	rectWnd.left+=1;
	rectWnd.bottom-=1;
	rectWnd.right-=1;

	pDC->FillSolidRect(&rectWnd,m_colorBkg);
	return FALSE;
}

void CComboBoxHistory::RefreshData(vector<string> &vecString)
{
	m_pList.DeleteAllItems();

	m_iMaxWidth=0;
	CDC *pDc=m_pList.GetDC();
	LOGFONT   lf;   
	memset(&lf,0,sizeof(LOGFONT));   
	strcpy(lf.lfFaceName,   "宋体");   
	lf.lfHeight   =   12;   
	HFONT hfont=CreateFontIndirect(&lf);   
	HFONT hOldFont=(HFONT)pDc->SelectObject(hfont);
	for (int i=0;i<vecString.size();++i)
	{
		CString str=vecString.at(i).c_str();
		m_pList.InsertItem(i,str.GetString());

		CSize size=pDc->GetTextExtent(str.Trim());

		if (size.cx>m_iMaxWidth)
		{
			m_iMaxWidth=size.cx;
		}
	}
	pDc->SelectObject(hOldFont);
	DeleteObject(hfont);
}
void CComboBoxHistory::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		CRect rect;
		GetClientRect(&rect);
		m_pList.SetColumnWidth(0,rect.Width()-2);

		rect.top+=1;
		rect.bottom-=1;
		rect.left+=1;
		rect.right-=1;
		m_pList.MoveWindow(rect);
		::SetCapture(m_pList.GetSafeHwnd());
	}
	else
	{
		::ReleaseCapture();
	}
}
void CComboBoxHistory::SetMsgWnd(CWnd *pMsgWnd)
{
	m_pMsgWnd=pMsgWnd;
}

int CComboBoxHistory::GetMaxItemTextWidth()
{
	return m_iMaxWidth;
}


int CComboBoxHistory::GetItemHeight()
{
	CRect rect;
	m_pList.GetItemRect(0,&rect,LVIR_BOUNDS);

	return rect.Height();
}


/////////////////////////////////////CxSkinComboBox

CxSkinComboBox::CxSkinComboBox()
:m_iBtnWidth(20)
,m_iBtnHeight(DEFAULT_COMBOBOX_SIZE_HEIGHT)
,m_iCurSel(-1)
,m_iMaxShowLines(5)
{
	RegisterWindowClass();
}

CxSkinComboBox::~CxSkinComboBox()
{

}

BOOL CxSkinComboBox::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, XSKINCOMBOBOX_CLASSNAME, &wndcls)))
	{
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = XSKINCOMBOBOX_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}
	return TRUE;
}

void CxSkinComboBox::SetSkin(COMBOBOXSKIN &skin)
{
	m_pSkin=skin;

	m_pEdit.SetSkin(skin.colorEditBkg,skin.colorEditText);
	m_pHistoryWnd.SetSkin(skin.colorEdge,skin.colorListBkg,skin.colorListText,skin.colorListSelect,skin.pBmpScrollVert,skin.pBmpScrollHorz);
	m_pHistoryBtn.SetSkin(skin.pBmpButtonNormal,skin.pBmpButtonDown,skin.pBmpButtonOver,skin.pBmpButtonDisable,0,0,0,0,0);
}

BOOL CxSkinComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(XSKINCOMBOBOX_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}
BEGIN_MESSAGE_MAP(CxSkinComboBox, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(MSG_LIST_CLICKED,OnListClicked)
	ON_MESSAGE(MSG_EDIT_CLICKED,OnEditClicked)
END_MESSAGE_MAP()

int CxSkinComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pEdit.SetMsgWnd(this);
	m_pEdit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP|ES_AUTOHSCROLL, CRect(1, 1, DEFAULT_COMBOBOX_SIZE_WIDTH-11, DEFAULT_COMBOBOX_SIZE_HEIGHT-1), this, XSKINCOMBOBOX_EDIT);

	m_pEditFont.CreateFont(
		m_pSkin.iFontSize,         // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet 
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体");  

	m_pEdit.SetFont(&m_pEditFont);
	m_pEdit.SetMargins(3,2);

	m_pHistoryWnd.SetMsgWnd(this);
	m_pHistoryWnd.Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),this,XSKINCOMBOBOX_HISTORYWND );
	m_pHistoryWnd.ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	m_pHistoryBtn.Create(NULL,WS_VISIBLE,CRect(DEFAULT_COMBOBOX_SIZE_WIDTH-10,1,DEFAULT_COMBOBOX_SIZE_WIDTH-1,DEFAULT_COMBOBOX_SIZE_HEIGHT-1),this,XSKINCOMBOBOX_BUTTON );

	if (m_pSkin.pBmpButtonNormal)
	{
		m_iBtnWidth=m_pSkin.pBmpButtonNormal->GetWidth();
		m_iBtnHeight=m_pSkin.pBmpButtonNormal->GetHeight();
	}

	m_brushBkg.CreateSolidBrush(m_pSkin.colorEdge);

	m_brushEditAddBkg.CreateSolidBrush(m_pSkin.colorEditBkg);

	MoveWindow(CRect(0,0,DEFAULT_COMBOBOX_SIZE_WIDTH,DEFAULT_COMBOBOX_SIZE_HEIGHT));
	
	m_pHistoryWnd.ShowWindow(SW_HIDE);
	return 0;
}


BOOL CxSkinComboBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rectWnd;
	GetClientRect(&rectWnd);
	CPen newPen(PS_DASHDOTDOT,10,m_pSkin.colorEdge);   
	CPen *pOldPen=pDC->SelectObject(&newPen);
	pDC->MoveTo(rectWnd.left,rectWnd.top);
	pDC->LineTo(rectWnd.right-1,rectWnd.top);
	pDC->LineTo(rectWnd.right-1,rectWnd.bottom-1);
	pDC->LineTo(rectWnd.left,rectWnd.bottom-1);
	pDC->LineTo(rectWnd.left,rectWnd.top);
	pDC->SelectObject(pOldPen);
	rectWnd.top+=1;
	rectWnd.left+=1;
	rectWnd.bottom-=1;
	rectWnd.right-=1;

	pDC->FillRect(&rectWnd,&m_brushEditAddBkg);

	return FALSE;
}


void CxSkinComboBox::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);


	CRect rect,rectEdit,rectBtn;
	GetClientRect(&rect);

	rectEdit.left=rect.left+1;
	rectEdit.top=rect.top+3;
	rectEdit.right=rect.right-m_iBtnWidth;
	rectEdit.bottom=rect.bottom-1;
	m_pEdit.MoveWindow(rectEdit);

	rectBtn.left=rect.right-m_iBtnWidth;
	rectBtn.top=rect.top+1;
	rectBtn.right=rect.right-1;
	rectBtn.bottom=rect.bottom-1;
	m_pHistoryBtn.MoveWindow(rectBtn);

}

void CxSkinComboBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
}

int CxSkinComboBox::AddString(LPCTSTR lpszString)
{
	if (m_vecString.size()<1)
	{
		m_iCurSel=0;
	}
	m_vecString.push_back(string(lpszString));

	return m_vecString.size()-1;
}

void CxSkinComboBox::SetCurSel(int iSel)
{
	if (iSel<0 || iSel>=m_vecString.size())
	{
		return;
	}
	m_iCurSel=iSel;
	m_pEdit.SetWindowText(m_vecString.at(iSel).c_str());
}

int CxSkinComboBox::GetCurSel()
{
	return m_iCurSel;
}

void CxSkinComboBox::SetEditSel(int iSel,int iLength)
{
	m_pEdit.SetFocus();
	m_pEdit.SetSel(iSel,iLength);
}

int CxSkinComboBox::GetCount()
{
	return m_vecString.size();
}

void CxSkinComboBox::GetLBText(int iSel,CString& strText)
{
	strText=m_vecString.at(iSel).c_str();
}

void CxSkinComboBox::ResetContent()
{
	m_vecString.clear();
}

void CxSkinComboBox::SetWindowText(LPCTSTR lpszString)
{
	m_pEdit.SetWindowText(lpszString);
	m_pEdit.SetFocus();
	m_pEdit.SetSel(0,-1);
}

void CxSkinComboBox::GetWindowText(CString &strText)
{
	m_pEdit.GetWindowText(strText);
}

BOOL CxSkinComboBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD lowWord	= LOWORD( wParam );

	if (lowWord==XSKINCOMBOBOX_BUTTON)
	{
		BOOL bIsHistoryVisuable=m_pHistoryWnd.IsWindowVisible();
		if (!bIsHistoryVisuable)
		{
			m_pHistoryWnd.RefreshData(m_vecString);
			
			int iItemHeight=m_pHistoryWnd.GetItemHeight()+1;
			int iHeight=4+iItemHeight*(m_vecString.size()>m_iMaxShowLines?m_iMaxShowLines:m_vecString.size());
			int iWidth=m_pHistoryWnd.GetMaxItemTextWidth()+10;
			CRect rect;
			GetWindowRect(&rect);
			rect.top=rect.bottom-1;
			rect.bottom=rect.top+iHeight;

			if (iWidth>rect.Width())
			{
				rect.right=rect.left+iWidth;
			}

			m_pHistoryWnd.SetParent(GetDesktopWindow());
			::SetWindowPos(m_pHistoryWnd,HWND_TOPMOST,rect.left,rect.top,rect.Width(),rect.Height(),NULL);

			m_pHistoryWnd.ShowWindow(SW_SHOW);
			m_pHistoryBtn.SendMessage(WM_KILLFOCUS);
			m_pHistoryWnd.SetFocus();
			m_pHistoryWnd.m_pList.SetItemState(m_iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else
		{
			m_pHistoryWnd.ShowWindow(SW_HIDE);
			m_pEdit.SetFocus();
			m_pEdit.SetSel(0,-1);
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}

LRESULT CxSkinComboBox::OnListClicked(WPARAM wParam,LPARAM lParama)
{
	CString *strText=(CString*)wParam;
	m_iCurSel=(int)lParama;
	m_pHistoryWnd.ShowWindow(SW_HIDE);
	m_pEdit.SetWindowText(*strText);
	m_pEdit.SetFocus();
	m_pEdit.SetSel(0,-1);
	GetParent()->SendMessage(MSG_COMBOBOX_SELECTE_CHANGE,GetDlgCtrlID());
	return 0;
}

LRESULT CxSkinComboBox::OnEditClicked(WPARAM wParam,LPARAM lParama)
{
	m_pHistoryWnd.ShowWindow(SW_HIDE);
	m_pEdit.SetFocus();
	m_pEdit.SetSel(0,-1);
	return 0;
}

void CxSkinComboBox::SetMaxShowLines(int iNum)
{
	m_iMaxShowLines=iNum;
}



