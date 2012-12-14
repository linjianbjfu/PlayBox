// ������������� Microsoft Visual C++ ������ IDispatch ��װ��

// ע��: ��Ҫ�޸Ĵ��ļ������ݡ����������
//  Microsoft Visual C++ �������ɣ������޸Ľ�����д��


#include "stdafx.h"
#include "ie_explorer.h"
#include <winuser.h>
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CWnd)

BEGIN_EVENTSINK_MAP(CWebBrowser, CWnd)
	ON_EVENT(CWebBrowser, 10003, 259 /* DocumentComplete */, OnDocumentComplete, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser2 ����

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser2 ����

void CWebBrowser::NavigateImage(CString strImagePath, long ImageWidth, long ImageHeight)
{
	// ע�͵��ĺ������ܣ������粻ͨʱ�� Ϊ����ʾIE��Ҳ�棬 �ڱ��ؼ���ͼƬ��ʾ
	//
	//// ��������ʴ�С
	//put_Width(ImageWidth);
	//put_Height(ImageHeight);
	//// ���ÿ�ҳ��
	//Navigate("about:blank", NULL, NULL, NULL, NULL);
	//
	//Sleep(5*1000);
	//CComPtr<IDispatch> spDispatch;
	//CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pDoc2;

	//// �õ��ĵ��ӿ�
	//if((spDispatch = get_Document()) != NULL)
	//{
	//	pDoc2 = spDispatch;
	//	if(pDoc2.p != NULL)
	//	{
	//		Writeln(pDoc2, "<HTML>");
	//		Writeln(pDoc2, "<BODY scroll = \"no\"\
	//					   bottomMargin=\"0\" leftMargin=\"0\" topMargin=\"0\" rightMargin=\"0\">");/*oncontextmenu=self.event.returnValue=false*/
	//		CString img;
	//		img = "<div ><img src = \"" + strImagePath + "\">";
	//		Writeln(pDoc2, img);
	//		Writeln(pDoc2, "</BODY>");
	//		Writeln(pDoc2, "</HTML>");
	//	}
	//}
	//SetFlatStyle();
}

HRESULT CWebBrowser::Writeln(IHTMLDocument2* pDoc, CString html)
{
	HRESULT hr = S_OK;
	VARIANT *param;
	SAFEARRAY *sfArray;

	if(pDoc == NULL)
		return E_FAIL;
	
	// Creates a new one-dimensional array
	sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	if (sfArray == NULL ) 
		return E_FAIL;
	
	BSTR bstr = html.AllocSysString();
	hr = SafeArrayAccessData(sfArray,(LPVOID*) & param);
	param->vt = VT_BSTR;
	param->bstrVal = bstr;
	hr = SafeArrayUnaccessData(sfArray);
	hr = pDoc->writeln(sfArray);

	SysFreeString(bstr);
	if (sfArray != NULL) 
	{
		SafeArrayDestroy(sfArray);
	}
	return hr;
}
void CWebBrowser::SetFlatStyle()
{
	CComPtr<IDispatch> spDispatch;
	CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pDoc2;

	// �õ��ĵ��ӿ�
	if((spDispatch = get_Document()) != NULL)
	{
		pDoc2 = spDispatch;
		if(pDoc2.p != NULL)
		{
			CComPtr<IHTMLElement> pElement;
			pDoc2->get_body(&pElement);
			if(pElement.p != NULL)
			{
				CComPtr<IHTMLStyle> pStyle;
				pElement->get_style(&pStyle);
				if(pStyle.p)
				{
					BSTR lbstrValve = A2BSTR( "none" );
					pStyle->put_borderStyle(lbstrValve);
				}
			}
		}
	}
}

void CWebBrowser::OnDocumentComplete(LPDISPATCH lpDisp,	VARIANT FAR* URL)
{
	CWnd *pWnd = GetParent();
	if(pWnd != NULL && URL != NULL)
	{
		pWnd->ShowWindow(SW_SHOWNOACTIVATE);
	}
}
