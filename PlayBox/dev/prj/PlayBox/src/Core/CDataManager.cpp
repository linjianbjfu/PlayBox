#include "stdafx.h"
#include "CDataManager.h"
#include "../module/HTTPControl/HTTPControl.h"

CDataManager::CDataManager()
{
	m_mapDataObject.clear();	
	InitializeCriticalSection(&m_csData);
}

CDataManager::~CDataManager()
{
	DeleteCriticalSection(&m_csData);
}

void CDataManager::DataManAppStart()
{	
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"DataManager","DataManAppStart======IN" );
	CHTTPControl::Init();
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"DataManager","DataManAppStart======OUT" );
}

void CDataManager::DataManAppExit()
{
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"DataManager","DataManAppExit======IN" );
	for( map<DWORD,IData*>::iterator it1 = m_mapDataObject.begin();
		it1 != m_mapDataObject.end(); it1++ )
	{
		DWORD d = it1->first;
		it1->second->DataAppExit();
	}
	CHTTPControl::Close();
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"DataManager","DataManAppExit--OUT" );
}

void CDataManager::AddDataObject( const DWORD& guidDataObject,IData* pData )
{
	if( pData == NULL )
	{
		return;
	}

	EnterCriticalSection(&m_csData);
	m_mapDataObject[guidDataObject] = pData;
	LeaveCriticalSection(&m_csData);
}

void CDataManager::RemoveDataObject( const DWORD& guidDataObject )
{
	EnterCriticalSection(&m_csData);
	m_mapDataObject.erase(guidDataObject);
	LeaveCriticalSection(&m_csData);
}

IData* CDataManager::GetDataObject(const DWORD& guidDataObject)
{
	IData* pData = _GetMapObject( guidDataObject );
	if( pData )
	{
		return pData;
	}
	switch( guidDataObject )
	{
	case ID_DATA_TABBAR:
		pData = CTabBarData::GetInstance();
		break;
	case ID_DATA_GAME:
		pData = CGameDataImp::GetInstance();
		break;
	case ID_DATA_PANELCHANGE:
		pData = CPanelChange::GetInstance();
		break;
	default:
		pData = NULL;
		break;
	}
	if( pData )
	{
		pData->DataAppStart();
		AddDataObject(guidDataObject, pData);
	}
	return pData;
}

IData* CDataManager::_GetMapObject(const DWORD& guidDataObject)
{
	if( m_mapDataObject.find(guidDataObject) == m_mapDataObject.end() )
	{
		return NULL;
	}
	return m_mapDataObject[guidDataObject];
}

IData* CDataManager::GetExportDataObject(const DWORD& guidDataObject)
{
	if( m_mapDataObject.find( guidDataObject ) == m_mapDataObject.end() )
	{
		return NULL;
	}
	return m_mapDataObject[guidDataObject];
}