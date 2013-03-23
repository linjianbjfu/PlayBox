
#include "stdafx.h"
#include "scrollbarEx.h"
#include "BitMapOperate.h"
#include "WndColorMgr.h"
extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScrollBarEx::CScrollBarEx()
{
	mpWnd = NULL;
	mScrollOrientation = esoVertical;
	mbEnabled = false;
	mbTrackLeave = false;
	mbLButtonDown = false;
	mbUpArrowDown = false;
	mbDownArrowDown = false;
	mbThumbDown = false;
	mbThumbSpaceDown = false;
	miLastHitTest = HTNOWHERE;
	miLButtonDownHitTest = HTNOWHERE;
	miButtonSize = GetSystemMetrics(SM_CXHTHUMB);
	ZeroMemory(&mScrollInfo, sizeof(mScrollInfo));
	mScrollInfo.cbSize = sizeof(mScrollInfo);

	mrcClient.left = 0;
	mrcClient.top = 0;
	mrcClient.right = 0;
	mrcClient.bottom = 0;


	/*mcolNormal1 = gWndColor.getColor(SCROLLBAR_CTROL)->colNormal1 ;
	mcolNormal2 = gWndColor.getColor(SCROLLBAR_CTROL)->colNormal2;
	mcolEnter1 = gWndColor.getColor(SCROLLBAR_CTROL)->colEnter1;
	mcolEnter2 = gWndColor.getColor(SCROLLBAR_CTROL)->colEnter2;
	mcolDown1 = gWndColor.getColor(SCROLLBAR_CTROL)->colDown1;
	mcolDown2 = gWndColor.getColor(SCROLLBAR_CTROL)->colDown2;
	mcolFrame1 = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFrame1;
	mcolFrame2 = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFrame2;
	mcolBtn1 = gWndColor.getColor(SCROLLBAR_CTROL)->colDisable1;
	mcolBtn2 = gWndColor.getColor(SCROLLBAR_CTROL)->colDisable2;
	mcolFont = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFont ;
	mcolBK = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colBK;*/

	misDlg = FALSE;
//	mPointOffsetSum = 0;
}

CScrollBarEx::~CScrollBarEx()
{
}

BEGIN_MESSAGE_MAP(CScrollBarEx, CStatic)
	//{{AFX_MSG_MAP(CScrollBarEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//	ON_WM_MBUTTONUP()
	//ON_WM_NCLBUTTONUP()
	//ON_WM_RBUTTONUP()
	//ON_WM_LBUTTONUP()
//	ON_WM_CTLCOLOR_REFLECT()
//ON_WM_PAINT()

ON_MESSAGE(WM_PDLG_MOUSEWHEEL_MSG,&CScrollBarEx::OnDlgMouseWheel)
ON_WM_CREATE()
END_MESSAGE_MAP()


#pragma warning(disable:4311)
#pragma warning(disable:4312)

void CScrollBarEx::PreSubclassWindow() 
{
	ModifyStyle(0, SS_NOTIFY);	//ʹstatic�ؼ�����Ӧ�����Ϣ
	mWndProc = (WNDPROC)SetWindowLong(this->GetSafeHwnd(), GWL_WNDPROC, (LONG)SelfProc);
	CStatic::PreSubclassWindow();
}

LRESULT CALLBACK CScrollBarEx::SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lReturn = 0;
	CScrollBarEx* lpThis = NULL;
	lpThis = (CScrollBarEx*)CWnd::FromHandle(hWnd);
	if (lpThis == NULL)
		return lReturn;

	PAINTSTRUCT ps; 
	HDC hDC; 
	switch (uMsg)
	{
	case WM_PAINT:
		{
			hDC = ::BeginPaint(hWnd, &ps);
			lpThis->Draw(hWnd, hDC);
			::EndPaint(hWnd, &ps);
			return 0L;
		}
		break;
	case WM_ENABLE:
		lpThis->Invalidate();
		break;
	}
	// ����ԭ���Ļص�����
	lReturn = (LONG)CallWindowProc(lpThis->mWndProc, hWnd, uMsg, wParam, lParam);
	return lReturn;
}

void CScrollBarEx::OnLButtonDown(UINT aiFlags, CPoint aPoint)
{
	if( TRUE == misDlg )
	{
		mbLButtonDown = true;
		SetCapture();
		Invalidate();
		miLButtonDownHitTest = HitTest(aPoint);

		if (miLButtonDownHitTest == HT_THUMB)
		{
			mbThumbDown = true;

			RECT lrcThumb = GetRect(HT_THUMB);
			CPoint lPointCenter;
			lPointCenter.x = lrcThumb.left + (lrcThumb.right - lrcThumb.left) / 2;
			lPointCenter.y = lrcThumb.top + (lrcThumb.bottom - lrcThumb.top) / 2;
			mPointDragOffset = aPoint - lPointCenter;
			//mPointMoveOffset = aPoint ;
			
			
		}
		else if (miLButtonDownHitTest == HT_THUMB_SPACE)
		{
			mbThumbSpaceDown = true;
			SetTimer(102, 30, NULL);
		}
		else if (miLButtonDownHitTest == HT_LEFTARROW || miLButtonDownHitTest == HT_UPARROW)
		{
			mbUpArrowDown = true;
			SetTimer(102, 60, NULL);
		}
		else if (miLButtonDownHitTest == HT_RIGHTARROW || miLButtonDownHitTest == HT_DOWNARROW)
		{
			mbDownArrowDown = true;
			SetTimer(102, 60, NULL);
		}
	}
	else
	{

	
		mbLButtonDown = true;
		miLButtonDownHitTest = HitTest(aPoint);

		//������ΪHT_THUMB_SPACE��ʱ����Ҫ���жϲ�������Ч���

		if (miLButtonDownHitTest==HT_THUMB_SPACE)
		{
			RECT mrcThmb = GetRect(HT_THUMB);
			if ((mScrollOrientation == esoVertical && (aPoint.x <= mrcThmb.left ||	aPoint.x >= mrcThmb.right))
				||
				(mScrollOrientation != esoVertical && (aPoint.y <= mrcThmb.top  ||	aPoint.y >= mrcThmb.bottom)))
			{
				miLButtonDownHitTest = HTNOWHERE ;
			}

		}


		
		///////////////////////////////////////////////////////////////

		SetCapture();
		Invalidate();

		if (miLButtonDownHitTest == HT_THUMB)
		{
			RECT lrcThumb = GetRect(HT_THUMB);
			CPoint lPointCenter;
			//ƫ��ֵ�ļ���
			lPointCenter.x = lrcThumb.left + (lrcThumb.right - lrcThumb.left) / 2;
			lPointCenter.y = lrcThumb.top + (lrcThumb.bottom - lrcThumb.top) / 2;

			mPointDragOffset = aPoint - lPointCenter;
			mbThumbDown = true;
		}
		else if (miLButtonDownHitTest == HT_THUMB_SPACE)
		{
			SetTimer(101, 60, NULL);
			mbThumbSpaceDown = true;
		}
		else if (miLButtonDownHitTest == HT_LEFTARROW || miLButtonDownHitTest == HT_UPARROW)
		{
			SetTimer(101, 60, NULL);
			mbUpArrowDown = true;
		}
		else if (miLButtonDownHitTest == HT_RIGHTARROW || miLButtonDownHitTest == HT_DOWNARROW)
		{
			SetTimer(101, 60, NULL);
			mbDownArrowDown = true;
		}

	}
	CStatic::OnLButtonDown(aiFlags, aPoint);
}

void CScrollBarEx::OnLButtonDblClk(UINT aiFlags, CPoint aPoint)
{
	OnLButtonDown(aiFlags, aPoint);
}

void CScrollBarEx::OnLButtonUp(UINT aiFlags, CPoint aPoint)
{
	if(TRUE == misDlg)
	{
		if(true == mbThumbSpaceDown||true == mbUpArrowDown|| true ==mbDownArrowDown)
		KillTimer(102);
		miLButtonDownHitTest = HTNOWHERE;
		if ( mbLButtonDown && GetCapture() == this )
		{
			ReleaseCapture();
			mbLButtonDown = false;
			mbUpArrowDown = false;
			mbDownArrowDown = false;
			mbThumbDown = false;
			mbThumbSpaceDown = false;
			Invalidate();
		}
	}
	else
	{
		KillTimer(101);
		miLButtonDownHitTest = HTNOWHERE;

		if (mbLButtonDown && GetCapture() == this)
		{
			ReleaseCapture();
			mbLButtonDown = false;
			mbUpArrowDown = false;
			mbDownArrowDown = false;
			mbThumbDown = false;
			mbThumbSpaceDown = false;
			Invalidate();
		}

		/*CWnd* lpParent = GetParent();
		if(lpParent != NULL)
			lpParent->Invalidate();*/
	}
	
	CStatic::OnLButtonUp(aiFlags, aPoint);
}

void CScrollBarEx::OnMouseMove(UINT aiFlags, CPoint aPoint)
{
	


	
	if( TRUE == misDlg )
	{
		//LOG_TRACE(3,true,"CULScrollBar_1",itoa(aPoint.y,string,10));
		if (miLButtonDownHitTest == HT_THUMB)
		{
			//char string[10];
			//char string1[10];
			MoveThumb(mPointDragOffset);
			//int ly = aPoint.y-mPointMoveOffset.y;
			//mPointMoveOffset.y =  aPoint.y;
			//mPointOffsetSum += ly;
			//LOG_TRACE(3,true,"CULScrollBar_2",itoa(ly,string,10));
			//LOG_TRACE(3,true,"CULScrollBar_3",itoa(mPointOffsetSum,string1,10));
			
		}
	}
	else
	{
		if (!mbTrackLeave)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = this->GetSafeHwnd();
			tme.dwFlags = TME_LEAVE;
			_TrackMouseEvent(&tme);
			mbTrackLeave = TRUE;
		}

		UINT liHitTest = HitTest(aPoint);
		if (miLastHitTest != liHitTest)
			Invalidate();

		if (miLButtonDownHitTest == HT_THUMB)
			MoveThumb(mPointDragOffset);
	}

	CStatic::OnMouseMove(aiFlags, aPoint);
}

LRESULT CScrollBarEx::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	if (mbTrackLeave)
	{
		mbTrackLeave = FALSE; 
		miLastHitTest = HTNOWHERE;
		UINT liHitTest = HitTest();
		Invalidate();
	}
	return 0;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�MoveThumb
// ����������aPointOffset	�����뻬���ĵ�λ��
// �� �� ֵ��NULL
// ����˵��������϶������ƶ�����
// $_FUNCTION_END ********************************
void CScrollBarEx::MoveThumb(CPoint aPointOffset)
{
	//ȡ�õ�ǰ�������
	CPoint lPointCursor;
	GetCursorPos(&lPointCursor);
	ScreenToClient(&lPointCursor);

	RECT lrcThumb = GetRect(HT_THUMB);
	int liCursorPos = 0;	//��ǰ�������
	int liPos = 0;			//��ǰ����Ӧ�Ĺ�����λ��Postion
	int liTrackHeight = mrcClient.bottom - mrcClient.top - miButtonSize * 2 - 1;	//����߶�
	int liTrackWidth = mrcClient.right - mrcClient.left - miButtonSize * 2 - 1;		//������
	int liThumbHeight = lrcThumb.bottom - lrcThumb.top;		//����߶�
	int liThumbWidth = lrcThumb.right - lrcThumb.left;		//������
	double ldbTrackArea = mScrollInfo.nMax - mScrollInfo.nMin - mScrollInfo.nPage + 0.5;		//��������(�����Position)
	double ldbTrackLength = 0.00;		//���쳤��
	double ldbTempPos = 0.00;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE;
	if (mpWnd == NULL)
		return;

	//ȡ�ø��ؼ�����
	char szTemp[MAX_PATH];
	GetClassName(mpWnd->GetSafeHwnd(), (LPTSTR)szTemp, sizeof(szTemp));

	if (mScrollOrientation == esoVertical)
	{
		mpWnd->GetScrollInfo(SB_VERT, &si);
		ldbTrackLength = liTrackHeight - liThumbHeight;
		liCursorPos = lPointCursor.y - miButtonSize - aPointOffset.y - liThumbHeight / 2;
		ldbTempPos = (double)liCursorPos * (ldbTrackArea / ldbTrackLength);
		liPos = (int)ldbTempPos;
		ldbTempPos -= (double)liPos;
		if (ldbTempPos > 0)
			liPos ++;
		int liTimes = 0;

		if (liPos >= mScrollInfo.nMin && liPos <= mpWnd->GetScrollLimit(SB_VERT))
		{
			if (lstrcmpi( (LPTSTR)szTemp, _T("SysListView32")) == 0)		//ListCtrl��������
			{
				/*bool lbDirDown = mScrollInfo.nPos < liPos;
				CSize lSize;
				lSize.cx = 0;
				lSize.cy = (liPos - mScrollInfo.nPos) * 13;
				if (IS_ODD(lSize.cy))
					lSize.cy ++;
				((CULListView*)mpWnd)->Scroll(lSize);*/
			}
			else
			//{ 
			    #if _MSC_VER   <   1400
				NotifyControlWnd(WM_VSCROLL, (int)MAKELPARAM(SB_THUMBTRACK, liPos));
			    #else
				NotifyControlWnd(WM_VSCROLL, (int)MAKELPARAM(SB_THUMBTRACK, liPos));
                #endif
				//char string[10]; 
				//LOG_TRACE(3,true,"CULScrollBar_2",itoa(liPos,string,10));
			//}
			si.nPos = liPos;
		}
		else if (liPos < mScrollInfo.nMin)
		{
			NotifyControlWnd(WM_VSCROLL, MAKELPARAM(SB_TOP, 0));
			si.nPos = mScrollInfo.nMin;
		}
		else if (liPos > mpWnd->GetScrollLimit(SB_VERT))
		{
			NotifyControlWnd(WM_VSCROLL, MAKELPARAM(SB_BOTTOM, 0));
			si.nPos = mpWnd->GetScrollLimit(SB_VERT);
		}
		//���¹�������Ϣ
		if (false == misDlg)
		{
				SetScrollInfo(&si);
		}	
	}
	else
	{
		mpWnd->GetScrollInfo(SB_HORZ, &si);
		ldbTrackLength = liTrackWidth - liThumbWidth;
		liCursorPos = lPointCursor.x - miButtonSize - aPointOffset.x - liThumbWidth / 2;
		ldbTempPos = (double)liCursorPos * (ldbTrackArea / ldbTrackLength);
		liPos = (int)ldbTempPos;
		ldbTempPos -= (double)liPos;
		if (ldbTempPos > 0)
			liPos ++;
		int liTimes = 0;

		if (liPos >= mScrollInfo.nMin && liPos <= mpWnd->GetScrollLimit(SB_HORZ))
		{
			if (lstrcmpi( (LPTSTR)szTemp, _T("SysListView32")) == 0)		//ListCtrl��������
			{
				/*bool lbDirDown = mScrollInfo.nPos < liPos;
				CSize lSize;
				lSize.cx = (liPos - mScrollInfo.nPos);
				lSize.cy = 0;
				((CULListView*)mpWnd)->Scroll(lSize);*/
			}
			else
				NotifyControlWnd(WM_HSCROLL, (int)MAKELPARAM(SB_THUMBTRACK, liPos));
			si.nPos = liPos;
		}
		else if (liPos < mScrollInfo.nMin)
		{
			NotifyControlWnd(WM_HSCROLL, (int)MAKELPARAM(SB_TOP, 0));
			si.nPos = mScrollInfo.nMin;
		}
		else if (liPos > mpWnd->GetScrollLimit(SB_HORZ))
		{
			NotifyControlWnd(WM_HSCROLL, (int)MAKELPARAM(SB_BOTTOM, 0));
			si.nPos = mpWnd->GetScrollLimit(SB_HORZ);
		}
		//���¹�������Ϣ
		SetScrollInfo(&si);
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�TrackSpace
// ����������
// �� �� ֵ��NULL
// ����˵��������������ƶ�����Ŀ���
// $_FUNCTION_END ********************************
void CScrollBarEx::TrackSpace()
{
	CPoint lPointCursor;
	GetCursorPos(&lPointCursor);
	ScreenToClient(&lPointCursor);

	RECT lrcThumb = GetRect(HT_THUMB);
	CPoint lPointCenter;

	int liMaxTimes = mScrollInfo.nPage;
	int liTimes = 0;
	int liStep = 1;
	if (mScrollOrientation == esoVertical)
	{
		lrcThumb = GetRect(HT_THUMB);
		lPointCenter.x = lrcThumb.left + (lrcThumb.right - lrcThumb.left) / 2;
		lPointCenter.y = lrcThumb.top + (lrcThumb.bottom - lrcThumb.top) / 2;

		BOOL bDirDown = lPointCursor.y > lPointCenter.y;
		NotifyControlWnd(WM_VSCROLL, (int)MAKEWPARAM(bDirDown?SB_PAGEDOWN:SB_PAGEUP, 0));
		UpdateScrollBar();
	}
	else//�ƶ�����Ļ���
	{

		lrcThumb = GetRect(HT_THUMB);
		lPointCenter.x = lrcThumb.left + (lrcThumb.right - lrcThumb.left) / 2;
		lPointCenter.y = lrcThumb.top + (lrcThumb.bottom - lrcThumb.top) / 2;

		BOOL bDirDown = lPointCursor.x > lPointCenter.x;
		//
		NotifyControlWnd(WM_HSCROLL, (int)MAKEWPARAM(bDirDown?SB_PAGEDOWN:SB_PAGEUP, 0));
		UpdateScrollBar();
	}
}

void CScrollBarEx::OnTimer(UINT aiIDEvent)
{
	if (aiIDEvent == 102)
	{
		UINT liHitTest = HitTest();
		if (miLButtonDownHitTest == liHitTest)
		{
			if(liHitTest == HT_DOWNARROW)
			{
				NotifyControlWnd(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0));
			}
			if (liHitTest == HT_UPARROW)
			{
				NotifyControlWnd(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0));
			}
			if(liHitTest == HT_THUMB_SPACE)
			{
				CPoint lPointCursor;
				CPoint lPointCenter;

				GetCursorPos(&lPointCursor);
				ScreenToClient(&lPointCursor);

				RECT lrcThumb = GetRect(HT_THUMB);
				lPointCenter.y = lrcThumb.top +  (lrcThumb.bottom - lrcThumb.top) / 2;

				BOOL bDirDown = lPointCursor.y > lPointCenter.y;
				if(lPointCursor.y > lPointCenter.y)
				{
					NotifyControlWnd(WM_VSCROLL, (int)MAKEWPARAM(SB_PAGEDOWN, 0));
				}
				if (lPointCursor.y < lPointCenter.y)
				{
					NotifyControlWnd(WM_VSCROLL, (int)MAKEWPARAM(SB_PAGEUP, 0));
				}
			}
		}
		
	}
	if (aiIDEvent == 101)
	{
		UINT liHitTest = HitTest();
		if (miLButtonDownHitTest == liHitTest)
		{
			int liValue = SB_LINEDOWN;

			if (mScrollOrientation == esoVertical)
			{
				if (liHitTest == HT_UPARROW)
				{
					liValue = SB_LINEUP;
					NotifyControlWnd(WM_VSCROLL, (int)MAKEWPARAM(liValue, 0));
				}
				else if(liHitTest == HT_DOWNARROW)
				{
					liValue = SB_LINEDOWN;
					NotifyControlWnd(WM_VSCROLL, (int)MAKEWPARAM(liValue, 0));
				}
				else if(liHitTest == HT_THUMB_SPACE)
				{
					TrackSpace();
				}
			}
			else
			{
				if (liHitTest == HT_LEFTARROW)
				{
					liValue = SB_LINEUP;
					NotifyControlWnd(WM_HSCROLL, (int)MAKEWPARAM(liValue, 0));
				}
				else if(liHitTest == HT_RIGHTARROW)
				{
					liValue = SB_LINEDOWN;
					NotifyControlWnd(WM_HSCROLL, (int)MAKEWPARAM(liValue, 0));
				}
				else if(liHitTest == HT_THUMB_SPACE)
				{
					TrackSpace();
				}
			}
			UpdateScrollBar();
		}
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�NotifyControlWnd
// ����������aiMsg	��Ϣֵ
//			 aiValue	��Ϣ����ֵ
// �� �� ֵ��NULL
// ����˵����֪ͨ���ƴ�����Ӧ����Ϣ
// $_FUNCTION_END ********************************
void CScrollBarEx::NotifyControlWnd(UINT aiMsg, int aiValue)
{
	if (mpWnd)
		mpWnd->SendMessage(aiMsg, aiValue, 0);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetControlWnd
// ����������apWnd	������
// �� �� ֵ��
// ����˵�������ÿ��ƴ�����
// $_FUNCTION_END ********************************
void CScrollBarEx::SetControlWnd(CWnd* apWnd)
{
	mpWnd = apWnd;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetPosition
// ����������
// �� �� ֵ��NULL
// ����˵�������ù�������λ��
// $_FUNCTION_END ********************************
void CScrollBarEx::SetPosition()
{
	if (mpWnd == NULL)
		return;

	CRect lrcWindow;
	mpWnd->GetWindowRect(&lrcWindow);
	CWnd* lpParent = NULL;
	lpParent = GetParent();
	if (lpParent == NULL)
		return;
   

	TCHAR   szBuf[32];  
	GetClassName(mpWnd->m_hWnd,   szBuf,   sizeof(szBuf)/sizeof(TCHAR))  ;
    CString lstr = szBuf;
	if( ( 0 == lstr.Compare( "AfxFrameOrView80ud")) || (0 ==  lstr.Compare( "AfxFrameOrView70d") ) ||
		( 0 == lstr.Compare( "AfxFrameOrView80u")) || (0 ==  lstr.Compare( "AfxFrameOrView70") ) )
	{
		misDlg = TRUE;
		mpWnd->ModifyStyle(NULL, WS_CLIPSIBLINGS);

	}
	else
	{
		mpWnd->ModifyStyle(NULL, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}
	
	mpWnd->SetWindowPos(this, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	
	//ȡ�ñ߿�ĸ߶ȺͿ��
	LONG lExStyle = GetWindowLong(mpWnd->GetSafeHwnd(), GWL_EXSTYLE);
	int liBorderHeight = 0;
	int liBorderWidth = 0;
	if (lExStyle & WS_EX_CLIENTEDGE)
	{
		liBorderHeight = GetSystemMetrics(SM_CYEDGE);
		liBorderWidth = GetSystemMetrics(SM_CXEDGE);
	}

	//ȡ�þ�����������
	lpParent->ScreenToClient(&lrcWindow);

	//ȡ�ù�������ʾ״̬
	DWORD dwStyles = mpWnd->GetStyle();
	bool lbVertical, lbHorizontal;
	if (dwStyles & WS_VSCROLL)
		lbVertical = true;
	else
		lbVertical = false;

	if (dwStyles & WS_HSCROLL)
		lbHorizontal = true;
	else
		lbHorizontal = false;

	//ȡ�ù������հ������С
	int liVerticleSpaceSize = 0;
	int liHorizontalSpaceSize = 0;
	int liRightBottomSpaceSize = 0;
	if (lbVertical)
		liHorizontalSpaceSize = miButtonSize;
	else
		liHorizontalSpaceSize = 0;
	if (lbHorizontal)
		liVerticleSpaceSize = miButtonSize;
	else
		liVerticleSpaceSize = 0;
	if (lbVertical && lbHorizontal)
		liRightBottomSpaceSize = 1;

	//��������������ͺ��������������
	CRect lrcVerticleBar, lrcHorizontalBar;
	if (mScrollOrientation == esoVertical)
	{
		lrcVerticleBar.left = lrcWindow.right - liHorizontalSpaceSize - liBorderWidth;
		lrcVerticleBar.top = lrcWindow.top + liBorderHeight;
		lrcVerticleBar.right = lrcWindow.right - liBorderWidth;
		lrcVerticleBar.bottom = lrcWindow.bottom - liVerticleSpaceSize - liBorderHeight - liRightBottomSpaceSize + 1;
		if (lbVertical)
		{
			if (mpWnd->IsWindowVisible())
				ShowWindow(SW_SHOW);
		}
		else
			ShowWindow(SW_HIDE);
		MoveWindow(lrcVerticleBar);
	}
	else
	{
		lrcHorizontalBar.left = lrcWindow.left + liBorderWidth;
		lrcHorizontalBar.top = lrcWindow.bottom - liVerticleSpaceSize - liBorderHeight;
		lrcHorizontalBar.right = lrcWindow.right - liHorizontalSpaceSize - liBorderWidth - liRightBottomSpaceSize + 1;
		lrcHorizontalBar.bottom = lrcWindow.bottom - liBorderHeight;
		if (lbHorizontal)
		{
			if (mpWnd->IsWindowVisible())
				ShowWindow(SW_SHOW);
		}
		else
			ShowWindow(SW_HIDE);
		MoveWindow(lrcHorizontalBar);
	}
	Invalidate();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�UpdateScrollBar
// ����������
// �� �� ֵ��NULL
// ����˵�������¹�������Ϣ���ػ�
// $_FUNCTION_END ********************************
void CScrollBarEx::UpdateScrollBar()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (mpWnd == NULL)
		return;

	if (mScrollOrientation == esoVertical)
		mpWnd->GetScrollInfo(SB_VERT, &si);
	else
		mpWnd->GetScrollInfo(SB_HORZ, &si);

	if (si.nMax <= 1)	//��ʱ����Ϊ����
		mbEnabled = false;
	else
		mbEnabled = true;

	SetScrollInfo(&si);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�Draw
// ����������hWnd	������
//			 hDC	�滭�豸
// �� �� ֵ��NULL
// ����˵�������ƿ���
// $_FUNCTION_END ********************************
void CScrollBarEx::Draw(HWND hWnd, HDC hDC)
{
	::GetWindowRect(hWnd, &mrcClient);
	ScreenToClient(&mrcClient);

	DrawThumbSpace(hDC, GetRect(HT_THUMB_SPACE));
	DrawThumb(hDC, GetRect(HT_THUMB));

	if (mScrollOrientation == esoVertical)
	{
		DrawUpBtn(hDC, GetRect(HT_UPARROW));
		DrawDownBtn(hDC, GetRect(HT_DOWNARROW));
	}
	else
	{
		DrawLeftBtn(hDC, GetRect(HT_LEFTARROW));
		DrawRightBtn(hDC, GetRect(HT_RIGHTARROW));
	}


	
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawLeftBtn
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�������������ͷ��ť
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawLeftBtn(HDC hDC, RECT aRect)
{
	miLastHitTest = HitTest();

	if (miLastHitTest == HT_LEFTARROW && !mbLButtonDown)		//����
		DrawHighlightFace(hDC, aRect, 0);
	else if (miLastHitTest == HT_LEFTARROW && mbUpArrowDown)	//����
		DrawDownFace(hDC, aRect, 0);
	else	//Ĭ��
		DrawNormalFace(hDC, aRect, 0);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawRightBtn
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�����������Ҽ�ͷ��ť
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawRightBtn(HDC hDC, RECT aRect)
{
	miLastHitTest = HitTest();

	if (miLastHitTest == HT_RIGHTARROW && !mbLButtonDown)		//����
		DrawHighlightFace(hDC, aRect, 1);
	else if (miLastHitTest == HT_RIGHTARROW && mbDownArrowDown)	//����
		DrawDownFace(hDC, aRect, 1);
	else	//Ĭ��
		DrawNormalFace(hDC, aRect, 1);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawUpBtn
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�����������ϼ�ͷ��ť
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawUpBtn(HDC hDC, RECT aRect)
{
	miLastHitTest = HitTest();

	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.left-2,aRect.top-2,aRect.right+1,aRect.top-2,false);
	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.left-2,aRect.top-2,aRect.left-2,aRect.bottom+1,false);
	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.right+1,aRect.top-2,aRect.right+1,aRect.bottom+1,false);
		if (miLastHitTest == HT_UPARROW && !mbLButtonDown)		//����
		DrawHighlightFace(hDC, aRect, 2);
	else if (miLastHitTest == HT_UPARROW && mbUpArrowDown)	//����
		DrawDownFace(hDC, aRect, 2);
	else	//Ĭ��
		DrawNormalFace(hDC, aRect, 2);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawDownBtn
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�����������¼�ͷ��ť
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawDownBtn(HDC hDC, RECT aRect)
{
	miLastHitTest = HitTest();

	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.left-2,aRect.bottom+1,aRect.right+2,aRect.bottom+1,false);
	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.left-2,aRect.top-2,aRect.left-2,aRect.bottom+1,false);
	gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.right+1,aRect.top-2,aRect.right+1,aRect.bottom+1,false);
		if (miLastHitTest == HT_DOWNARROW && !mbLButtonDown)		//����
		DrawHighlightFace(hDC, aRect, 3);
	else if (miLastHitTest == HT_DOWNARROW && mbDownArrowDown)	//����
		DrawDownFace(hDC, aRect, 3);
	else	//Ĭ��
		DrawNormalFace(hDC, aRect, 3);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawThumb
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�������ƻ���
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawThumb(HDC hDC, RECT aRect)
{	
	miLastHitTest = HitTest();
	RECT lrcBK = aRect;
	if (mScrollOrientation == esoVertical)
	{
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.right+1,aRect.top-2,aRect.right+1,aRect.bottom,false);
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,aRect.left-2,aRect.top-1,aRect.left-2,aRect.bottom,false);
	}

	if (miLastHitTest == HT_THUMB && !mbLButtonDown)		//����
		DrawHighlightFace(hDC, aRect, 4);
	else if (miLastHitTest != HT_LEFTARROW &&
		miLastHitTest != HT_RIGHTARROW &&
		miLastHitTest != HT_UPARROW &&
		miLastHitTest != HT_DOWNARROW &&
		miLastHitTest != HT_THUMB_SPACE &&
		mbThumbDown)	//����
		DrawDownFace(hDC, aRect, 4);
	else if(!mbThumbDown)	//Ĭ��
		DrawNormalFace(hDC, aRect, 4);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawThumbSpace
// ����������hDC	�滭�豸
//			 aRect	�滭����
// �� �� ֵ��NULL
// ����˵�������ƻ���
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawThumbSpace(HDC hDC, RECT aRect)
{
	RECT lrcSpaceBegin, lrcSpaceEnd, lrcThumb, lrcBK;
	lrcThumb = GetRect(HT_THUMB);
	lrcSpaceBegin = lrcSpaceEnd = lrcBK = aRect;
	lrcBK.left -=1; 
	lrcBK.right+= 1;
	if (mScrollOrientation == esoVertical)
	{
		lrcSpaceBegin.bottom = lrcThumb.top-1;
	//	if (lrcThumb.bottom > 0)
		//	lrcSpaceEnd.top = lrcThumb.bottom;
	//	else
		//	lrcSpaceEnd.top = miButtonSize;
		lrcSpaceEnd.top = lrcThumb.bottom+1;

		//gBitMapOperate.FillRectangle(hDC, lrcBK, RGB(255,0,255), FALSE);
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,lrcSpaceBegin.left-1,lrcSpaceBegin.top,lrcSpaceBegin.left-1,lrcSpaceBegin.bottom-1,false);
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,lrcSpaceBegin.right,lrcSpaceBegin.top,lrcSpaceBegin.right,lrcSpaceBegin.bottom-1,false);
		gBitMapOperate.FillRectangle(hDC, lrcSpaceBegin, 0xf5f5f5, FALSE);
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,lrcSpaceEnd.left-1,lrcSpaceEnd.top,lrcSpaceEnd.left-1,lrcSpaceEnd.bottom-1,false);
		gBitMapOperate.DrawLine(hDC,RGB(255,255,255),1,lrcSpaceEnd.right,lrcSpaceEnd.top,lrcSpaceEnd.right,lrcSpaceEnd.bottom-1,false);
		gBitMapOperate.FillRectangle(hDC, lrcSpaceEnd,   0xf5f5f5, FALSE);
	}
	else
	{
		lrcSpaceBegin.right = lrcThumb.left;
		if (lrcThumb.right > 0)
			lrcSpaceEnd.left = lrcThumb.right;
		else
			lrcSpaceEnd.left = miButtonSize;
		gBitMapOperate.FillRectangle(hDC, lrcSpaceBegin, RGB(241, 241, 241), FALSE);
		gBitMapOperate.FillRectangle(hDC, lrcSpaceEnd, RGB(241, 241, 241), FALSE);
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawHighlightFace
// ����������hDC	�滭�豸
//			 aRect	�滭����
//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
// �� �� ֵ��NULL
// ����˵�������ƽ���ʱ����
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawHighlightFace(HDC hDC, RECT aRect, int aiType)
{
	if (!mbEnabled)
		return;
	InflateRect(&aRect, 1, 1);
	RECT lre1 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre2 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre3 = aRect;

	switch(aiType)
	{
	case 0:		//���ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+4, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+3, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+3, lre2.top+6, lre2.left+7, lre2.top+10, FALSE);

		}
		break;
	case 1:		//�Ҽ�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+8, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+8, lre2.top+6, lre2.left+5, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+9, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+9, lre2.top+6, lre2.left+5, lre2.top+10, FALSE);
		}
		break;
	case 2:		//�ϼ�ͷ
		{

			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			lre2.left = lre2.left -1;
			lre2.top = lre2.top -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+7, lre2.left+7, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+5, lre2.left+10, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+7, lre2.left+7, lre2.top+4, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+4, lre2.left+11, lre2.top+8, FALSE);

		}
		break;
	case 3:		//�¼�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			lre2.left = lre2.left -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+6, lre2.left+7, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+8, lre2.left+10, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+9, lre2.left+11, lre2.top+5, FALSE);

		}
		break;
	case 4:		//��������
		if (mScrollOrientation == esoVertical)
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolEnter1,mcolEnter2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);
			// ������
			if( lre2.bottom >= 15 )
			{
				int liLift = lre2.top + (lre2.bottom - 11 -lre2.top)/2 + 1;

				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift,   lre2.right-3, liLift, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift+4, lre2.right-3, liLift+4, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift+8, lre2.right-3, liLift+8, FALSE);
			}


		}
		else	//esoHorizontal
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolEnter1,mcolEnter2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			// ������
			if( lre2.right >= 15 )
			{
				int liLift = lre2.left + (lre2.right - 11 -lre2.left)/2 + 1;

				gBitMapOperate.DrawLine(lDC,mcolBK,  1, liLift, lre2.top+2, liLift, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK,  1, liLift+4, lre2.top+2, liLift+4, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC,mcolBK,  1, liLift+8, lre2.top+2, liLift+8, lre2.bottom-3, FALSE);
			}

		}

		break;
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawDownFace
// ����������hDC	�滭�豸
//			 aRect	�滭����
//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
// �� �� ֵ��NULL
// ����˵����������갴��ʱ����
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawDownFace(HDC hDC, RECT aRect, int aiType)
{
	if (!mbEnabled)
		return;

	InflateRect(&aRect, 1, 1);
	RECT lre1 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre2 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre3 = aRect;

	switch(aiType)
	{
	case 0:		//���ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);


			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+4, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+3, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+3, lre2.top+6, lre2.left+7, lre2.top+10, FALSE);

		}
		break;
	case 1:		//�Ҽ�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+8, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+8, lre2.top+6, lre2.left+5, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+9, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+9, lre2.top+6, lre2.left+5, lre2.top+10, FALSE);
	}
		break;
	case 2:		//�ϼ�ͷ
		{

			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			lre2.left = lre2.left -1;
			lre2.top = lre2.top -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+7, lre2.left+7, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+5, lre2.left+10, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+7, lre2.left+7, lre2.top+4, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+4, lre2.left+11, lre2.top+8, FALSE);

		}
		break;
	case 3:		//�¼�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			lre2.left = lre2.left -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+6, lre2.left+7, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+8, lre2.left+10, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+9, lre2.left+11, lre2.top+5, FALSE);

		}
		break;
	case 4:		//��������
		if (mScrollOrientation == esoVertical)
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolDown1,mcolDown2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);
			// ������
			if( lre2.bottom >= 15 )
			{
				int liLift = lre2.top + (lre2.bottom - 11 -lre2.top)/2 + 1;

				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift,   lre2.right-3, liLift, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift+4, lre2.right-3, liLift+4, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK,  1, lre2.left+2, liLift+8, lre2.right-3, liLift+8, FALSE);
			}


		}
		else	//esoHorizontal
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolDown1,mcolDown2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			// ������
			if( lre2.right >= 15 )
			{
				int liLift = lre2.left + (lre2.right - 11 -lre2.left)/2 + 1;

				gBitMapOperate.DrawLine(lDC,mcolBK,  1, liLift, lre2.top+2, liLift, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolBK, 1, liLift+4, lre2.top+2, liLift+4, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC,mcolBK,  1, liLift+8, lre2.top+2, liLift+8, lre2.bottom-3, FALSE);
			}

		}
		break;
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�DrawNormalFace
// ����������hDC	�滭�豸
//			 aRect	�滭����
//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
// �� �� ֵ��NULL
// ����˵��������Ĭ�ϱ���
// $_FUNCTION_END ********************************
void CScrollBarEx::DrawNormalFace(HDC hDC, RECT aRect, int aiType)
{
	InflateRect(&aRect, 1, 1);
	RECT lre1 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre2 = aRect;
	InflateRect(&aRect, -1, -1);
	RECT lre3 = aRect;

	switch(aiType)
	{
	case 0:		//���ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);


			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+4, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+3, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+3, lre2.top+6, lre2.left+7, lre2.top+10, FALSE);

		}
		break;
	case 1:		//�Ҽ�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+4, lre2.left+8, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+8, lre2.top+6, lre2.left+5, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+6, lre2.top+3, lre2.left+9, lre2.top+6, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+9, lre2.top+6, lre2.left+5, lre2.top+10, FALSE);

		}
		break;
	case 2:		//�ϼ�ͷ
		{

			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn1,mcolBtn2,gBitMapOperate.edoVertical,FALSE);

			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);


			lre2.left = lre2.left -1;
			lre2.top = lre2.top -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+7, lre2.left+7, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+5, lre2.left+10, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+7, lre2.left+7, lre2.top+4, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+4, lre2.left+11, lre2.top+8, FALSE);

		}
		break;
	case 3:		//�¼�ͷ
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolBtn2,mcolBtn1,gBitMapOperate.edoVertical,FALSE);
			
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);
			lre2.left = lre2.left -1;
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+5, lre2.top+6, lre2.left+7, lre2.top+8, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+8, lre2.left+10, lre2.top+5, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+4, lre2.top+6, lre2.left+7, lre2.top+9, FALSE);
			gBitMapOperate.DrawLine(lDC, mcolFont, 1,  lre2.left+7, lre2.top+9, lre2.left+11, lre2.top+5, FALSE);
		
		}
		break;
	case 4:		//��������
		if (mScrollOrientation == esoVertical)
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolNormal1,mcolNormal2,gBitMapOperate.edoVertical,FALSE);

			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			// ������
			if( lre2.bottom  >= 15 )
			{
				int liLift = lre2.top + (lre2.bottom - 11 -lre2.top)/2 + 1;

				gBitMapOperate.DrawLine(lDC, mcolFont,  1, lre2.left+2, liLift,   lre2.right-3, liLift, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolFont,  1, lre2.left+2, liLift+4, lre2.right-3, liLift+4, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolFont,  1, lre2.left+2, liLift+8, lre2.right-3, liLift+8, FALSE);
			}


		}
		else	//esoHorizontal
		{
			CMemDC lDC(CDC::FromHandle(hDC), &lre1);
			gBitMapOperate.FillRectangle(lDC, lre1, mcolFrame1, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre2, mcolFrame2, FALSE);
			gBitMapOperate.FillRectangle(lDC, lre3, mcolNormal1,mcolNormal2,gBitMapOperate.edoVertical,FALSE);
			SetPixel(lDC,lre1.left,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.top, mcolNormal1);
			SetPixel(lDC,lre1.right -1,lre1.bottom-1, mcolNormal1);
			SetPixel(lDC,lre1.left,lre1.bottom-1, mcolNormal1);

			// ������
			if( lre2.right >= 15 )
			{
				int liLift = lre2.left + (lre2.right - 11 -lre2.left)/2 + 1;

				gBitMapOperate.DrawLine(lDC,mcolFont,  1, liLift, lre2.top+2, liLift, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC, mcolFont,  1, liLift+4, lre2.top+2, liLift+4, lre2.bottom-3, FALSE);
				gBitMapOperate.DrawLine(lDC,mcolFont,  1, liLift+8, lre2.top+2, liLift+8, lre2.bottom-3, FALSE);
			}

		}
		break;
	}
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�HitTest
// ����������aPoint	��������
// �� �� ֵ�����Խ��(�����������Ŀ�������)
// ����˵�������Կ�������
// $_FUNCTION_END ********************************
UINT CScrollBarEx::HitTest(CPoint aPoint)
{
	UINT liIDArr[] = {HT_UPARROW, HT_DOWNARROW, HT_LEFTARROW, HT_RIGHTARROW, HT_THUMB, HT_THUMB_SPACE};
	for (int i = 0; i < COUNT_OF(liIDArr); i++)
	{
		RECT lrcTest = GetRect(liIDArr[i]);
		if (PtInRect(&lrcTest, aPoint))
			return liIDArr[i];
		//�����������ΪHT_THUMB_SPACE��ʱ��

	}
	return HTNOWHERE;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�HitTest
// ����������
// �� �� ֵ�����Խ��(�����������Ŀ�������)
// ����˵�������Ե�ǰ�������λ�õĿ�������
// $_FUNCTION_END ********************************
UINT CScrollBarEx::HitTest()
{
	CPoint lPoint;
	GetCursorPos(&lPoint);
	ScreenToClient(&lPoint);
	return HitTest(lPoint);
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetRect
// ����������aiHitTest
// �� �� ֵ����������
// ����˵��������ֵ��Ӧ�Ŀ�������
// $_FUNCTION_END ********************************
RECT CScrollBarEx::GetRect(UINT aiHitTest)
{
	RECT lrcResult;
	lrcResult.left = 0;
	lrcResult.top = 0;
	lrcResult.right = 0;
	lrcResult.bottom = 0;

	switch(aiHitTest) 
	{
	case HT_UPARROW:
		{
			if (mScrollOrientation == esoVertical)
			{
				lrcResult.left = 2;
				lrcResult.top = 2;
				lrcResult.right = mrcClient.right - 2;
				lrcResult.bottom = miButtonSize - 2;
			}
		}
		break;
	case HT_DOWNARROW:
		{
			if (mScrollOrientation == esoVertical)
			{
				lrcResult.left = 2;
				lrcResult.top = mrcClient.bottom - miButtonSize + 1;
				lrcResult.right = mrcClient.right - 2;
				lrcResult.bottom = mrcClient.bottom - 3;
			}
		}
		break;
	case HT_LEFTARROW:
		{
			if (mScrollOrientation == esoHorizontal)
			{
				lrcResult.left = 1;
				lrcResult.top = 2;
				lrcResult.right = miButtonSize - 2;
				lrcResult.bottom = mrcClient.bottom - 2;
			}
		}
		break;
	case HT_RIGHTARROW:
		{
			if (mScrollOrientation == esoHorizontal)
			{
				lrcResult.left = mrcClient.right - miButtonSize + 2;
				lrcResult.top = 2;
				lrcResult.right = mrcClient.right - 1;
				lrcResult.bottom = mrcClient.bottom - 2;
			}
		}
		break;
	case HT_THUMB:
		{
			SCROLLINFO &si = mScrollInfo;
			int liThumbSpace;
			int liLength;
			int liCenter;

			if (si.nMax > si.nMin && si.nMax > 1)
			{
				if (mScrollOrientation == esoVertical)
				{
					lrcResult.left = 2;
					lrcResult.right = mrcClient.right - 2;
					liThumbSpace = (mrcClient.bottom - mrcClient.top) - miButtonSize * 2;
					liLength = (int)(liThumbSpace * si.nPage / (si.nMax - si.nMin + 1));
					liCenter = (int)(liThumbSpace * (si.nPos + si.nPage) / (si.nMax - si.nMin + 1));
					if (liLength > 5)
					{
						lrcResult.top = miButtonSize + liCenter - liLength;
						lrcResult.bottom = miButtonSize + liCenter;
					}
					else if (miButtonSize + liCenter + 2 < mrcClient.bottom - miButtonSize)
					{
						lrcResult.top = miButtonSize + liCenter - liLength;
						lrcResult.bottom = miButtonSize + liCenter + 5 - liLength;
					}
					else
					{
						lrcResult.top = mrcClient.bottom - miButtonSize - 5;
						lrcResult.bottom = mrcClient.bottom - miButtonSize;
					}
				}
				else
				{
					lrcResult.top = 2;
					lrcResult.bottom = mrcClient.bottom - 2;
					liThumbSpace = (mrcClient.right - mrcClient.left) - miButtonSize * 2;
					liLength = (int)(liThumbSpace * si.nPage / (si.nMax - si.nMin + 1));
					liCenter = (int)(liThumbSpace * (si.nPos + si.nPage) / (si.nMax - si.nMin + 1));
					lrcResult.left = miButtonSize + liCenter - liLength;
					lrcResult.right = miButtonSize + liCenter;
				}
			}
		}
		break;
	case HT_THUMB_SPACE:
		{
			if (mScrollOrientation == esoVertical)
			{
				lrcResult.left = 1;
				lrcResult.top = miButtonSize-1 ;
				lrcResult.right = mrcClient.right-1;
				//lrcResult.bottom = (mrcClient.bottom - mrcClient.top) - miButtonSize + 1;
				lrcResult.bottom = mrcClient.bottom - miButtonSize;
			}
			else
			{
				lrcResult.left = miButtonSize -1;
				lrcResult.top = 1;
				lrcResult.right = (mrcClient.right - mrcClient.left) - miButtonSize +1;
				lrcResult.bottom = mrcClient.bottom-2;
			}
		}
		break;
	}

	return lrcResult;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetScrollInfo
// ����������aScrollInfo	��������Ϣ
//			 aiMask		��ȡ���ѡ��
// �� �� ֵ���Ƿ�ɹ�
// ����˵������ȡ��������Ϣ
// $_FUNCTION_END ********************************
BOOL CScrollBarEx::GetScrollInfo(LPSCROLLINFO aScrollInfo, UINT aiMask)
{
	memcpy(aScrollInfo, &mScrollInfo, sizeof(mScrollInfo));
	return TRUE;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetScrollLimit
// ����������
// �� �� ֵ��������λ��
// ����˵������ȡ������λ��
// $_FUNCTION_END ********************************
int CScrollBarEx::GetScrollLimit()
{
	return mScrollInfo.nMax;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetScrollPos
// ����������
// �� �� ֵ������λ��
// ����˵������ȡ����λ��
// $_FUNCTION_END ********************************
int CScrollBarEx::GetScrollPos()
{
	return mScrollInfo.nPos;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetScrollRange
// ����������aiMinPos	������Сֵ
//			 aiMaxPos	�������ֵ
// �� �� ֵ��
// ����˵������ȡ������Χ
// $_FUNCTION_END ********************************
void CScrollBarEx::GetScrollRange(LPINT aiMinPos, LPINT aiMaxPos)
{
	memcpy(aiMinPos, &mScrollInfo.nMin, sizeof(mScrollInfo.nMin));
	memcpy(aiMaxPos, &mScrollInfo.nMax, sizeof(mScrollInfo.nMin));
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�GetScrollOrientation
// ����������
// �� �� ֵ������������
// ����˵������ȡ����������
// $_FUNCTION_END ********************************
ENUM_ScrollOrientation CScrollBarEx::GetScrollOrientation()
{
	return mScrollOrientation;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetScrollInfo
// ����������aScrollInfo	��������Ϣ
//			 abRedraw	�Ƿ��ػ�
// �� �� ֵ���Ƿ�ɹ�
// ����˵�������ù�������Ϣ
// $_FUNCTION_END ********************************
BOOL CScrollBarEx::SetScrollInfo(LPSCROLLINFO aScrollInfo, BOOL abRedraw)
{
	const SCROLLINFO& si = mScrollInfo;
	if (si.nMax != aScrollInfo->nMax
		|| si.nMin != aScrollInfo->nMin
		|| si.nPage != aScrollInfo->nPage
		|| si.nPos != aScrollInfo->nPos
		|| si.nTrackPos != aScrollInfo->nTrackPos)
	{
	}
	else
		return TRUE;

	memcpy(&mScrollInfo, aScrollInfo, sizeof(mScrollInfo));

	if (abRedraw)
		RedrawWindow();

	return TRUE;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetScrollPos
// ����������aiPos	������λ��
//			 abRedraw	�Ƿ��ػ�
// �� �� ֵ�����óɹ���λ��
// ����˵�������ù�����λ��
// $_FUNCTION_END ********************************
int CScrollBarEx::SetScrollPos(int aiPos, BOOL abRedraw)
{
	mScrollInfo.nPos = aiPos;
	if (abRedraw)
		Invalidate();
	return mScrollInfo.nPos;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetScrollRange
// ����������aiMinPos	������Сֵ
//			 aiMaxPos	�������ֵ
//			 abRedraw	�Ƿ��ػ�
// �� �� ֵ��
// ����˵�������ù�������Χ
// $_FUNCTION_END ********************************
void CScrollBarEx::SetScrollRange(int aiMinPos, int aiMaxPos, BOOL abRedraw)
{
	mScrollInfo.nMin = aiMinPos;
	mScrollInfo.nMax = aiMaxPos;
	if (abRedraw)
		Invalidate();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�SetScrollOrientation
// ����������aScrollOrientation		��������
// �� �� ֵ��
// ����˵�������ù���������
// $_FUNCTION_END ********************************
void CScrollBarEx::SetScrollOrientation(ENUM_ScrollOrientation aScrollOrientation)
{
	mScrollOrientation = aScrollOrientation;
}


LRESULT CScrollBarEx::OnDlgMouseWheel(WPARAM wParam, LPARAM lParam)
{
	return SetScrollPos((int)lParam,TRUE);
	
}
int CScrollBarEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	mcolNormal1 = gWndColor.getColor(SCROLLBAR_CTROL)->colNormal1 ;
	mcolNormal2 = gWndColor.getColor(SCROLLBAR_CTROL)->colNormal2;
	mcolEnter1 = gWndColor.getColor(SCROLLBAR_CTROL)->colEnter1;
	mcolEnter2 = gWndColor.getColor(SCROLLBAR_CTROL)->colEnter2;
	mcolDown1 = gWndColor.getColor(SCROLLBAR_CTROL)->colDown1;
	mcolDown2 = gWndColor.getColor(SCROLLBAR_CTROL)->colDown2;
	mcolFrame1 = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFrame1;
	mcolFrame2 = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFrame2;
	mcolBtn1 = gWndColor.getColor(SCROLLBAR_CTROL)->colDisable1;
	mcolBtn2 = gWndColor.getColor(SCROLLBAR_CTROL)->colDisable2;
	mcolFont = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colFont ;
	mcolBK = gWndColor.getColor(SCROLLBAR_CTROL)->structColBase->colBK;

	return 0;
}
