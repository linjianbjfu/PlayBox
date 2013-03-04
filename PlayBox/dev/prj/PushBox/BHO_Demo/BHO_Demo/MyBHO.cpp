// MyBHO.cpp : CMyBHO µÄÊµÏÖ

#include "stdafx.h"
#include "MyBHO.h"
#include <ExDisp.h>
#include <ExDispid.h>
//#include <ShlGuid.h>
#define LOG_FILE "\\BHO_Demo.log"



// CMyBHO
CMyBHO::CMyBHO()
:m_dwCookie(0),
m_spWebBrowser(NULL),
m_spCP(NULL)
{
#ifdef __LEA_LOG
	m_fpLog = NULL;
#endif
}

STDMETHODIMP CMyBHO::SetSite(IUnknown *pUnkSite)
{
	if(pUnkSite != NULL)
	{
		pUnkSite->QueryInterface(IID_IWebBrowser2,(void **)&m_spWebBrowser);
		CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer> spCPC(m_spWebBrowser);
		HRESULT hr = spCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_spCP);
		m_spCP->Advise(reinterpret_cast<IDispatch*>(this),&m_dwCookie);
#ifdef __LEA_LOG
		char tempPath[MAX_PATH];
		DWORD dwLen = GetTempPathA(MAX_PATH,tempPath);
		strcat_s(tempPath,LOG_FILE);
		if((m_fpLog = fopen(tempPath,"a+")) == NULL)
		{
			TCHAR Msg[1024];
			wsprintf(Msg,L"Log file open error. ");
			MessageBox(NULL, Msg,L"Warning!",MB_OK|MB_ICONINFORMATION);
		}
#endif

	}
	else
	{
		m_spWebBrowser.Release();
#ifdef __LEA_LOG
		if(m_fpLog !=NULL)
		{
			fclose(m_fpLog);
		}	
#endif
	}

	//MessageBox(NULL,L"Success fully loaded",L"Lea BHO",MB_OK|MB_ICONINFORMATION);
	return IObjectWithSiteImpl::SetSite(pUnkSite);
}

STDMETHODIMP CMyBHO::Invoke(DISPID dispidMember,REFIID riid, LCID lcid, 
							WORD wFlags, DISPPARAMS * pDispParams, 
							VARIANT * pvarResult,EXCEPINFO * pexcepinfo, 
							UINT * puArgErr)
{	
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(wFlags);
	UNREFERENCED_PARAMETER(pvarResult);
	UNREFERENCED_PARAMETER(pexcepinfo);
	UNREFERENCED_PARAMETER(puArgErr);

	VARIANT v[5]; // Used to hold converted event parameters before passing them onto the event handling method
	int n;
	PVOID pv;
	LONG lbound,ubound,sz;

	for(n=0;n<5;n++) VariantInit(&v[n]);


	switch (dispidMember)
	{
	case DISPID_BEFORENAVIGATE2:
#ifdef __LEA_LOG
#ifdef DEBUG
		{
			VariantChangeType(&v[0],&pDispParams->rgvarg[5],0,VT_BSTR); // url
			VariantChangeType(&v[1],&pDispParams->rgvarg[4],0,VT_I4); // Flags
			VariantChangeType(&v[2],&pDispParams->rgvarg[3],0,VT_BSTR); // TargetFrameName
			VariantChangeType(&v[3],&pDispParams->rgvarg[2],0,VT_UI1|VT_ARRAY); // PostData
			VariantChangeType(&v[4],&pDispParams->rgvarg[1],0,VT_BSTR); // Headers
			if(v[3].vt!=VT_EMPTY) {
				SafeArrayGetLBound(v[3].parray,0,&lbound);
				SafeArrayGetUBound(v[3].parray,0,&ubound);
				sz=ubound-lbound+1;
				SafeArrayAccessData(v[3].parray,&pv);
			} else {
				sz=0;
				pv=NULL;
			}

			/*TCHAR Msg[1024];
			wsprintf(Msg,L"Message from Invoke.  \nUrl = %ls",v[0].bstrVal);
			MessageBox(NULL,Msg,_T("DISPID_BEFORENAVIGATE2"),MB_OK|MB_ICONINFORMATION);*/
			USES_CONVERSION;
			if(m_fpLog != NULL && (stricmp(W2CA(v[0].bstrVal),"about:blank") != 0)&&(stricmp(W2CA(v[0].bstrVal),"about:Tabs") != 0))
			{
				fprintf(m_fpLog,"DISPID_BEFORENAVIGATE2 -- %ls\n",v[0].bstrVal);
				fflush(m_fpLog);
			}		
		}
#endif
#endif
		break;
	case DISPID_DOCUMENTCOMPLETE:
		{
			BSTR msg;
			m_spWebBrowser->get_LocationURL(&msg);
			TCHAR *pMsg = OLE2T(msg);
			//MessageBox(NULL,pMsg,L"DISPID_DOCUMENTCOMPLETE",MB_OK|MB_ICONINFORMATION);
#ifdef __LEA_LOG
			USES_CONVERSION;
			if(stricmp(m_szLastOrgURL.c_str(),W2CA(pMsg)) != 0)
			{
				m_szLastOrgURL = W2CA(pMsg);
				if(m_fpLog != NULL && (stricmp(W2CA(pMsg),"about:blank") != 0) && (stricmp(W2CA(pMsg),"about:Tabs") != 0))
				{
					fprintf(m_fpLog,"%ls\n",pMsg);
					fflush(m_fpLog);
				}	
			}
#endif
			SysFreeString(msg);

		}	
		break;
	case DISPID_NAVIGATECOMPLETE2:
		{
			BSTR msg;
			m_spWebBrowser->get_LocationURL(&msg);
			TCHAR *pMsg = OLE2T(msg);
			//MessageBox(NULL,pMsg,L"DISPID_NAVIGATECOMPLETE2",MB_OK|MB_ICONINFORMATION);
#ifdef __LEA_LOG
			USES_CONVERSION;
			if(stricmp(m_szLastOrgURL.c_str(),W2CA(pMsg)) != 0)
			{
				m_szLastOrgURL = W2CA(pMsg);
				if(m_fpLog != NULL && (stricmp(W2CA(pMsg),"about:blank") != 0) && (stricmp(W2CA(pMsg),"about:Tabs") != 0))
				{
					fprintf(m_fpLog,"%ls\n",pMsg);
					fflush(m_fpLog);
				}	
			}
#endif
			SysFreeString(msg);
		}
		break;
	case DISPID_DOWNLOADCOMPLETE:
		{
			BSTR msg;
			m_spWebBrowser->get_LocationURL(&msg);
			TCHAR *pMsg = OLE2T(msg);
			//MessageBox(NULL,pMsg,L"DISPID_DOWNLOADCOMPLETE",MB_OK|MB_ICONINFORMATION);
#ifdef __LEA_LOG

			USES_CONVERSION;
			if(stricmp(m_szLastOrgURL.c_str(),W2CA(pMsg)) != 0)
			{
				m_szLastOrgURL = W2CA(pMsg);
				if(m_fpLog != NULL && (stricmp(W2CA(pMsg),"about:blank") != 0) && (stricmp(W2CA(pMsg),"about:Tabs") != 0))
				{
					fprintf(m_fpLog,"%ls\n",pMsg);
					fflush(m_fpLog);
				}		
			}
#endif
			SysFreeString(msg);
		}
		break;
	case DISPID_NEWWINDOW3:
		{
			BSTR msg;
			m_spWebBrowser->get_LocationURL(&msg);
			TCHAR *pMsg = OLE2T(msg);
			//MessageBox(NULL,pMsg,L"DISPID_DOWNLOADCOMPLETE",MB_OK|MB_ICONINFORMATION);
#ifdef __LEA_LOG
			USES_CONVERSION;
			if(stricmp(m_szLastOrgURL.c_str(),W2CA(pMsg)) != 0)
			{
				m_szLastOrgURL = W2CA(pMsg);
				if(m_fpLog != NULL && (stricmp(W2CA(pMsg),"about:blank") != 0) && (stricmp(W2CA(pMsg),"about:Tabs") != 0))
				{
					fprintf(m_fpLog,"%ls\n",pMsg);
					fflush(m_fpLog);
				}		
			}
#endif
			SysFreeString(msg);
		}
		break;
	default:
		break;
	}
	return E_FAIL;
}