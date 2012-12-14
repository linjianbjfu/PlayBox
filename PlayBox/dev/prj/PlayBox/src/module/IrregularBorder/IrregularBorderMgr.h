#pragma  once 

class CTranslucentDialog;
class CIrregularBorderMgr
{
private :
	CWnd*							m_pMainWnd;
	CTranslucentDialog*				m_pTopLeftWnd;
	CTranslucentDialog*				m_pBottomLeftWnd;
	CTranslucentDialog*				m_pBottomRightWnd;
	static	CIrregularBorderMgr*	m_pMgr;

	OneSubject*						m_curSubject;
	void							LoadChangeSubjectData();
	BOOL							m_bUpdateView;
public:
	CTranslucentDialog*			m_pTopRightWnd;
	static CIrregularBorderMgr* GetInstance();
	static void 				DelInstance(void);
	void InitTransDlg(BOOL bMustInit = false);
	CIrregularBorderMgr();
	~CIrregularBorderMgr();
	void DeletePngImage();
	void SetMainWnd(CWnd * pWnd);
	void OnMove(int x , int y);
	void OnSize();
	void DrawToppanelPng(CDC *pDC, CRect rc);
	void LoadSubject();
	void ShowTransDlg(bool bShow);
};