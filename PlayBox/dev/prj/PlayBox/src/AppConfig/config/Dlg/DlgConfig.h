#pragma once

#include <map>
#include "resource.h"
#include "DlgDown.h"
#include "DlgSys.h"
#include "BaseDlg.h"
#include "../../../DataInterface/IAppExitObserver.h"
#include "../../../gui/CommonControl/HyperLinkCtrl.h"
#include "../../gui/CommonControl/xSkinButton.h"

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
	int					m_iShowIdx;		//��ǰ��ʾ�ĶԻ������
public:
	CConfigDialog( int iShowIdx = 0 , CWnd *pParent = NULL);
	virtual ~CConfigDialog();
	enum { IDD = IDD_CONFIG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnClickedOK();						//ȷ��
	afx_msg void OnClickedCancel();					//ȡ��
	afx_msg void OnConfigBtnDefaultBnClicked();		//ȱʡ����

	afx_msg void OnBnClickedRadMin();
	afx_msg void OnBnClickedRadExit();
	afx_msg void OnBnClickedChkNoask();
	afx_msg void OnBnClickedCheckAutoRun();
	afx_msg void OnBnClickedChkBosskey();
	afx_msg void OnBnClickedChkAutoLogin();


	bool IsAllInputOK();
	void IAppExit();
	void SetDefault();

	BOOL UpdateData(BOOL bSaveAndValidate  = TRUE );	//����UpdateData;
	// control
	CxSkinButton m_btnOK;
	CxSkinButton m_btnCancle;
 	CxSkinButton m_btnReset;
	CxSkinButton m_btnClose;

	BOOL			m_bAutoLogin;		// �Զ���¼
	CxSkinButton	m_checkedAutoLogin;
	CxSkinButton	m_unChkedAutoLogin;

	BOOL			m_bAutoRun;			//�����Զ�����
	CxSkinButton	m_checkedAutoRun;
	CxSkinButton	m_unChkedAutoRun;

	BOOL m_minOrExit;					// TRUE:exit	FALSE:min
	CxSkinButton	m_radMin_Checked;
	CxSkinButton	m_radMin_unChked;
	CxSkinButton	m_radExit_Checked;
	CxSkinButton	m_radExit_unChked;

	BOOL m_bNoAsk;
	CxSkinButton	m_checkedNoAsk;
	CxSkinButton	m_unChkedNoAsk;

	SetBaseStatic m_staticMinOrExit;

	BOOL		m_bUseBossKey;	// �Ƿ������ϰ��
	CxSkinButton	m_checkUseBossKey;
	CxSkinButton	m_unChkUseBossKey;
	CHotKeyCtrl m_BossKeyCtrl;	// �ϰ������ؼ�

	CDibBitmap*  m_bkg;

public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
