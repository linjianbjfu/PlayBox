#include "stdafx.h"
#include "WindowsMover.h"
#include "HostWnd.h"

CWindowsMover* CWindowsMover::m_pMover = NULL;

CWindowsMover * CWindowsMover::GetInstance()
{
	if(m_pMover == NULL)
	{
		m_pMover = new CWindowsMover();
	}

	return m_pMover;
}

void CWindowsMover::DelInstance()
{
	delete m_pMover;
}

CWindowsMover::CWindowsMover()
{
	m_bMouseDown = false;
	m_bDontMoveContent = false;
}

CWindowsMover::~CWindowsMover()
{
}

void CWindowsMover::AddWndindow(CWnd *pWnd, bool isMain)
{
	WndMoveData data;
	data.pWnd = pWnd;
	data.isMain = isMain;
	m_mapWndMoveData[pWnd->m_hWnd] = data;
}

void CWindowsMover::Clear()
{
	m_mapWndMoveData.clear();
}

void CWindowsMover::GetAllWindowsRect()
{

}

void CWindowsMover::CalMovingWndNewPos()
{
	map<HWND, WndMoveData>::iterator it = m_mapWndMoveData.begin();

	while(it != m_mapWndMoveData.end())
	{
		CRect rc;
		if(it->second.nState == STATE_MOVING)
		{
			TRACE("offset:%d , %d\n", m_offsetX, m_offsetY);
			it->second.rc.OffsetRect(m_offsetX, m_offsetY);
		}
		it++;
	}
}

void CWindowsMover::DoMouseDown(HWND hwnd, CPoint p)
{
//	ClientToScreen(hwnd, &p);
	m_ptMouseDownPos = p;
	m_bMouseDown = true;
	SetCapture(hwnd);
	bool isMain = m_mapWndMoveData[hwnd].isMain;
	CRect rcMain ;//= m_mapWndMoveData[hwnd].rc;
	GetWindowRect(hwnd, &rcMain);

	if(m_bDontMoveContent)
	{
		m_rcKeep = rcMain;
		DrawMoveFrame(m_rcKeep);
		return;
	}

	map<HWND, WndMoveData>::iterator it;
	map<HWND, WndMoveData>::iterator it1;

	it = m_mapWndMoveData.begin();
	while(it != m_mapWndMoveData.end())
	{
		CRect rc;
		WndMoveData data = it->second;
		::GetWindowRect(data.pWnd->m_hWnd, &data.rc);
		it->second = data;
		if(it->second.pWnd->m_hWnd == hwnd)
			m_mapWndMoveData[hwnd].nState = STATE_MOVING;
		else
			it->second.nState = STATE_STAND;
		it++;
	}

	bool bFind = false;
	if(isMain)
	{
		it = m_mapWndMoveData.begin();
		while(it != m_mapWndMoveData.end())
		{
			if(it->second.nState != STATE_MOVING)
			{
				it1 = m_mapWndMoveData.begin();
				while(it1 != m_mapWndMoveData.end())
				{
					if(it1->second.nState == STATE_MOVING
						&& IsTwoWndStucked(it->second.rc, it1->second.rc, rcMain))
					{
						it->second.nState = STATE_MOVING;
						bFind = true;
						break;
					}
					it1 ++;
				}

			}
			if(bFind == true)
			{
				it = m_mapWndMoveData.begin();
				bFind = false;
			}
			else
				it++;
		}
	}
}

BOOL CWindowsMover::IsTwoWndStucked(CRect rc, CRect rcTwo, CRect rcMain)
{
	bool bVertJoined = max(rc.bottom, rcTwo.bottom) - min(rc.top , rcTwo.top) <= rc.Height() + rcTwo.Height() + 1;
	bool bHorzJoined = max(rc.right, rcTwo.right) - min(rc.left, rcTwo.left) <= rc.Width() + rcTwo.Width() + 1;

	// 用其中一个矩形来创建一个CRgn对象, 哪个都行
	CRgn rgn;
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

	// 然后就可以判断了
	if (rgn.RectInRegion(&rcTwo) || rgn.RectInRegion(rcMain))
	{
		rgn.DeleteObject();
		return false;
	}
	rgn.DeleteObject();

	if(
		(rc.left  == rcTwo.left && bVertJoined)	||
		(rc.left  == rcTwo.right	&& bVertJoined)	||
		(rc.right == rcTwo.left	&& bVertJoined)	||
		(rc.right == rcTwo.right && bVertJoined)	||
		(rc.top   == rcTwo.top && bHorzJoined	)	||
		(rc.top	  == rcTwo.bottom && bHorzJoined)	||
		(rc.bottom== rcTwo.top && bHorzJoined)		||
		(rc.bottom== rcTwo.bottom && bHorzJoined)
	)
		return true;
	else
		return false;
}

void CWindowsMover::DoMouseUp(HWND hwnd, CPoint p)
{

	ReleaseCapture();

	if(m_bDontMoveContent && m_bMouseDown)
	{
		CRect rc;

		m_offsetX = p.x-m_ptMouseDownPos.x;
		m_offsetY = p.y-m_ptMouseDownPos.y;

		GetWindowRect(hwnd, &rc);
		rc.OffsetRect(m_offsetX, m_offsetY);

		DrawMoveFrame(&m_rcKeep);

		m_mapWndMoveData[hwnd].pWnd->MoveWindow(&rc);
		m_bMouseDown = false;
		return;
	}
	m_bMouseDown = false;
	MovingAllWindows();
}

void CWindowsMover::DoMouseMove(HWND hWnd ,UINT nFlags, CPoint p)
{
	//ClientToScreen(hWnd, &p);
	if(!m_bMouseDown)
		return;

	int nMinX, nMinY;
	if(!(nFlags & MK_LBUTTON))
	{
		return;
	}

	if(m_bDontMoveContent)
	{
		CRect rc;

		m_offsetX = p.x-m_ptMouseDownPos.x;
		m_offsetY = p.y-m_ptMouseDownPos.y;

		GetWindowRect(hWnd, &rc);
		rc.OffsetRect(m_offsetX, m_offsetY);

		DrawMoveFrame(&m_rcKeep);
		m_rcKeep = rc;
		DrawMoveFrame(&rc);
		return;
	}

	if(m_ptLastMousePos == p)
		return;

	if(abs(m_ptLastMousePos.x - p.x) < 3 && abs(m_ptLastMousePos.y - p.y) < 3)
		return;

	DWORD count = GetTickCount();
	m_ptLastMousePos = p;
	m_hCurWnd = hWnd;
	nMinX = nMinY = 0;
	m_offsetX = p.x-m_ptMouseDownPos.x;
	m_offsetY = p.y-m_ptMouseDownPos.y;
	CalMovingWndNewPos();
	CalMinDistance(nMinX, nMinY);
	ModifyMovingWndPos(nMinX, nMinY);
	MovingAllWindows();

	//TRACE("%d\n", GetTickCount() - count);
}

void CWindowsMover::DoMouseResizingFinished(HWND hWnd ,UINT nFlags, CPoint p,int iDirection)
{
	int nMinX, nMinY;

//	ClientToScreen(hWnd, &p);
	if(m_ptLastMousePos == p)
		return;

	if(abs(m_ptLastMousePos.x - p.x) < 3 && abs(m_ptLastMousePos.y - p.y) < 3)
		return;

	DWORD count = GetTickCount();

	m_ptLastMousePos = p;
	m_hCurWnd = hWnd;

	nMinX = nMinY = 0;
	m_offsetX = 0;//p.x-m_ptMouseDownPos.x;
	m_offsetY = 0;//p.y-m_ptMouseDownPos.y;

	map<HWND, WndMoveData>::iterator it = m_mapWndMoveData.begin();
	while(it != m_mapWndMoveData.end())
	{
		//::GetWindowRect(hWnd, &m_mapWndMoveData[hWnd].rc);
		::GetWindowRect(it->second.pWnd->m_hWnd, &it->second.rc);
		it++;
	}

	//	TRACE("OFFSET %d, %d, %d, %d\n", m_offsetX, m_offsetY, m_ptMouseDownPos.x, m_ptMouseDownPos.y);

	CalMovingWndNewPos();
	CalMinDistance(nMinX, nMinY);
	//ModifyMovingWndPos(nMinX, nMinY);
	ModifyMovingWndPosAndSize(nMinX, nMinY, iDirection);
	MovingAllWindows();
	//TRACE("%d\n", GetTickCount() - count);
}

void CWindowsMover::MovingAllWindows()
{
	map<HWND, WndMoveData>::iterator it = m_mapWndMoveData.begin();
	while(it != m_mapWndMoveData.end())
	{
		CRect rc;
		if(it->second.nState == STATE_MOVING)
		{
			rc = it->second.rc;
			if(IsRectValid(rc))
			{
				it->second.pWnd->MoveWindow(&rc, true);
			}
			TRACE("MOVING %d, %d, %d, %d\n", rc.left, rc.right, rc.top, rc.bottom);
		}
		it++;
	}
}

//坐标值是否错的离谱时
BOOL CWindowsMover::IsRectValid(CRect rc)
{
	if(rc.left < -2000 || rc.left > 3000)
		return false; 
	if(rc.right < -2000 || rc.right > 3000)
		return false;
	if(rc.top < -2000 || rc.top > 3000)
		return false;
	if(rc.bottom < -2000 || rc.bottom > 3000)
		return false;
	return true;
}

void CWindowsMover::ModifyMovingWndPosAndSize(int deltaX, int deltaY, int iDirection)
{
	if(deltaX == 0 && deltaY == 0)
		return;

	map<HWND, WndMoveData>::iterator it = m_mapWndMoveData.begin();
	while(it != m_mapWndMoveData.end())
	{
		CRect rc;
		if(it->second.nState == STATE_MOVING)
		{
			rc = it->second.rc;
			if(iDirection & DIRECTION_LEFT)
				it->second.rc.left -= deltaX;
			if(iDirection & DIRECTION_RIGHT)
				it->second.rc.right -= deltaX;
			if(iDirection & DIRECTION_TOP)
				it->second.rc.top -= deltaY;
			if(iDirection & DIRECTION_BOTTOM)
				it->second.rc.bottom -= deltaY;
			//it->second.rc.OffsetRect(-deltaX, -deltaY);
			rc = it->second.rc;
		}
		it++;
	}
}

void CWindowsMover::ModifyMovingWndPos(int deltaX, int deltaY)
{
	if(deltaX == 0 && deltaY == 0)
		return;

	map<HWND, WndMoveData>::iterator it = m_mapWndMoveData.begin();
	while(it != m_mapWndMoveData.end())
	{
		CRect rc;
		if(it->second.nState == STATE_MOVING)
		{
			rc = it->second.rc;
			it->second.rc.OffsetRect(-deltaX, -deltaY);
			rc = it->second.rc;
		}
		it++;
	}
}

void CWindowsMover::CalMinDistanceBetweenTwoWindows(WndMoveData& moving, WndMoveData& stand, int &deltax, int &deltay)
{
	CRect rcM = moving.rc;
	CRect rcS = stand.rc;

	int  diff;
	//if(	
	//	( rcS.left - STICK_DISTANCE < rcM.left && rcM.left < rcS.right + STICK_DISTANCE )
	//  ||( rcS.left - STICK_DISTANCE < rcM.right && rcM.right < rcS.right + STICK_DISTANCE)
	//  )
	if( !(rcS.right + STICK_DISTANCE < rcM.left || rcS.left - STICK_DISTANCE > rcM.right))
	{
		//计算公式是：|y2-y1|<D
		//top and top
		diff = rcM.top - rcS.top;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltay = diff;
		}
		//top and bottom
		diff=rcM.top-rcS.bottom;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltay = diff;
		}

		//bottom and top
		diff=rcM.bottom-rcS.top;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltay = diff;
		}

		//bottom and bottom
		diff=rcM.bottom-rcS.bottom;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltay = diff;
		}
	}

	if(!(rcS.bottom + STICK_DISTANCE < rcM.top || rcS.top - STICK_DISTANCE > rcM.bottom))
	{
		//计算公式是：|y2-y1|<D
		//left and left
		diff=rcM.left-rcS.left;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltax=diff;
		}

		//left and right
		diff=rcM.left-rcS.right;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltax=diff;
		}

		//right and left
		diff=rcM.right-rcS.left;
		if(abs(diff)<STICK_DISTANCE && diff != 0) 
		{
			deltax=diff;
		}

		//right and right
		diff=rcM.right-rcS.right;
		if(abs(diff)<STICK_DISTANCE && diff != 0)
		{
			deltax=diff;
		}
	}
}

void CWindowsMover::CalMinDistance(int &deltaX, int &deltaY)
{
	map<HWND, WndMoveData>::iterator itMoving = m_mapWndMoveData.begin();
	deltaX = deltaY = 0;
	while(itMoving != m_mapWndMoveData.end())
	{
		CRect rc;
		if(itMoving->second.nState == STATE_MOVING)
		{
			map<HWND, WndMoveData>::iterator itStand = m_mapWndMoveData.begin();
			while(itStand != m_mapWndMoveData.end())
			{
				if(itStand->second.nState == STATE_STAND)
				{
					CalMinDistanceBetweenTwoWindows(itMoving->second, itStand->second, deltaX, deltaY);
				}
				itStand ++;
			}
			//it->second.rc.MoveToXY(m_offsetX, m_offsetY);
		}
		itMoving++;
	}
}

void CWindowsMover::DrawMoveFrame(CRect rc)
{
	CWnd* pParentWnd = AfxGetMainWindow()->GetParent();
	CWindowDC dc(pParentWnd);


	//CRect rc;
	//this->GetWindowRect(&rc);
	//rc -= rcWin.TopLeft();					 

	CBitmap bmp;

	bmp.LoadBitmap(IDB_FRAME_LINE);

	CBrush brush(&bmp);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		1, 
		rc.Height(), 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		rc.Width(), 
		1, 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.bottom - 4, 
		rc.Width(), 
		1, 
		PATINVERT);

	dc.PatBlt(
		rc.right, 
		rc.top, 
		1, 
		rc.Height(), 
		PATINVERT);

	dc.SelectObject(pOldBrush);
}