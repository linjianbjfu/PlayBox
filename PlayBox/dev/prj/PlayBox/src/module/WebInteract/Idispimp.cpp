/*
 * idispimp.CPP
 * IDispatch for Extending Dynamic HTML Object Model
 *
 * Copyright (c)1995-1999 Microsoft Corporation, All Rights Reserved
 */ 

#include "stdafx.h"
#include "idispimp.h"
#include "WebManager.h"

CString cszCB_CallLhBox = "calllhbox";
#define DISPID_CB_calllhbox 1

/*
 * CImpIDispatch::CImpIDispatch
 * CImpIDispatch::~CImpIDispatch
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */ 

CImpIDispatch::CImpIDispatch( void )
{
    m_cRef = 0;
}

CImpIDispatch::~CImpIDispatch( void )
{
	ASSERT( m_cRef == 0 );
}


/*
 * CImpIDispatch::QueryInterface
 * CImpIDispatch::AddRef
 * CImpIDispatch::Release
 *
 * Purpose:
 *  IUnknown members for CImpIDispatch object.
 */ 

STDMETHODIMP CImpIDispatch::QueryInterface( REFIID riid, void **ppv )
{
    *ppv = NULL;


    if ( IID_IDispatch == riid )
	{
        *ppv = this;
	}
	
	if ( NULL != *ppv )
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CImpIDispatch::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CImpIDispatch::Release(void)
{
    return --m_cRef;
}


//IDispatch
STDMETHODIMP CImpIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetTypeInfo(
			/* [in] */ UINT /*iTInfo*/,
            /* [in] */ LCID /*lcid*/,
            /* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetIDsOfNames(
			/* [in] */ REFIID riid,
            /* [size_is][in] */ OLECHAR** rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT hr;
	UINT	i;

	// Assume some degree of success
	hr = NOERROR;

	for ( i=0; i < cNames; i++) {
		CString cszName  = CString(rgszNames[i]);

		if(cszName == cszCB_CallLhBox)
		{
			rgDispId[i] = DISPID_CB_calllhbox;
		}		
		else {
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return hr;
}

STDMETHODIMP CImpIDispatch::Invoke(
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID /*riid*/,
            /* [in] */ LCID /*lcid*/,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS* pDispParams,
            /* [out] */ VARIANT* pVarResult,
            /* [out] */ EXCEPINFO* /*pExcepInfo*/,
            /* [out] */ UINT* puArgErr)
{	
	
	if(dispIdMember == DISPID_CB_calllhbox) 
	{
		string strRes = "";
		if ( wFlags & DISPATCH_METHOD )
		{	
			CString cszArg1= (CString)pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy			
			CWebManager::GetInstance()->CallGBoxFromWeb(string(cszArg1),strRes);
		}

		if(wFlags & DISPATCH_PROPERTYGET)
		{
			if(pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult)=VT_BSTR;
				//				V_BOOL(pVarResult) = false;
				pVarResult->bstrVal = CString(strRes.c_str()).AllocSysString();
			}
			return 1;
		}	
	}
 	return S_OK;
}
