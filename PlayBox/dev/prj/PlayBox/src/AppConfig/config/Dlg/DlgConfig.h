#pragma once

#include <map>
#include "resource.h"
#include "DlgDown.h"
#include "DlgSys.h"
#include "BaseDlg.h"
#include "../../../DataInterface/IAppExitObserver.h"
#include "../../../gui/CommonControl/HyperLinkCtrl.h"

using std::map;

extern HWND g_hLastConfWnd;

#define	MAIN_PAGE	"http://www.kuwo.cn"
#define	PROBLEM_REPORT	"http://www.kuwo.cn/forum/forumdisplay.php?fid=12"

#define CONF_IDX_DOWN	0
#define CONF_IDX_SYS	1

class CConfigDialog : public CDialog,
					  public IAppExitObserver
{
	DECLARE_DYNAMIC(CConfigDialog)
public:
	CDownDialog				m_downDlg;
	CSysDialog				m_sysDlg;
	map<int,BaseDialog*>	m_mapBase;

	BOOL				m_bFullScreen;
	int					m_iShowIdx;		//当前显示的对话框序号
public:
	CConfigDialog( int iShowIdx = 0 , CWnd *pParent = NULL);
	virtual ~CConfigDialog();
	enum { IDD = IDD_CONFIG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnClickedOK();						//确定
	afx_msg void OnClickedCancel();					//取消
	afx_msg void OnConfigBtnDefaultBnClicked();		//缺省设置
	bool IsAllInputOK();
	void IAppExit();	
};
