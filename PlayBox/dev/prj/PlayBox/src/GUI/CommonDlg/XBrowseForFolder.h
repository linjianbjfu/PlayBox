// XBrowseForFolder.h  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.  You are free to use it 
// in any way you like.
//
// This software is provided "as is" with no expressed or implied warranty.  
// I accept no liability for any damage or loss of business that this software 
// may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XBROWSEFORFOLDER_H
#define XBROWSEFORFOLDER_H

class CCheckBox : public CButton
{
	DECLARE_DYNAMIC(CCheckBox)

public:
	CCheckBox();
	virtual ~CCheckBox();
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlag, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};


BOOL XBrowseForFolder(HWND hWnd,
					  LPCTSTR lpszInitialFolder,
					  LPTSTR lpszBuf,
					  DWORD dwBufSize, BOOL& bIncludeSubFolder);

#endif //XBROWSEFORFOLDER_H
