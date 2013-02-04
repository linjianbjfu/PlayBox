// MyBHO.h : CMyBHO ������

#pragma once
#include "resource.h"       // ������
#include <string>
#include "BHO_Demo_i.h"

using namespace std;


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

#define __LEA_LOG
// CMyBHO

class ATL_NO_VTABLE CMyBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMyBHO, &CLSID_MyBHO>,
	public IObjectWithSiteImpl<CMyBHO>,
	public IDispatchImpl<IMyBHO, &IID_IMyBHO, &LIBID_BHO_DemoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
	/*,public IDispEventImpl<1,CMyBHO,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,1,1>  */
{
public:
	CMyBHO();

DECLARE_REGISTRY_RESOURCEID(IDR_MYBHO)

DECLARE_NOT_AGGREGATABLE(CMyBHO)

BEGIN_COM_MAP(CMyBHO)
	COM_INTERFACE_ENTRY(IMyBHO)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalReMyBHOse()
	{
	}

public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(Invoke)(DISPID dispidMember,
										REFIID riid, 
										LCID lcid, 
										WORD wFlags, 
										DISPPARAMS * pDispParams, 
										VARIANT * pvarResult,
										EXCEPINFO * pexcepinfo, 
										UINT * puArgErr);
protected:
	//Declare a smart manage pointer to store the Browser.
	CComPtr<IWebBrowser2> m_spWebBrowser;
	//CComQIPtr<IWebBrowser2,&IID_IWebBrowser2> m_spWebBrowser;

	DWORD m_dwCookie;
	CComPtr<IConnectionPoint> m_spCP;

	//Declare a stirng to store the last original url.
	string m_szLastOrgURL;

#ifdef __LEA_LOG
	FILE *m_fpLog;
#endif


};

OBJECT_ENTRY_AUTO(__uuidof(MyBHO), CMyBHO)
