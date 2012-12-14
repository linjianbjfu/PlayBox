#pragma  once

#include <map>
using namespace std;

#define STICK_DISTANCE	15

struct WndMoveData
{
	CWnd *pWnd;
	CRect rc;
	int	  nState;
	bool  isMain;
};

class CWindowsMover
{
private:
	enum {
		STATE_MOVING,
		STATE_STAND
	};

	CWindowsMover();
	~CWindowsMover();

	BOOL					IsRectValid(CRect rc);
	BOOL					m_bMouseDown;
	map<HWND, WndMoveData>	m_mapWndMoveData;
	static CWindowsMover *	m_pMover;
	HWND					m_hCurWnd;
	CPoint					m_ptMouseDownPos;
	CPoint					m_ptLastMousePos;
	int						m_offsetX;
	int						m_offsetY;

	//int						m_nMinDistanceX;
	//int						m_nMinDistanceY;

	void	GetAllWindowsRect();
	void	CalMovingWndNewPos();
	void	CalMinDistance(int &nMinDeltaX, int &nMinDeltaY);
	void	CalMinDistanceBetweenTwoWindows(WndMoveData& moving, WndMoveData& stand, int &deltax,int& deltay);
	void	ModifyMovingWndPos(int deltaX, int deltaY);
	void	MovingAllWindows();
	BOOL	IsTwoWndStucked(CRect rc, CRect rcTwo, CRect rcMain);
	void	ModifyMovingWndPosAndSize(int deltaX, int deltaY, int iDirection);

	CRect	m_rcKeep;
public:
	BOOL	m_bDontMoveContent;
	static CWindowsMover*	GetInstance();
	static void				DelInstance();
	
	void	SetDontMoveContent()
	{
		OSVERSIONINFO  version = {0};
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&version);
		if(version.dwMajorVersion < 6)
		{
			if(AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
				m_bDontMoveContent = false;
			else
				m_bDontMoveContent = true;
		}
		else
		{
			m_bDontMoveContent = false;
		}
	};
	void	DoMouseResizingFinished(HWND hWnd ,UINT nFlags, CPoint p, int iDirection);
	void	DoMouseMove(HWND hWnd, UINT nFlags, CPoint point);
	void	DoMouseDown(HWND hwnd, CPoint p);
	void	DoMouseUp(HWND hwnd, CPoint p);
	void	AddWndindow(CWnd *pWnd, bool isMain);
	void	DrawMoveFrame(CRect rc);
	void	Clear();

};