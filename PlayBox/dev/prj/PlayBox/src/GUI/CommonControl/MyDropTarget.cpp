// MyDropTarget.cpp: implementation of the CMyDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyDropTarget.h"
#include "CDropTarget.h"

#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"

#include <vector>
#include <string>
using std::vector;
using std::string;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//********************************************************************
// If you don't have a recent Platform SDK installed, you'll get linker
// errors on CLSID_DragDropHelper and IID_IDropTargetHelper, and you
// won't have the definition of IDropTargetHelper.  Uncomment the
// following stuff to fix that.

//struct IDropTargetHelper : public IUnknown
//{
//    // IUnknown methods
//    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
//    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
//    STDMETHOD_(ULONG, Release) ( THIS ) PURE;
//
//    // IDropTargetHelper
//    STDMETHOD (DragEnter)(THIS_ HWND hwndTarget, IDataObject* pDataObject,
//                          POINT* ppt, DWORD dwEffect) PURE;
//    STDMETHOD (DragLeave)(THIS) PURE;
//    STDMETHOD (DragOver)(THIS_ POINT* ppt, DWORD dwEffect) PURE;
//    STDMETHOD (Drop)(THIS_ IDataObject* pDataObject, POINT* ppt,
//                     DWORD dwEffect) PURE;
//    STDMETHOD (Show)(THIS_ BOOL fShow) PURE;
//};

// {4657278A-411B-11d2-839A-00C04FD918D0}
extern "C" const GUID __declspec(selectany) CLSID_DragDropHelper = 
    { 0x4657278a, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};

// {4657278B-411B-11d2-839A-00C04FD918D0}
extern "C" const GUID __declspec(selectany) IID_IDropTargetHelper = 
    { 0x4657278b, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};

//********************************************************************


//////////////////////////////////////////////////////////////////////
// Construction/destruction

CMyDropTarget::CMyDropTarget  ( CDropTarget* pDropTarget) : m_pDropTarget(pDropTarget), 
                                                     m_piDropHelper(NULL),
                                                     m_bUseDnDHelper(false)
{
    // Create an instance of the shell DnD helper object.

    if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDropTargetHelper, 
                                       (void**) &m_piDropHelper ) ))
        {
        m_bUseDnDHelper = true;
        }
}

CMyDropTarget::~CMyDropTarget()
{
    if ( NULL != m_piDropHelper )
        m_piDropHelper->Release();
}


//////////////////////////////////////////////////////////////////////
// IDropTarget methods

DROPEFFECT CMyDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
		                                DWORD dwKeyState, CPoint point )
{
DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.

          // Look for CF_HDROP data in the data object, and accept the drop if
        // it's there.

        if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
            dwEffect = DROPEFFECT_COPY;

    // Call the DnD helper.

    if ( m_bUseDnDHelper )
        {
        // The DnD helper needs an IDataObject interface, so get one from
        // the COleDataObject.  Note that the FALSE param means that
        // GetIDataObject will not AddRef() the returned interface, so 
        // we do not Release() it.

        IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 

        m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, 
                                    &point, dwEffect );
        }

    return dwEffect;
}

DROPEFFECT CMyDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
		                               DWORD dwKeyState, CPoint point )
{
DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.

  
        // Look for CF_HDROP data in the data object, and accept the drop if
        // it's there.

        if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
            dwEffect = DROPEFFECT_COPY;

    // Call the DnD helper.

    if ( m_bUseDnDHelper )
        {
        m_piDropHelper->DragOver ( &point, dwEffect );
        }

	m_pDropTarget->DropOver( point );
    return dwEffect;
}

BOOL CMyDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject,
		                     DROPEFFECT dropEffect, CPoint point )
{
BOOL bRet;

    // Read the CF_HDROP data and put the files in the main window's list.

    bRet = ReadHdropData ( pDataObject,point );

    // Call the DnD helper.

    if ( m_bUseDnDHelper )
        {
        // The DnD helper needs an IDataObject interface, so get one from
        // the COleDataObject.  Note that the FALSE param means that
        // GetIDataObject will not AddRef() the returned interface, so 
        // we do not Release() it.

        IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 

        m_piDropHelper->Drop ( piDataObj, &point, dropEffect );
        }
    
    return bRet;
}

void CMyDropTarget::OnDragLeave ( CWnd* pWnd )
{
    if ( m_bUseDnDHelper )
        {
        m_piDropHelper->DragLeave();
        }
}

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.
BOOL CMyDropTarget::ReadHdropData ( COleDataObject* pDataObject,const CPoint& pt )
{
HGLOBAL     hg;
HDROP       hdrop;
UINT        uNumFiles;
TCHAR       szNextFile [512];

    // Get the HDROP data from the data object.

    hg = pDataObject->GetGlobalData ( CF_HDROP );
    
    if ( NULL == hg )
        {
        return FALSE;
        }

    hdrop = (HDROP) GlobalLock ( hg );

    if ( NULL == hdrop )
        {
        GlobalUnlock ( hg );
        return FALSE;
        }

    // Get the # of files being dropped.

    uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );

	vector<string> vecFile;
	string strFilePath;
	string strSuffix;
    for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
        {
        // Get the next filename from the HDROP info.

        if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
            {
				if( YL_FileInfo::IsDirectory( szNextFile) )
				{
					vector<string> vecTmpFile;
					YL_DirInfo::FindFileAndDir( string(szNextFile),NULL,&vecTmpFile);
					for( size_t idx = 0;idx < vecTmpFile.size();idx++ )
					{
						YL_FileInfo::GetFileNameSuffix( vecTmpFile[idx],strSuffix);
						YL_StringUtil::Str2Lower( strSuffix);
	

						// [YUYU]
						//if( strSuffix == "mp3" || strSuffix == "wma" )
						//{
						//	vecFile.push_back( vecTmpFile[idx]);
						//}
					}
				}
				else
				{		
					strFilePath = szNextFile;
					YL_FileInfo::GetFileNameSuffix( strFilePath,strSuffix);
					YL_StringUtil::Str2Lower( strSuffix);


					// [YUYU]
					//if( strSuffix == "mp3" || strSuffix == "wma" )
					//{
					//	vecFile.push_back( strFilePath);
					//}
					// ↓
					//==============================lrf 支持播放列表文件和cue文件 begin
					if (/*strSuffix == "m3u" || strSuffix == "m3u8" ||
						strSuffix == "pls" || strSuffix == "wpl" || strSuffix == "ttpl" || */strSuffix == "cue")
					{
						vecFile.push_back(strFilePath);
						//break;
					}
					//==============================end
				}				
            }
        }   // end for

    GlobalUnlock ( hg );

  
	m_pDropTarget->DropFile( vecFile,pt );

    return TRUE;
}
