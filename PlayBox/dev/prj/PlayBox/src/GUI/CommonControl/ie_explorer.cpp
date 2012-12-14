// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被改写。


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
// CWebBrowser2 属性

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser2 操作

void CWebBrowser::NavigateImage(CString strImagePath, long ImageWidth, long ImageHeight)
{
	// 注释掉的函数功能，当网络不通时候， 为不显示IE空也面， 在本地加载图片提示
	//
	//// 设置浏览资大小
	//put_Width(ImageWidth);
	//put_Height(ImageHeight);
	//// 设置空页面
	//Navigate("about:blank", NULL, NULL, NULL, NULL);
	//
	//Sleep(5*1000);
	//CComPtr<IDispatch> spDispatch;
	//CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pDoc2;

	//// 得到文档接口
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

	// 得到文档接口
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
