#pragma once

#define WM_DROP_MSG		WM_USER + 101

class CAcceptDrag :
	public CObject
{
	DECLARE_DYNAMIC(CAcceptDrag)

public:
	CAcceptDrag(void);
	~CAcceptDrag(void);

public:

	void	DropDown( CWnd* pSrc,CWnd* pDes,void* pp);

};
