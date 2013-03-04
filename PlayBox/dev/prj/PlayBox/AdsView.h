#pragma once



// CAdsView html view
class CPlayBoxDlg;

class CAdsView : public CHtmlView
{
	DECLARE_DYNCREATE(CAdsView)

protected:
	CAdsView();           // protected constructor used by dynamic creation
	virtual ~CAdsView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CPlayBoxDlg * m_pParentDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


