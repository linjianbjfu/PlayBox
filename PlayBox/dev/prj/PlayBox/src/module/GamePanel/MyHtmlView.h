#pragma once
#include "..\src\module\GamePanel\DownPercentWnd.h"
#include "Resource.h"



// CMyHtmlView html view
class DownPercentWnd;

class CMyHtmlView : public CHtmlView
{
	DECLARE_DYNCREATE(CMyHtmlView)

public:
	CMyHtmlView();           // protected constructor used by dynamic creation
	virtual ~CMyHtmlView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	enum {IDD = IDD_HTML_VIEW};
	//Declare the parent dialog's pointer.
	DownPercentWnd* pViewDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


