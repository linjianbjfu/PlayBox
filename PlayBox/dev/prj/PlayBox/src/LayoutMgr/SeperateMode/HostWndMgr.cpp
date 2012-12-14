#include "stdafx.h"

#include "hostwndmgr.h"
#include "hostwnd.h"
#include "MainHostWnd.h"
#include "WindowsMover.h"

CHostWndMgr* CHostWndMgr::m_pHostMgr = NULL;

CHostWndMgr * CHostWndMgr::GetInstance()
{
	if(m_pHostMgr == NULL)
	{
		m_pHostMgr = new CHostWndMgr();

	}

	return m_pHostMgr;
}

void CHostWndMgr::DelInstance()
{
	delete m_pHostMgr;
}

CHostWndMgr::CHostWndMgr():m_bHideWindow(FALSE)
{
	m_pMainWnd = NULL;
}

CHostWndMgr::~CHostWndMgr()
{
}

void CHostWndMgr::KeepOriParent(CWnd *pWndContent)
{
	//;//pHostWnd->HostWindow(pWndContent);
	CWndData data;
	data.pWnd = pWndContent;
	data.pParentWnd = pWndContent->GetParent();
	//pWnd->SetParent(this);
	m_mapContentWnd[pWndContent->m_hWnd] = data;
}

void CHostWndMgr::AttachMovingWnds(vector<SepWindowData> & vecSepData)
{
	for(int i = 0; i<vecSepData.size(); i++)
		AttachMovingWnd(vecSepData[i]);
}

void CHostWndMgr::AttachMovingWnd(SepWindowData data)
								//  CWnd* pWnd, bool isMain, CRect rc, string strName, bool resizable, CString strContent)
{
	CHostWnd *pHostWnd = NULL;

	if(data.isMain)
	{
		pHostWnd = new CMainHostWnd();
		m_pMainWnd = pHostWnd;//data.pWnd;
	}
	else
		pHostWnd = new CHostWnd();

	pHostWnd->SetMainWnd(data.isMain);
//	pHostWnd->SetBackGround(strBorder);
	pHostWnd->SetResizable(data.isResizable);
	pHostWnd->SetContent(data.strContent);
	pHostWnd->SetMinConstrant(data.nMinX, data.nMinY);

    //pHostWnd->Create(IDD_HOST,NULL);
	if(!data.isMain)
		pHostWnd->Create(IDD_HOST,::AfxGetMainWindow());//pHostWnd->SetParent(m_pMainWnd);//::AfxGetMainWindow());
	else
		pHostWnd->Create(IDD_HOST,::AfxGetMainWindow());

	pHostWnd->SetName(string(data.strName));
	m_vecHostWnds.push_back(pHostWnd);

	m_vecVisibleWnds.push_back(pHostWnd);
	if(data.bHide && !data.isMain)
		pHostWnd->ShowWindow(SW_HIDE);
	else
	{
		if (!m_bHideWindow)
		{
			pHostWnd->ShowWindow(SW_SHOW);
		}
	}

	pHostWnd->HostWindow(data.pWnd, data.isMain);
	pHostWnd->MoveWindow(data.rc);


	if(data.isMain)
	{
		AfxGetMainWindow()->MoveWindow(data.rc.left, data.rc.top, 0,0);
		CString str;
		str.Format("%d,%d", data.rc.left, data.rc.top);
		AfxGetMainWindow()->GetWindowRect(&data.rc);
		//if(AfxGetMainWindow()->IsWindowVisible())
		//AfxMessageBox(str);
	}
}

vector<CHostWnd*>* CHostWndMgr::GetHostWnds()
{
	return &m_vecHostWnds;
	/*for(int i = 0 ; i<m_vecHostWnds.size(); i++)
	{
		vecNames.push_back(m_vecHostWnds[i]->GetName());
	}
	*/
}

void CHostWndMgr::DetachAllMovingWnd()
{
	if(m_vecHostWnds.size() == 0)
		return ;

	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","3");	
	for(int i = 0 ; i<m_vecHostWnds.size(); i++)
	{
		CHostWnd* pHost = (CHostWnd*)m_vecHostWnds[i];
		pHost->DetachWindow();
	}

	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","31");	
	map<HWND, CWndData>::iterator it = m_mapContentWnd.begin();
	while(it != m_mapContentWnd.end())
	{
		CWndData d = it->second;
		it->second.pWnd->SetParent(it->second.pParentWnd);
		//it->second.pWnd->SetParent(::AfxGetMainWindow());
		it++;
	}

	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","32");	
	for(int i = 0 ; i<m_vecHostWnds.size(); i++)
	{
		CHostWnd* pHost = (CHostWnd*)m_vecHostWnds[i];
		pHost->DestroyWindow();
		delete pHost;
	}
	m_vecHostWnds.clear();
	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","33");	
	CWindowsMover::GetInstance()->Clear();

	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","34");	
}

//void CHostWndMgr::DetachMovingWnd(CHostWnd *pHostWnd)
//{
//	pHostWnd->get
//}
void CHostWndMgr::MoveMainWindow(CRect rcNew)
{

}

void CHostWndMgr::test()
{
	CHostWnd *pWnd;
	CWnd * pMain;
	for(int i = 0 ; i<10; i++)
	{
		pWnd = new CHostWnd();
		if(i == 0)
		{
			pWnd->SetMainWnd(true);
			pMain = pWnd;
		}
		int x = i*150;
		if(i == 0)
			pWnd->Create(IDD_HOST, ::AfxGetMainWindow());
		else
			pWnd->Create(IDD_HOST, pMain);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->MoveWindow(CRect(x, x, x + 200 , x+ 100));
	}

	// Another improved PeekMessage() loop
	MSG msg;
	for (;;)   
	{
		for (;;)    
		{        
			if (false)//IfBackgroundProcessingRequired())  
			{          
				if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           
					break;     
			}       
			else      
				GetMessage(&msg, NULL, 0, 0);

            if (msg.message == WM_QUIT)    
				return;
			TranslateMessage(&msg);      
			DispatchMessage(&msg);      
		}     
		//BackgroundProcessing();
	}	

}

CHostWnd* CHostWndMgr::GetHostWnd(string strName)
{
	for(int i = 0; i<m_vecHostWnds.size(); i++)
	{
		CHostWnd * wnd = m_vecHostWnds[i]; 

		if(wnd->GetName() == strName)
			return wnd;
	}
	return NULL;
}

CHostWnd* CHostWndMgr::GetHostWnd(CWnd *pWnd)
{
	for(int i = 0; i<m_vecHostWnds.size(); i++)
	{
		CHostWnd * wnd = m_vecHostWnds[i]; 

		if(wnd->GetContentWnd() == pWnd)
			return wnd;
	}
	return NULL;
}
void CHostWndMgr::ShowHostWindows(BOOL bShow)
{
	if(::AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() != DISPLAY_SEPERATE_MODE)
		return;

	if(!bShow)
	{
		for(int i = 0 ; i<m_vecHostWnds.size(); i++)
		{
			if(m_vecHostWnds[i]->IsWindowVisible())
			{
				break;
			}
		}
		//都没有显示，所以直接返回
		if(i == m_vecHostWnds.size())
		{
			return;
		}
	}

	if(bShow)
	{
		for(int i = 0 ; i<m_vecHostWnds.size(); i++)
		{
			//其实已经有窗口显示了，所以也直接返回
			if(m_vecHostWnds[i]->IsWindowVisible())
			{
				return;
			}
		}
	}

	if(bShow)
	{
		ShowPopupWndsBeforeMainChange(true);
		for(int i = 0 ; i<m_vecVisibleWnds.size(); i++)
			m_vecVisibleWnds[i]->ShowWindow(SW_SHOW);

		if( i == 0)
		{
			CString str;
			CRect rc;
			m_pMainWnd->ShowWindow(SW_SHOW);
		}
		ShowPopupWndsAfterMainChange(true);
	}
	else
	{
		ShowPopupWndsBeforeMainChange(false);
		m_vecVisibleWnds.clear();
		for(int i = 0 ; i<m_vecHostWnds.size(); i++)
		{
			if(m_vecHostWnds[i]->IsWindowVisible())
			{
				m_vecVisibleWnds.push_back(m_vecHostWnds[i]);
				m_vecHostWnds[i]->ShowWindow(SW_HIDE);
			}
		}
		ShowPopupWndsAfterMainChange(false);
	}
}

void CHostWndMgr::ShowPopupWndsBeforeMainChange(BOOL bShow)
{
}
void CHostWndMgr::ShowPopupWndsAfterMainChange(BOOL bShow)
{

}
void CHostWndMgr::PopupWndsChangeTo1x()
{

}
void CHostWndMgr::PopupWndsChangeTo2x()
{
}
