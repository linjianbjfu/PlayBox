

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sat Jan 05 10:34:12 2013
 */
/* Compiler settings for .\BHO_Demo.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "BHO_Demo_i.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _BHO_Demo_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } BHO_Demo_MIDL_TYPE_FORMAT_STRING;

typedef struct _BHO_Demo_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } BHO_Demo_MIDL_PROC_FORMAT_STRING;

typedef struct _BHO_Demo_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } BHO_Demo_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const BHO_Demo_MIDL_TYPE_FORMAT_STRING BHO_Demo__MIDL_TypeFormatString;
extern const BHO_Demo_MIDL_PROC_FORMAT_STRING BHO_Demo__MIDL_ProcFormatString;
extern const BHO_Demo_MIDL_EXPR_FORMAT_STRING BHO_Demo__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMyBHO_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IMyBHO_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const BHO_Demo_MIDL_PROC_FORMAT_STRING BHO_Demo__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const BHO_Demo_MIDL_TYPE_FORMAT_STRING BHO_Demo__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMyBHO, ver. 0.0,
   GUID={0xD6E394D6,0x1014,0x485E,{0xAB,0x3C,0xB4,0x5E,0x72,0xD7,0xBE,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IMyBHO_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMyBHO_ProxyInfo =
    {
    &Object_StubDesc,
    BHO_Demo__MIDL_ProcFormatString.Format,
    &IMyBHO_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IMyBHO_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    BHO_Demo__MIDL_ProcFormatString.Format,
    &IMyBHO_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IMyBHOProxyVtbl = 
{
    0,
    &IID_IMyBHO,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IMyBHO_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IMyBHOStubVtbl =
{
    &IID_IMyBHO,
    &IMyBHO_ServerInfo,
    7,
    &IMyBHO_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    BHO_Demo__MIDL_TypeFormatString.Format,
    0, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _BHO_Demo_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IMyBHOProxyVtbl,
    0
};

const CInterfaceStubVtbl * _BHO_Demo_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IMyBHOStubVtbl,
    0
};

PCInterfaceName const _BHO_Demo_InterfaceNamesList[] = 
{
    "IMyBHO",
    0
};

const IID *  _BHO_Demo_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _BHO_Demo_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _BHO_Demo, pIID, n)

int __stdcall _BHO_Demo_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_BHO_Demo_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo BHO_Demo_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _BHO_Demo_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _BHO_Demo_StubVtblList,
    (const PCInterfaceName * ) & _BHO_Demo_InterfaceNamesList,
    (const IID ** ) & _BHO_Demo_BaseIIDList,
    & _BHO_Demo_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

