#ifndef _DOWN_SUBJECT_H
#define _DOWN_SUBJECT_H

#define MSG_DOWNSUBSTART WM_USER + 100
#define MSG_DOWNSUBCLOSE WM_USER + 101

class CDownSub
{
public:
	static	void	ReqDownSub(const CString& strSub,HWND hMsgWnd );
	static	void	ReqDownWebSub(const CString& strSubUrl,const CString& strSubId,HWND hMsgWnd );
	static	void	StopDownWebSubThread();
	static  bool	IsDownWebSub();
};


#endif