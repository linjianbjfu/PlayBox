#include "StdAfx.h"
#include "PanesMgr.h"
#include "CreateControlFactory.h"
#include <string>
#include "skinmgr.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "../AppConfig/config/ConfigSkinDef.h"
#include "EtsSplitter.h"
#include "math.h"
#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
#include "./SeperateMode/HostWndMgr.h"
#include "./seperatemode/hostwnd.h"
#include "shlwapi.h"

#include "YL_FileInfo.h"
#include "YL_FileZip.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"
#include "YL_RegInfo.h"
#include "SkinVersion.h"

#include "CDataManager.h"

using namespace std;
extern HWND g_hLastConfWnd;
extern CBitMapOperate gBitMapOperate;

CPanesMgr::CPanesMgr(void)
{
	m_bJustLoadColor = FALSE;
	m_bChangedSkin   = FALSE;
	m_bWaitOutFullScreen = FALSE;
	m_strWaitSkin = "";
	m_SubEvent = NULL;
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*)this);
}

bool CPanesMgr::SetXmlPath(CString path)
{		
	if(!m_xml.Open(path))
	{
		return false;
	}
	m_xml.ResetPos();
	m_xml.FindElem();
	m_xml.IntoElem();
	m_modeDisplay = DISPLAY_FULL_MODE;
	return true;
}

void CPanesMgr::SetNodeValue(CString tag, CString nameValue ,CString attrib, CString attribvalue)
{
	CString value = m_xml.GetAttrib("start_x_left");
	if(value != "")
		LocateNode(tag, nameValue,"start_x_left",value);
	else
		LocateNode(tag, nameValue);

	m_xml.SetAttrib(attrib, attribvalue);
}

void CPanesMgr::GetNodeValue(CString tag, CString nameValue ,CString attrib, CString& attribvalue)
{
	CString value = m_xml.GetAttrib("start_x_left");
	if(value != "")
		LocateNode(tag, nameValue,"start_x_left",value);
	else
		LocateNode(tag, nameValue);

	attribvalue = m_xml.GetAttrib(attrib);
}

void CPanesMgr::ClearBkLayoutMap()
{
	POSITION pos = m_mapBkLayoutMgr.GetStartPosition();
	HWND key;
	ETSLayoutMgr * pLayout;
	while (pos != NULL)
	{
		m_mapBkLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );
		delete pLayout;
	}
	m_mapBkLayoutMgr.RemoveAll();
}

void CPanesMgr::ClearWndLayoutMap()
{
	POSITION pos = m_mapLayoutMgr.GetStartPosition();
	HWND key;
	ETSLayoutMgr * pLayout;
	while (pos != NULL)
	{
		m_mapLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );
		delete pLayout;
	}
	m_mapLayoutMgr.RemoveAll();
}

CPanesMgr::~CPanesMgr(void)
{
	m_xml.Close();
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*)this);
	ClearBkLayoutMap();
	ClearWndLayoutMap();
}
//保存整个主界面的界面配置
void CPanesMgr::SavePanes()
{
	if(FindNode("rootpanel", "playbox"))
	{
		RecursionSavePane(m_xml.GetAttrib("ori"));
	}
	SaveWndPanel();
	m_xml.Save();
}
//程序退出时调用， 保存相应参数
void CPanesMgr::AppExit()
{
	SavePanes();
}
//找到一个节点（包括: rootpane, startparm）
BOOL CPanesMgr::FindNode(CString tag, CString name)
{
	m_xml.ResetPos();
	m_xml.FindElem();
	m_xml.IntoElem();
	while(m_xml.FindChildElem())
	{
		//CString tag = m_xml.GetTagName();
		//CString dd = m_xml.GetAttrib("name");
		if(tag == m_xml.GetTagName() && 
			(m_xml.GetAttrib("name") == name||name == ""))
		{
			return true;
		}
	}
	return false;
}
//保存root pane参数
void CPanesMgr::SaveWndPanel(CString szWndName, CString szControlName)
{
	HWND key;
	ETSLayoutMgr * pLayout;
	CString strWndName, strControlName;
	bool bFind = false;

	POSITION pos = m_mapLayoutMgr.GetStartPosition();
	while (pos != NULL)
	{
		m_mapLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );
		if(key == ::AfxGetMainWindow()->m_hWnd)
		{
			continue;
		}

		if(!::IsWindow(key))
		{
			continue;
		}

		strWndName = pLayout->m_strWndName;
		strControlName = pLayout->m_strControlName;

		if(szWndName != "" && strWndName != szWndName && strControlName != szControlName)
		{
			continue;
		}

		if(!FindNode("wndpanel",strWndName))
			return;

		m_xml.IntoElem();

		bFind = false;
		while(m_xml.FindChildElem())
		{
			if(m_xml.GetTagName() == "controlpanel" && m_xml.GetAttrib("name") == strControlName)
			{
				bFind = true;
				CString strX,strY;

				strX = m_xml.GetAttrib("sizeX");
				strY = m_xml.GetAttrib("sizeY");

				RECT rc;
				::GetWindowRect(key, &rc);		
				RecursionSavePane(m_xml.GetAttrib("ori"));
				break;
			}
		}
		m_xml.OutOfElem();
		if(bFind)
		{
			m_xml.SetAttrib("mode",strControlName );
		}
	}
}

void CPanesMgr::SaveSplPane()
{
	ETSLayoutMgr::CPane pane;

	CString strValue;

	ETSSplitter *spl = (ETSSplitter*)GetWnd(m_xml.GetAttrib("name"));

	if(spl->m_pane == NULL)
		return;
	if(spl->m_pane->getSecondSize() < 0)
	{
		//TRACE("secondsize error\n");
	}
	else
	{
		strValue.Format("%d", spl->m_pane->getSecondSize());
		m_xml.SetAttrib("splpane-size", strValue);
		strValue.Format("%d", spl->m_pane->m_nSplOpendPos);
		m_xml.SetAttrib("open-size", strValue);
	}
	return;
}
//递归保存各pane的参数
BOOL CPanesMgr::RecursionSavePane(CString parentOri)
{
	CString tagName;
	m_xml.IntoElem();

	bool bFirstPaneWidth = 0;
	int parentvalue = 0;
	while(m_xml.FindChildElem())
	{
		CString tagName = m_xml.GetTagName();
		CString paneName = m_xml.GetAttrib("name");
		//TRACE("%s, %s\n", tagName, paneName);
		if(tagName == "paneitem")
		{
			CWnd * wndPanel = this->GetWnd(paneName);
			if(wndPanel == NULL)
				continue;
			ASSERT(wndPanel != NULL);
			CRect rc;
			GetWindowRect(wndPanel->m_hWnd, &rc);
			CString value;
			if((value = m_xml.GetAttrib("sizeX")) != "" && m_xml.GetAttrib("private_setting") != "1")
			{
				value.Format("%d", rc.Width());
				m_xml.SetAttrib("sizeX", value);
			}
			if((value = m_xml.GetAttrib("sizeY")) != "" && m_xml.GetAttrib("private_setting") != "1")
			{
				value.Format("%d", rc.Height());
				m_xml.SetAttrib("sizeY", value);
			}

			if(!bFirstPaneWidth)
			{
				if(parentOri  == "HORZ")
					parentvalue= rc.Height();
				else
					parentvalue = rc.Width();
			}
		}
		else if(tagName == "panespl")
		{
			RecursionSavePane(m_xml.GetAttrib("ori"));
			SaveSplPane();
		}
		else if(tagName == "pane")
		{
			ETSLayoutMgr::CPaneBase base;
			if(GetPane(paneName, base))
			{
				CRect rc = base->getPaneRect();
				CString value;
				if(m_xml.GetAttrib("ori") == "VERT")
					value.Format("%d", rc.Width());
				else
					value.Format("%d", rc.Height());

				if(value != "-1")
					m_xml.SetAttrib("second-size", value);
			}
			RecursionSavePane(m_xml.GetAttrib("ori"));			
		}else if(tagName == "panemulti")
		{
			ETSLayoutMgr::CPaneMulti pane;
			GetPaneMulti(paneName, pane);
			if(pane.IsValid())
			{
				CString current = pane->getCurrent();
				m_xml.SetAttrib("current", current);

				SaveMultiLayer(m_xml.GetAttrib("ori"));
			}
		}else
		{
			//AfxMessageBox("无效的Pane类型: " + tagName);
		}
	}
	m_xml.OutOfElem();
	return parentvalue;
}

ETSLayoutMgr::CPaneBase CPanesMgr::CreatePaneItem(CString paneName, ETSLayoutMgr *pLayout)
{
	ETSLayoutMgr::CPaneBase pane;

	ETSLayoutMgr::itemType itemType = getItemType();

	CWnd *wndPanel = NULL;

	if(itemType == ETSLayoutMgr::TYPE_WINDOW || itemType == ETSLayoutMgr::TYPE_CONTROL)
	{
		wndPanel = this->GetWnd(paneName);
		if(wndPanel == NULL)
		{
			wndPanel = this->GetCtrl(NULL,paneName);
			if(wndPanel == NULL)
				return NULL;
		}
		CWnd * pWnd = wndPanel->GetParent();

		//gch edit 换色时，有多个gamepanel和gameplayingpanel，不能改变它们的父子关系
		if( !m_bJustLoadColor )
		{
			if(pLayout->m_pWnd != pWnd)
				wndPanel->SetParent(pLayout->m_pWnd);
		}
	}

	bool bDontChangeColor = atoi(m_xml.GetAttrib("dont_change_color")) == 1;

	if(itemType == ETSLayoutMgr::TYPE_BMP)
	{
		if(m_pSkin->GetDibBmp(paneName, false) == NULL)
			m_pSkin->PutDibBmp(paneName, m_pSkin->m_strSkinPath);
		m_pSkin->SetChangeColorSign(paneName,bDontChangeColor);
	}
	else
		CreateControlFactory::CreateControl(wndPanel, &m_xml, m_pSkin,this);

	if(this->m_bJustLoadColor)
		return NULL;

	int sizeX = getSizeX();
	int	sizeY = getSizeY();
	
	CRect rc;
	if(wndPanel != NULL)
	{
		wndPanel->GetWindowRect(&rc);
		
		if(m_xml.GetAttrib("height_custom") == "1")
			sizeY = rc.Height();
		if(m_xml.GetAttrib("width_custom") == "1")
			sizeX = rc.Width();
	}

	pane = pLayout->item(wndPanel, getModeSize(), sizeX, sizeY, getSizeXMin(), getSizeYMin());

	((ETSLayoutMgr::PaneItem *)pane.GetPaneBase())->setItemType(getItemType());
	pane->SetName(paneName);

	SetPositionParam(pane.GetPaneBase());
	return pane;
}

ETSLayoutMgr::CPane CPanesMgr::CreatePaneSpl(CString paneName, ETSLayoutMgr *pLayout)
{
	ETSLayoutMgr::CPane pane;
	CWnd * spl = this->GetWnd(paneName);

	if(this->m_bJustLoadColor && spl == NULL)
		return NULL;

	spl->SetParent(pLayout->m_pWnd);

	CreateControlFactory::CreateControl(spl, &m_xml, m_pSkin,this);

	if(this->m_bJustLoadColor)
		return NULL;

	//	m_pMapPanels->Lookup(paneName , (CObject *&)spl);
	ASSERT(spl != NULL);
	//	spl->ShowWindow(SW_SHOW);

	pane = pLayout->paneSpl((ETSSplitter*)spl, getOrization() , getModeSize(), getClosedPos(), 
		getClosedOri(), 
		getSplSecondSize(),
		getSplOpenSize(),
		getSplHideCloseBtn(),
		getSplSlimSign());
	pane->SetName(paneName);
	SetPositionParam(pane.GetPaneBase());
	CreateControlFactory::CreateControl(spl, &m_xml, m_pSkin,this);
	return pane;
}

ETSLayoutMgr::CPane CPanesMgr::CreatePane(CString paneName, ETSLayoutMgr *pLayout)
{
	ETSLayoutMgr::CPane pane;
	pane = pLayout->pane(getOrization(), getModeSize(),0, 0,getSecondSize() );
	pane->SetName(paneName);
	SetPositionParam(pane.GetPaneBase());
	AddPane(paneName, pane);
	return pane;
}

void CPanesMgr::AddPane(CString name, ETSLayoutMgr::CPaneBase pane)
{
	m_mapPanes.SetAt(name, pane);
}

bool CPanesMgr::GetPane(CString name, ETSLayoutMgr::CPaneBase &pane)
{
	if(!m_mapPanes.Lookup(name, pane))
	{
		return false;
	}
	return true;
}

ETSLayoutMgr::CPaneMulti CPanesMgr::CreatePaneMulti(CString paneName, ETSLayoutMgr *pLayout, CString current)
{
	ETSLayoutMgr::CPaneMulti pane;
	CString strCurrent;

	strCurrent = m_xml.GetAttrib("current");
	if(m_bJustLoadColor)
	{
		CreateMultiLayer(NULL, pLayout, NULL, strCurrent);
		return NULL;
	}

	pane = pLayout->paneMulti(getOrization() , getModeSize() , 0);
	pane->SetName(paneName);
	CreateMultiLayer(&pane, pLayout, NULL, strCurrent);
	pane->setCurrent(strCurrent, false);
	SetPositionParam(pane.GetPaneBase());
	return pane;
}

void CPanesMgr::SetPositionParam(ETSLayoutMgr::PaneBase *base)
{
	CString str_start_x_left	= m_xml.GetAttrib("start_x_left");
	CString str_start_x_right	= m_xml.GetAttrib("start_x_right");
	CString str_start_x_percent = m_xml.GetAttrib("start_x_percent");
	CString str_start_y_top		= m_xml.GetAttrib("start_y_top"); //== "");
	CString str_start_y_bottom	= m_xml.GetAttrib("start_y_bottom"); //== "") ? -1:atoim_xml.GetAttrib("start_y_bottom"));
	CString str_start_y_percent = m_xml.GetAttrib("start_y_percent"); //== "") ? -1:atoim_xml.GetAttrib("start_y_percent"));
	CString str_end_x_left		= m_xml.GetAttrib("end_x_left"); //== "") ? -1:atoim_xml.GetAttrib("end_x_left"));
	CString str_end_x_right		= m_xml.GetAttrib("end_x_right"); //== "") ? -1:atoim_xml.GetAttrib("end_x_right"));
	CString str_end_x_percent	= m_xml.GetAttrib("end_x_percent"); //== "") ? -1:atoim_xml.GetAttrib("end_x_percent"));
	CString str_end_y_top		= m_xml.GetAttrib("end_y_top"); //== "") ? -1:atoim_xml.GetAttrib("end_y_top"));
	CString str_end_y_bottom	= m_xml.GetAttrib("end_y_bottom"); //== "") ? -1:atoim_xml.GetAttrib("end_y_bottom"));
	CString str_end_y_percent	= m_xml.GetAttrib("end_y_percent"); //== "") ? -1:atoim_xml.GetAttrib("end_y_percent"));
	CString str_x_delta			= m_xml.GetAttrib("x_delta"); //== "") ? -1:atoim_xml.GetAttrib("x_delta"));
	CString str_y_delta			= m_xml.GetAttrib("y_delta"); //== "") ? -1:atoi(m_xml.GetAttrib("y_delta"));
	CString str_percent_start	= m_xml.GetAttrib("percent_start");
	CString str_percent_end		= m_xml.GetAttrib("percent_end");

	int no_stretch				= atoi(m_xml.GetAttrib("no_stretch"));
	int tile					= atoi(m_xml.GetAttrib("tile"));
	int trans					= (m_xml.GetAttrib("trans") == "") ? 0 : 1;	
	int center_strech			= atoi(m_xml.GetAttrib("center_strech"));
	COLORREF color				= CreateControlFactory::StringToRGB(m_xml.GetAttrib("trans"));

	DWORD mode = ETSLayoutMgr::NONE;
	CString sx,sy,ex,ey;
	sx = sy = ex = ey = "";
	if(str_start_x_left != ""){
		mode = mode| ETSLayoutMgr::START_X_FROM_LEFT;
		sx = str_start_x_left;
	}
	if(str_start_x_right != ""){
		mode = mode| ETSLayoutMgr::START_X_FROM_RIGHT;
		sx = str_start_x_right;
	}
	if(str_start_x_percent != ""){
		mode |= ETSLayoutMgr::START_X_PERCENT;
		sx = str_start_x_percent;
	}
	if(str_start_y_top != ""){
		mode = mode| ETSLayoutMgr::START_Y_FROM_TOP;
		sy = str_start_y_top;
	}
	if(str_start_y_bottom != ""){
		mode = mode| ETSLayoutMgr::START_Y_FROM_BOTTOM;
		sy = str_start_y_bottom;
	}
	if(str_start_y_percent != ""){
		mode = mode| ETSLayoutMgr::START_Y_PERCENT;
		sy = str_start_y_percent;
	}
	if(str_end_x_left != ""){
		mode = mode| ETSLayoutMgr::END_X_FROM_LEFT;
		ex = str_end_x_left;
	}
	if(str_end_x_right != ""){
		mode = mode| ETSLayoutMgr::END_X_FROM_RIGHT;
		ex = str_end_x_right;
	}
	if(str_end_x_percent != ""){
		mode = mode|ETSLayoutMgr::END_X_PERCENT;
		ex = str_end_x_percent;
	}
	if(str_end_y_top != ""){
		mode = mode|ETSLayoutMgr::END_Y_FROM_TOP;
		ey = str_end_y_top;
	}
	if(str_end_y_bottom != ""){
		mode = mode|ETSLayoutMgr::END_Y_FROM_BOTTOM;
		ey = str_end_y_bottom;
	}
	if(str_end_y_percent != ""){
		mode = mode|ETSLayoutMgr::END_Y_PERCENT;
		ey = str_end_y_percent;
	}
	if(str_x_delta != ""){
		mode = mode|ETSLayoutMgr::X_DELTA;
		ex = str_x_delta;
	} 
	if(str_y_delta != ""){  
		mode = mode|ETSLayoutMgr::Y_DELTA;
		ey = str_y_delta;
	}

	if(no_stretch) 
		mode = mode|ETSLayoutMgr::DRAW_NO_STRECTH;
	if(tile)
		mode = mode|ETSLayoutMgr::DRAW_TILE;
	if(trans)
	{
		mode = mode|ETSLayoutMgr::DRAW_TRANSPRANT;
		base->m_clrTrans = color;
	}
	if(center_strech)
	{
		mode = mode|ETSLayoutMgr::DRAW_CENTER_STRECH;
	}

	base->setPosition(sx, sy, ex, ey, str_percent_start, str_percent_end,(ETSLayoutMgr::layPositionMode)mode);
}

//创建multipane的多个层。
BOOL CPanesMgr::SaveMultiLayer(CString parentOri)
{
	CString tagName = m_xml.GetAttrib("name");
	ETSLayoutMgr::CPaneMulti pane;
	GetPaneMulti(tagName, pane);

	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		CString tagName = m_xml.GetTagName();
		CString paneName = m_xml.GetAttrib("name");

		if(!pane->isLayerExist(paneName))
			continue;

		if(tagName == "paneitem")
		{
			CWnd * wndPanel = this->GetWnd(paneName);
			if(wndPanel == NULL)
				continue;
			ASSERT(wndPanel != NULL);
			CRect rc;
			GetWindowRect(wndPanel->m_hWnd, &rc);
			CString value;
			if((value = m_xml.GetAttrib("sizeX")) != "" && m_xml.GetAttrib("private_setting") != "1")
			{
				value.Format("%d", rc.Width());
				m_xml.SetAttrib("sizeX", value);
			}
			if((value = m_xml.GetAttrib("sizeY")) != "" && m_xml.GetAttrib("private_setting") != "1")
			{
				value.Format("%d", rc.Height());
				m_xml.SetAttrib("sizeY", value);
			}
		}else if(tagName == "panespl")
		{
			RecursionSavePane(m_xml.GetAttrib("ori"));
			SaveSplPane();
		}else if(tagName == "pane")
		{
			RecursionSavePane(m_xml.GetAttrib("ori"));
		}else 
		{
			AfxMessageBox("无效的Pane类型: " + tagName);
			continue;
		}
	}
	m_xml.OutOfElem();
	return true;
}

//创建multipane的多个层。
BOOL CPanesMgr::CreateMultiLayer(ETSLayoutMgr::CPaneMulti *parent, ETSLayoutMgr *pLayout, ETSLayoutMgr::CPane *createPane,CString current)
{
	CString firstTagName, firstPaneName;
	CString tagName;
	BOOL	bCreateSuss = false;

	CString parentTag, parentName;
	parentTag = m_xml.GetTagName();
	parentName = m_xml.GetAttrib("name");

	firstTagName = firstPaneName = "";
	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		CString tagName = m_xml.GetTagName();
		CString paneName = m_xml.GetAttrib("name");

		if(firstPaneName == "")
		{
			firstTagName = tagName;
			firstPaneName = paneName;
		}

		if(paneName != current && !m_bJustLoadColor)
			continue;

		bCreateSuss = true;

		if(tagName == "paneitem")
		{
			ETSLayoutMgr::CPaneBase pane = this->CreatePaneItem(paneName, pLayout);

			if(m_bJustLoadColor)
				continue;

			if(pane.IsValid())
				(*parent)->addLayer(paneName, pane);
		}else if(tagName == "panespl")
		{
			ETSLayoutMgr::CPane pane = CreatePaneSpl(paneName, pLayout);

			if(m_bJustLoadColor)
			{
				RecursionCreatePane(&pane, pLayout,NULL);
				continue;
			}
			(*parent)->addLayer(paneName, pane);
			RecursionCreatePane(&pane, pLayout,NULL);
		}else if(tagName == "pane" && !m_bJustLoadColor)
		{
			ETSLayoutMgr::CPane pane = CreatePane(paneName, pLayout);

			(*parent)->addLayer(paneName, pane);
			RecursionCreatePane(&pane, pLayout,NULL);
		}else 
		{
			if(!m_bJustLoadColor)
			{
				//AfxMessageBox("无效的Pane类型: " + tagName);
			}
			continue;
		}
	}
	m_xml.OutOfElem();

	if(!bCreateSuss && firstPaneName != "")
	{
		CString value = m_xml.GetAttrib("start_x_left");
		if(value != "")
			LocateNode(parentTag, parentName,"start_x_left",value);
		else
			LocateNode(parentTag, parentName);

		CreateMultiLayer(parent, pLayout, createPane, firstPaneName);
	}
	return bCreateSuss;
}

//递归从配置文件中读取个pane参数并创建之
BOOL CPanesMgr::RecursionCreatePane(ETSLayoutMgr::CPane *parent, ETSLayoutMgr *pLayout, ETSLayoutMgr::CPane *createPane)
{
	CString tagName;
	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		CString tagName = m_xml.GetTagName();
		CString paneName = m_xml.GetAttrib("name");
		//创建一个item
		if(tagName == "paneitem")
		{
			ETSLayoutMgr::CPaneBase pane = this->CreatePaneItem(paneName, pLayout);
			if(parent != NULL && pane.IsValid())
				(*parent)<<pane;

		}
		//创建一个splitter pane
		else if(tagName == "panespl")
		{
			ETSLayoutMgr::CPane pane = this->CreatePaneSpl(paneName, pLayout);
			////AddSubPane(m_xml.GetAttrib("name"), &pane);
			if(m_bJustLoadColor)
			{
				RecursionCreatePane(&pane, pLayout,NULL);
				continue;
			}
			if(parent != NULL)
				(*parent)<<pane;
			if(createPane != NULL)
				*createPane = pane;
			RecursionCreatePane(&pane, pLayout,NULL);
		}
		//创建一个多层的pane
		else if(tagName == "panemulti")
		{
			CString strCurrent;
			ETSLayoutMgr::CPaneMulti pane = this->CreatePaneMulti(paneName, pLayout, strCurrent);

			if(m_bJustLoadColor)
			{
				RecursionCreatePane(&pane, pLayout,NULL);
				continue;
			}

			AddPaneMulti(paneName, &pane);

			if(parent != NULL && pane.IsValid())
				(*parent)<<pane;
			if(createPane != NULL)
				*createPane = pane;

			RecursionCreatePane(&pane, pLayout,NULL);
			pane->showPane(true);
		}
		//创建一个pane
		else if(tagName == "pane")
		{
			if(m_bJustLoadColor)
			{
				RecursionCreatePane(NULL, pLayout,NULL);
				continue;
			}

			ETSLayoutMgr::CPane pane = this->CreatePane(paneName, pLayout);

			if(parent != NULL && pane.IsValid())
				(*parent)<<pane;
			if(createPane != NULL)
				*createPane = pane;

			RecursionCreatePane(&pane, pLayout,NULL);
		}
		else if(tagName == "panefixed" && !m_bJustLoadColor)
		{
			int size;
			ETSLayoutMgr::layOrientation mode = this->getOrization();

			if(mode == ETSLayoutMgr::HORIZONTAL)
				size = getSizeX();
			else
				size = getSizeY();

			ETSLayoutMgr::CPaneBase pane = pLayout->itemFixed(mode , size);
			pane->SetName(m_xml.GetAttrib("name"));
			if(parent != NULL)
				(*parent)<<pane;
		}
		else
		{
			if(!m_bJustLoadColor)
			{
				//AfxMessageBox("无效的Pane类型: " + tagName);
			}
		}
	}
	m_xml.OutOfElem();

	if(createPane == NULL)
		return FALSE;
	return TRUE;
}

ETSLayoutMgr::itemType CPanesMgr::getItemType()
{
	CString str = m_xml.GetAttrib("type");

	if(str == "" || str == "window")
		return ETSLayoutMgr::TYPE_WINDOW; 
	if(str == "bmp")
		return ETSLayoutMgr::TYPE_BMP;
	if(str == "gif")
		return ETSLayoutMgr::TYPE_GIF;
	if(str == "control")
		return ETSLayoutMgr::TYPE_CONTROL;
	if(str == "button")
		return ETSLayoutMgr::TYPE_CONTROL;
	if(str == "corlor")
		return ETSLayoutMgr::TYPE_CORLOR;
	if(str == "slider")
		return ETSLayoutMgr::TYPE_CONTROL;

	return ETSLayoutMgr::TYPE_WINDOW;
}

//从配置文件中得到panel的移动属性greedy等
ETSLayoutMgr::layResizeMode CPanesMgr::getModeSize()
{	
	ETSLayoutMgr::layResizeMode modeSize = ETSLayoutMgr::GREEDY;

	if(m_xml.GetAttrib("greedy") == "1")
		modeSize = modeSize |  ETSLayoutMgr::GREEDY;

	if(m_xml.GetAttrib("relative_horz") == "1")
		modeSize = modeSize | ETSLayoutMgr::RELATIVE_HORZ;

	if(m_xml.GetAttrib("relative_vert") == "1")
		modeSize  = modeSize | ETSLayoutMgr::RELATIVE_VERT;

	if(m_xml.GetAttrib("absolute_horz") == "1")
		modeSize  = modeSize | ETSLayoutMgr::ABSOLUTE_HORZ;

	if(m_xml.GetAttrib("absolute_vert") == "1")
		modeSize  = modeSize | ETSLayoutMgr::ABSOLUTE_VERT;

	if(m_xml.GetAttrib("align_v") == "center")
		modeSize = modeSize | ETSLayoutMgr::ALIGN_VCENTER;

	if(m_xml.GetAttrib("align_h") == "center")
		modeSize = modeSize | ETSLayoutMgr::ALIGN_HCENTER;

	if(m_xml.GetAttrib("align_v") == "bottom")
		modeSize = modeSize | ETSLayoutMgr::ALIGN_BOTTOM;

	if(m_xml.GetAttrib("align_h") == "right")
		modeSize = modeSize | ETSLayoutMgr::ALIGN_RIGHT;

	return modeSize;
}

void CPanesMgr::AddPaneMulti(CString name, ETSLayoutMgr::CPaneMulti *pane)
{
	m_mapPaneMulti.SetAt(name, *pane);
}

BOOL CPanesMgr::GetPaneMulti(CString name,  ETSLayoutMgr::CPaneMulti &pane)
{
	ETSLayoutMgr::CPaneMulti temp;
	if(!m_mapPaneMulti.Lookup(name, temp))
		return FALSE;
	pane = temp;
	return TRUE;
}

void CPanesMgr::AddWndLayoutMgr(HWND hWnd, ETSLayoutMgr *pLayout)
{
	m_mapLayoutMgr.SetAt(hWnd, pLayout);
}

void CPanesMgr::AddBkLayoutMgr(HWND hWnd, ETSLayoutMgr *pLayout)
{
	m_mapBkLayoutMgr.SetAt(hWnd, pLayout);
}

ETSLayoutMgr * CPanesMgr::GetWndLayoutMgr(HWND hWnd)
{
	ETSLayoutMgr *temp = NULL;
	m_mapLayoutMgr.Lookup(hWnd, temp);

	return temp;
}

ETSLayoutMgr * CPanesMgr::GetBkLayoutMgr(HWND hWnd)
{
	ETSLayoutMgr *temp = NULL;
	m_mapBkLayoutMgr.Lookup(hWnd, temp);
	return temp;
}

void CPanesMgr::PaintBkGround(HWND hWnd, CDC *pDC, BOOL bUseMemDC)
{
	ETSLayoutMgr *pLayout = GetBkLayoutMgr(hWnd);
	if(pLayout != NULL)
		pLayout->UpdateBkLayout(pDC, bUseMemDC);
}

void CPanesMgr::RegisterWnd(CString name, CWnd *panes)
{
	CHostWndMgr::GetInstance()->KeepOriParent(panes);
	m_mapPanels.SetAt(name, (CObject*)panes);
};

CWnd * CPanesMgr::GetWnd(CString name)
{
	CWnd *wndPanel = NULL;
	m_mapPanels.Lookup(name , (CObject *&)wndPanel);

	if(wndPanel == NULL && !m_bJustLoadColor)
	{
		CString strName = m_xml.GetAttrib("name");
		CString strTag = m_xml.GetTagName();
		CString value = m_xml.GetAttrib("start_x_left");

		AfxGetUIManager()->CreateUIWindow(name);

		m_mapPanels.Lookup(name , (CObject *&)wndPanel);

		if(strName != "" && wndPanel != NULL)
		{			
			if(value != "")
				LocateNode(strTag, strName,"start_x_left",value);
			else
				LocateNode(strTag, strName);
		}
	}
	return wndPanel;
}

void CPanesMgr::RegisterCtrl(CWnd * pWnd, CString name, CWnd *pCtrl)
{
	m_mapCtrls.SetAt(name, (CObject *) pCtrl);
}

CWnd * CPanesMgr::GetCtrl(HWND hWnd, CString name)
{
	CWnd *wndPanel = NULL;
	m_mapCtrls.Lookup(name , (CObject *&)wndPanel);
	return wndPanel;
};

void CPanesMgr::SetLayoutBorder(ETSLayoutMgr *pLayout)
{
	CString cx, cy, cx1,cy1;

	cx = m_xml.GetAttrib("border_l");
	cy = m_xml.GetAttrib("border_t");
	cx1 = m_xml.GetAttrib("border_r");
	cy1 = m_xml.GetAttrib("border_b");

	pLayout->setRootBorders(atoi(cx), atoi(cy), atoi(cx1), atoi(cy1));
}

//遍历pane.xml里各个控件、窗口的配置，读取他们的color设置。
void	CPanesMgr::ChangeWndColor(CWnd *pParent, CString name, CString strControlName)
{
	if(!::IsWindow(pParent->m_hWnd))
		return;
	//m_bJustLoadColor为true时，只读取color，不重新创建各个pane
	m_bJustLoadColor = true;

	if(FindNode("rootpanel",name))
	{
		ETSLayoutMgr *pLayout = GetWndLayoutMgr(pParent->m_hWnd);
		RecursionCreatePane(NULL, pLayout, NULL);
	}
	else if(FindNode("wndpanel", name))
	{
		m_xml.IntoElem();

		while(m_xml.FindChildElem())
		{
			if(m_xml.GetTagName() == "controlpanel" && m_xml.GetAttrib("name") == strControlName)
			{
				ETSLayoutMgr *pLayout = GetWndLayoutMgr(pParent->m_hWnd);
				RecursionCreatePane(NULL, pLayout, NULL);
				break;
			}
		}
	}
	m_bJustLoadColor = false;
}


void CPanesMgr::CreateWndPane(CWnd *pParent, CString name, bool bAppStart)
{
	if(!::IsWindow(pParent->m_hWnd))
		return;

	ETSLayoutMgr *pLayout;

	if(bAppStart)
	{
		NotifhyLayoutInitBegin();
	}

	if(FindNode("rootpanel",name))
	{
		pLayout = NULL;//GetWndLayoutMgr(pParent->m_hWnd);
		m_modeDisplay = DISPLAY_FULL_MODE;
		if(pLayout == NULL)
		{
			pLayout = new ETSLayoutMgr(pParent);
			pLayout->SetName(name, "");
			AddWndLayoutMgr(pParent->m_hWnd, pLayout);
		}

		pLayout->CreateRoot(getOrization());
		SetLayoutBorder(pLayout);
		RecursionCreatePane(&pLayout->m_RootPane, pLayout, NULL);
		pLayout->UpdateLayout();
		NotifyUpdateLayout(pParent->m_hWnd);
	}
	if(bAppStart)
		NotifyLayoutInitFinished();
	return;
}

void CPanesMgr::CreateControlPane(CWnd *pParent, CString strWndName, CString strControlName)
{
	if(!::IsWindow(pParent->m_hWnd))
		return;

	DWORD count = GetTickCount();

	if(!FindNode("wndpanel",strWndName))
		return;
	if(strControlName == "")
		strControlName = m_xml.GetAttrib("mode");
	//和以前版本保持兼容
	if(strControlName == "")
		strControlName = "normal";

	ETSLayoutMgr *pLayout = GetWndLayoutMgr(pParent->m_hWnd);
	if(pLayout != NULL)
		delete pLayout;

	pLayout = new ETSLayoutMgr(pParent);
	pLayout->SetName(strWndName, strControlName);
	AddWndLayoutMgr(pParent->m_hWnd, pLayout);

	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		if(m_xml.GetTagName() == "controlpanel" && m_xml.GetAttrib("name") == strControlName)
		{
			//CString rgn = m_xml.GetAttrib("parent_rgn");
			//CreateControlFactory::SetWindowRgn(rgn, pParent->GetParent(), m_pSkin, RGB(255,0,0));
			CString value;
			if((value = m_xml.GetAttrib("trans")) == "1")
			{
				COLORREF clr = CreateControlFactory::StringToRGB(m_xml.GetAttrib("trans_color"), false);

				HMODULE   hModule = GetModuleHandle("User32.DLL");   
				if(hModule   !=   NULL)   
				{   
					typedef   BOOL   (WINAPI* SETLAYEREDWND)( HWND, COLORREF,BYTE,DWORD);   
					SETLAYEREDWND   SetLayeredWindowPtr   =   NULL;   
					SetLayeredWindowPtr =   (SETLAYEREDWND)GetProcAddress(hModule,   "SetLayeredWindowAttributes");   
					if(SetLayeredWindowPtr)   
					{   
						LONG   lStyle   =   GetWindowLong(pParent->m_hWnd,   GWL_EXSTYLE)   |   0x00080000;   
						SetWindowLong( pParent->m_hWnd,   GWL_EXSTYLE,   lStyle);   
						SetLayeredWindowPtr(pParent->m_hWnd, clr, 255, LWA_COLORKEY|2);   
					} 
				}
			}

			if(m_xml.GetAttrib("fixed") == "1")
				pLayout->AddParameter("fixed", "1");
			
			CString x, y;

			x = m_xml.GetAttrib("sizeXMin");
			if(x != "")
				pLayout->AddParameter("sizeXMin", x);

			y = m_xml.GetAttrib("sizeYMin");
			if(y != "")
				pLayout->AddParameter("sizeYMin", y);

			SetWindowPos(pParent);

			pLayout->CreateRoot(getOrization());
			SetLayoutBorder(pLayout);

			RecursionCreatePane(&pLayout->m_RootPane, pLayout, NULL);
			pLayout->UpdateLayout();
			NotifyUpdateLayout(pParent->m_hWnd);
			return;
		}
	}
}

void CPanesMgr::LoadChangeColorData()
{
	if(!FindNode("changecolor", ""))
		return;

	CString basehue = m_xml.GetAttrib("basehue");
	m_pSkin->m_ChangeColorData.baseHue = atoi(basehue);
	m_xml.IntoElem();
	m_pSkin->m_ChangeColorData.vecBaseColors.clear();
	while(m_xml.FindChildElem())
	{
		if(m_xml.GetTagName() == "color")
		{
			basecolor b;
			b.name = m_xml.GetAttrib("name");
			b.light = atoi(m_xml.GetAttrib("light"));
			b.hue = atoi(m_xml.GetAttrib("hue"));
			b.sat = atoi(m_xml.GetAttrib("sat"));
			m_pSkin->m_ChangeColorData.vecBaseColors.push_back(b);
		}
	}

	if(!FindNode("bitmaps", ""))
		return;
	m_xml.IntoElem();
	while(m_xml.FindChildElem())
	{
		CString name = m_xml.GetAttrib("name");
		BOOL bDontChangeColor = (atoi(m_xml.GetAttrib("dont_change_color")) == 1);
		m_pSkin->SetChangeColorSign(name, bDontChangeColor);
	}
}
void CPanesMgr::DeleteSubjectData(CString name)
{
	for(vector<OneSubject>::iterator itor =  m_pSkin->m_subjectData.vecSubject.begin();itor != m_pSkin->m_subjectData.vecSubject.end();itor++)
	{
		OneSubject tempsub = *itor;
		if(tempsub.strSubName.c_str() == name)
		{
			 m_pSkin->m_subjectData.vecSubject.erase(itor);
			 return;
		}
	}
}

void CPanesMgr::InsertSubjectData(CString name)
{
	string strSkinPath = m_pSkin->GetSkinPath();
	string subPath = strSkinPath + "\\SbujectSkin\\";	
	subPath += name;
	string strPath = subPath+ "\\conf.ini";

	if( !YL_FileInfo::IsValid( strPath) )
	{
		return;
	}
	char szSkinName[256];
	GetPrivateProfileString("CONF","name","",szSkinName,255,strPath.c_str());

	char szHue[256];
	GetPrivateProfileString("CONF","hue","",szHue,255,strPath.c_str());

	char szSat[256];
	GetPrivateProfileString("CONF","sat","",szSat,255,strPath.c_str());

	char szLight[256];
	GetPrivateProfileString("CONF","light","",szLight,255,strPath.c_str());

	char szWhite[256];
	GetPrivateProfileString("CONF","white","",szWhite,255,strPath.c_str());

	char szShowName[256];
	GetPrivateProfileString("CONF","showname","",szShowName,255,strPath.c_str());

	char szPos[256];
	GetPrivateProfileString("CONF","pos","",szPos,255,strPath.c_str());

	char szSystem[256];
	GetPrivateProfileString("CONF","systempos","",szSystem,255,strPath.c_str());

	//lefttop
	char szLeftTopx[256];
	GetPrivateProfileString("CONF","lefttopx","",szLeftTopx,255,strPath.c_str());

	char szLeftTopy[256];
	GetPrivateProfileString("CONF","lefttopy","",szLeftTopy,255,strPath.c_str());

	//leftbottom
	char szLeftBottomx[256];
	GetPrivateProfileString("CONF","leftbottomx","",szLeftBottomx,255,strPath.c_str());

	char szLeftBottomy[256];
	GetPrivateProfileString("CONF","leftbottomy","",szLeftBottomy,255,strPath.c_str());

	//righttop
	char szRightTopx[256];
	GetPrivateProfileString("CONF","righttopx","",szRightTopx,255,strPath.c_str());

	char szRightTopy[256];
	GetPrivateProfileString("CONF","righttopy","",szRightTopy,255,strPath.c_str());

	//rightbottom
	char szRightBottomx[256];
	GetPrivateProfileString("CONF","rightbottomx","",szRightBottomx,255,strPath.c_str());

	char szRightBottomy[256];
	GetPrivateProfileString("CONF","rightbottomy","",szRightBottomy,255,strPath.c_str());

	char szver[256];
	memset(szver,0,256);
	GetPrivateProfileString("CONF","ver","",szver,255,strPath.c_str());

	char szID[256];
	GetPrivateProfileString("CONF","id","",szID,255,strPath.c_str());

// 	if( strcmp(szver,SUB_VERSION) > 0 )
// 	{
// 		return;
// 	}

	OneSubject oneSubject;
	YL_FileInfo::GetFileName(strPath,oneSubject.strDirName);
	oneSubject.strSubName = szSkinName;
	oneSubject.bShowWhite = atoi(szWhite);
	oneSubject.subColor   = gBitMapOperate.HLStoRGB( ((double)atoi(szHue)) / 240, ((double)atoi(szLight))/ 100 , ((double)atoi(szSat)) / 100 );
	oneSubject.iLight	  = 120;
	oneSubject.dSystemPos = atof(szSystem);
	oneSubject.iPos		  = atoi(szPos);
	oneSubject.strShowName = szShowName;
	oneSubject.ptLeftBottom.x = (strlen(szLeftBottomx)==0)? -1000 :atoi(szLeftBottomx);
	oneSubject.ptLeftBottom.y = (strlen(szLeftBottomx)==0 )? -1000 :atoi(szLeftBottomy);
	oneSubject.ptLeftTop.x = (strlen(szLeftTopx)==0 )? -1000 :atoi(szLeftTopx);
	oneSubject.ptLeftTop.y = (strlen(szLeftTopy)==0 )? -1000 :atoi(szLeftTopy);
	oneSubject.ptRightBottom.x = (strlen(szRightBottomx)==0 )? -1000 :atoi(szRightBottomx);
	oneSubject.ptRightBottom.y = (strlen(szRightBottomy)==0 )? -1000 :atoi(szRightBottomy);
	oneSubject.ptRightTop.x = (strlen(szRightTopx)==0 )? -1000 :atoi(szRightTopx);
	oneSubject.ptRightTop.y = (strlen(szRightTopy)==0 )? -1000 :atoi(szRightTopy);
	oneSubject.nID	= atoi(szID);

	oneSubject.bNeedDown = false;
	
	if(oneSubject.dSystemPos==0&&oneSubject.iPos==0)
	{
		//获得最大的pos值，以确定插入皮肤的位置
		int pos = 0;
		for(int i=0;i < m_pSkin->m_subjectData.vecSubject.size();i++)
		{
			OneSubject tempsub = m_pSkin->m_subjectData.vecSubject[i];
			if(tempsub.iPos > pos)
				pos = tempsub.iPos;
		}
		pos++;
		oneSubject.iPos = pos;
		char sznewPos[256];
		itoa(pos,sznewPos,10);
		WritePrivateProfileString("CONF","pos",sznewPos,strPath.c_str());
		//插在默认皮肤后面
		m_pSkin->m_subjectData.vecSubject.insert(++m_pSkin->m_subjectData.vecSubject.begin(),oneSubject);
	}else 
	{
		m_pSkin->m_subjectData.vecSubject.push_back(oneSubject);
	}
}

void CPanesMgr::CreateLoadSubjectTread()
{
	DWORD dwThreadId, dwThrdParam = 1; 
	HANDLE hThread; 
	m_SubEvent = CreateEvent(NULL,true,FALSE,NULL);
	hThread = CreateThread( 
		NULL,                        // default security attributes 
		0,                           // use default stack size  
		(LPTHREAD_START_ROUTINE)LoadSubjectProc,                  // thread function 
		this,                // argument to thread function 
		0,                           // use default creation flags 
		&dwThreadId);                // returns the thread identifier 
	CloseHandle(hThread);
}

LPVOID CPanesMgr::LoadSubjectProc(LPVOID lparg)
{
	((CPanesMgr *)lparg)->LoadChangeSubjectData();
	return 0;
}

void CPanesMgr::WaitforLoadSub()
{
	WaitForSingleObject(m_SubEvent,INFINITE);
}

void CPanesMgr::LoadChangeSubjectData()
{
	m_pSkin->m_subjectData.bHasSub = false;

	string strSkinPath = m_pSkin->GetSkinPath();	

	m_pSkin->m_subjectData.bHasSub = true;

	vector<string> vecDirName;

	YL_DirInfo::FindFileAndDir( strSkinPath + "\\SbujectSkin",&vecDirName,NULL,false);

	m_pSkin->m_subjectData.vecSubject.clear();
//	m_pSkin->m_subjectData.vecSubject.resize(vecDirName.size());

	vector<OneSubject> vecSysSubs;
	vector<OneSubject> vecUserSubs;

	int	iSubjectPos = 0;

	for(size_t idx = 0;idx < vecDirName.size();idx++ )
	{
		string strPath = vecDirName[idx] + "\\conf.ini";
		
		string strMidPath = vecDirName[idx] + "\\TopPanelBKMid.bmp";
		if(!YL_FileInfo::IsValid(strMidPath))
			continue;
		char szSkinName[256];
		GetPrivateProfileString("CONF","name","",szSkinName,255,strPath.c_str());

		char szHue[256];
		GetPrivateProfileString("CONF","hue","",szHue,255,strPath.c_str());

		char szSat[256];
		GetPrivateProfileString("CONF","sat","",szSat,255,strPath.c_str());

		char szLight[256];
		GetPrivateProfileString("CONF","light","",szLight,255,strPath.c_str());

		char szWhite[256];
		GetPrivateProfileString("CONF","white","",szWhite,255,strPath.c_str());

		char szShowName[256];
		GetPrivateProfileString("CONF","showname","",szShowName,255,strPath.c_str());

		char szPos[256];
		GetPrivateProfileString("CONF","pos","",szPos,255,strPath.c_str());

		char szSystem[256];
		GetPrivateProfileString("CONF","systempos","",szSystem,255,strPath.c_str());

		//lefttop
		char szLeftTopx[256];
		GetPrivateProfileString("CONF","lefttopx","",szLeftTopx,255,strPath.c_str());

		char szLeftTopy[256];
		GetPrivateProfileString("CONF","lefttopy","",szLeftTopy,255,strPath.c_str());

		//leftbottom
		char szLeftBottomx[256];
		GetPrivateProfileString("CONF","leftbottomx","",szLeftBottomx,255,strPath.c_str());

		char szLeftBottomy[256];
		GetPrivateProfileString("CONF","leftbottomy","",szLeftBottomy,255,strPath.c_str());

		//righttop
		char szRightTopx[256];
		GetPrivateProfileString("CONF","righttopx","",szRightTopx,255,strPath.c_str());

		char szRightTopy[256];
		GetPrivateProfileString("CONF","righttopy","",szRightTopy,255,strPath.c_str());

		//rightbottom
		char szRightBottomx[256];
		GetPrivateProfileString("CONF","rightbottomx","",szRightBottomx,255,strPath.c_str());

		char szRightBottomy[256];
		GetPrivateProfileString("CONF","rightbottomy","",szRightBottomy,255,strPath.c_str());

		char szver[256];
		memset(szver,0,256);
		GetPrivateProfileString("CONF","ver","",szver,255,strPath.c_str());

		char szID[256];
		GetPrivateProfileString("CONF","id","",szID,255,strPath.c_str());

		if( strcmp(szver,SUB_VERSION) > 0 )
		{
			continue;
		}

		OneSubject oneSubject;
		YL_FileInfo::GetFileName(vecDirName[idx],oneSubject.strDirName);
		oneSubject.strSubName = szSkinName;
		oneSubject.bShowWhite = atoi(szWhite);
		oneSubject.subColor   = gBitMapOperate.HLStoRGB( ((double)atoi(szHue)) / 240, ((double)atoi(szLight))/ 100 , ((double)atoi(szSat)) / 100 );
		oneSubject.iLight	  = 120;
		oneSubject.dSystemPos = atof(szSystem);
		oneSubject.iPos		  = atoi(szPos);
		oneSubject.strShowName = szShowName;
		oneSubject.ptLeftBottom.x = (strlen(szLeftBottomx)==0)? -1000 :atoi(szLeftBottomx);
		oneSubject.ptLeftBottom.y = (strlen(szLeftBottomx)==0 )? -1000 :atoi(szLeftBottomy);
		oneSubject.ptLeftTop.x = (strlen(szLeftTopx)==0 )? -1000 :atoi(szLeftTopx);
		oneSubject.ptLeftTop.y = (strlen(szLeftTopy)==0 )? -1000 :atoi(szLeftTopy);
		oneSubject.ptRightBottom.x = (strlen(szRightBottomx)==0 )? -1000 :atoi(szRightBottomx);
		oneSubject.ptRightBottom.y = (strlen(szRightBottomy)==0 )? -1000 :atoi(szRightBottomy);
		oneSubject.ptRightTop.x = (strlen(szRightTopx)==0 )? -1000 :atoi(szRightTopx);
		oneSubject.ptRightTop.y = (strlen(szRightTopy)==0 )? -1000 :atoi(szRightTopy);
		oneSubject.nID	= atoi(szID);

		if(oneSubject.strSubName == "ra")
		{
			TRACE("subject: %d\n" , &oneSubject);
		}
		if(	!YL_FileInfo::IsValid( vecDirName[idx] + "\\TopPanelBKMid.bmp") )
		{
			oneSubject.bNeedDown = true;
		}
		else
		{
			oneSubject.bNeedDown = false;
		}

		if( atoi( szPos ) > iSubjectPos )
		{
			iSubjectPos = atoi( szPos );
		}

		if( oneSubject.dSystemPos > 0 )
		{
			vecSysSubs.push_back( oneSubject );
		}
		else
		{
			vecUserSubs.push_back( oneSubject );
		}
	}
	
	vector<OneSubject> vecUser2Subs;
	for(size_t idxUser = 0;idxUser < vecUserSubs.size();idxUser++)
	{
		OneSubject& one = vecUserSubs[idxUser];
		if( one.iPos == 0 )
		{
			one.iPos = ++iSubjectPos;
			vecUser2Subs.insert(vecUser2Subs.begin(),one);

			string strconf = strSkinPath + "\\SbujectSkin\\" + one.strDirName+ "\\conf.ini";
			char szPos[256];
			sprintf(szPos,"%d",one.iPos);			
			BOOL bWrite = WritePrivateProfileString("CONF","pos",szPos,strconf.c_str());
			DWORD dw = GetLastError();			
		}
		else
		{
			vector<OneSubject>::iterator itSub = vecUser2Subs.begin();
			while( itSub != vecUser2Subs.end() )
			{
				if( one.iPos >= itSub->iPos )
				{
					vecUser2Subs.insert(itSub,one);
					break;
				}
				itSub++;
			}
			if( itSub == vecUser2Subs.end() ) 
			{
				vecUser2Subs.push_back( one );
			}
		}
	}

	vector<OneSubject> vecSys2Subs;
	for(size_t idxSys = 0;idxSys < vecSysSubs.size();idxSys++)
	{
		OneSubject& one = vecSysSubs[idxSys];
		
		vector<OneSubject>::iterator itSub = vecSys2Subs.begin();
		while( itSub != vecSys2Subs.end() )
		{
			if( one.dSystemPos <= itSub->dSystemPos )
			{
				vecSys2Subs.insert(itSub,one);
				break;
			}
			itSub++;
		}
		if( itSub == vecSys2Subs.end() ) 
		{
			vecSys2Subs.push_back( one );
		}		
	}

	for(size_t idx = 0;idx < vecUser2Subs.size();idx++ )
	{
		m_pSkin->m_subjectData.vecSubject.push_back( vecUser2Subs[idx] );
	}

	for(size_t idx = 0;idx < vecSys2Subs.size();idx++ )
	{
		if( vecSys2Subs[idx].strSubName == "1")
		{
			m_pSkin->m_subjectData.vecSubject.insert( m_pSkin->m_subjectData.vecSubject.begin(),vecSys2Subs[idx] );
		}
		else
		{
			m_pSkin->m_subjectData.vecSubject.push_back( vecSys2Subs[idx] );
		}
	}
	if(m_SubEvent!=NULL)
	{
		SetEvent(m_SubEvent);
		CloseHandle(m_SubEvent);
		m_SubEvent = NULL;
	}
}
void CPanesMgr::LoadColors()
{
	if(!FindNode("colors", ""))
		return;

	m_pSkin->m_clrMap.clear();
	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		if(m_xml.GetTagName() == "color")
		{
			bool bDontChangeColor = atoi(m_xml.GetAttrib("dont_change_color")) == 1;
			if(bDontChangeColor)
				m_pSkin->PutColor(m_xml.GetAttrib("name"), CreateControlFactory::StringToRGB(m_xml.GetAttrib("value"), false));
			else
				m_pSkin->PutColor(m_xml.GetAttrib("name"), CreateControlFactory::StringToRGB(m_xml.GetAttrib("value")));
		}
	}
	m_xml.ResetPos();
	if( m_xml.FindElem("root") )
	{
		m_skinColor	= CreateControlFactory::StringToRGB(m_xml.GetAttrib("color"));
	}
}

void CPanesMgr::SetWindowPos(CWnd *pWnd)
{
	CRect rc;
	CWnd* pHostWnd = CHostWndMgr::GetInstance()->GetHostWnd(pWnd);

	if(pHostWnd != NULL)
		pWnd = pHostWnd;
	pWnd->GetWindowRect(&rc);

	BOOL bSizeChanged = false;
	if(this->getSizeX() != 0)
	{
		rc.right = rc.left + getSizeX();
		bSizeChanged = true;
	}
	if(this->getSizeY() != 0)
	{
		bSizeChanged = true;
		rc.bottom = rc.top + getSizeY();
	}
	CWnd *parent = pWnd->GetParent();
	if(parent != NULL)
		parent->ScreenToClient(&rc);

	if(bSizeChanged)
		pWnd->SetWindowPos(NULL, 0,0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOACTIVATE);
}

void CPanesMgr::CreateBmpPane(CWnd *pParent, CString strWndName, CString strBackGroundName)
{
	ETSLayoutMgr *pLayout = GetBkLayoutMgr(pParent->m_hWnd);
	if(pLayout != NULL)
	{
		delete pLayout;
	}
		pLayout = new ETSLayoutMgr(pParent);
		pLayout->SetName(strWndName, strBackGroundName);
		AddBkLayoutMgr(pParent->m_hWnd, pLayout);
		pLayout->m_pSkin = this->m_pSkin;

	if(!FindNode("wndpanel", strWndName))
		return;
	if(strBackGroundName == "")
		strBackGroundName = m_xml.GetAttrib("mode");
	//和以前版本保持兼容
	if(strBackGroundName == "")
		strBackGroundName = "normal";

	m_xml.IntoElem();

	while(m_xml.FindChildElem())
	{
		if(m_xml.GetTagName() == "background" && m_xml.GetAttrib("name") == strBackGroundName)
		{
			CString str = m_xml.GetAttrib("ori");
			pLayout->CreateRoot(getOrization());

			COLORREF clrBackground = CreateControlFactory::StringToRGB(m_xml.GetAttrib("background_color"));

			pLayout->setOriBackGroundColor(clrBackground);
			pLayout->setBackgroundColor(clrBackground);

			RecursionCreatePane(&pLayout->m_RootPane, pLayout, NULL);
			//	pLayout->UpdateLayout();
			return;
		}
	}
}

bool CPanesMgr::LocateNode(CString tag, CString name, CString key, CString value)
{
	BOOL bFinished = FALSE;
	CString csTag;
	CString csName;
	CString csValue;
//	m_xml.IntoElem();
	csTag = m_xml.GetTagName();
	m_xml.ResetPos();
	if ( ! m_xml.FindChildElem() )
		bFinished = TRUE;
	bool bMatchMode = false;
	while ( ! bFinished )
	{
		// Process element
		csTag = m_xml.GetTagName();
		csName = m_xml.GetAttrib("name");
		if(key != "")
			csValue = m_xml.GetAttrib(key);
		else
			csValue = "";
		if(csTag == tag && name == csName && csValue == value)
			return true;

		m_xml.IntoElem();
		csTag = m_xml.GetTagName();

		// Next element (depth first)
		BOOL bFound = m_xml.FindChildElem();
		while ( ! bFound && ! bFinished )
		{
			if ( m_xml.OutOfElem() )
				bFound = m_xml.FindChildElem();
			else
				bFinished = TRUE;
		}
	}
	return false;
}

void CPanesMgr::ShowLayer(CString paneName, CString layerName)
{
	ETSLayoutMgr::CPaneMulti pane;
	GetPaneMulti(paneName,pane);
	if(pane.IsValid())
	{
		CString old = pane->getCurrent();
		if(old == layerName)
			return;
	//	CString value = m_xml.GetAttrib("start_y_bottom");
		if(!pane->isLayerExist(layerName))
		{

			if(!LocateNode("panemulti", paneName, "start_y_top", pane->m_start_y))
				LocateNode("panemulti", paneName, "start_y_bottom", pane->m_start_y);

			CreateMultiLayer(&pane, pane->m_pMgr, NULL, layerName);
			SetPositionParam(pane.GetPaneBase());
		}

		if(pane->setCurrent(layerName))
			NotifyLayerChanged(paneName, old, layerName);
	}
}

CString CPanesMgr::GetCurrentLayer(CString paneName)
{
	ETSLayoutMgr::CPaneMulti pane;
	GetPaneMulti(paneName,pane);
	if(pane.IsValid())
		return ((ETSLayoutMgr::PaneMulti*)pane.GetPaneBase())->getCurrent();
	return "";
}

CString	CPanesMgr::GetWndParameter(HWND hWnd, CString parameter)
{
	ETSLayoutMgr* pLayout = GetWndLayoutMgr(hWnd);
	if(pLayout == NULL)
		return "";
	CString value;
	pLayout->GetValue(parameter, value);
	return value;
}

void CPanesMgr::AddBkParameter(HWND hWnd, CString parameter, CString value)
{
	ETSLayoutMgr* pLayout = GetBkLayoutMgr(hWnd);
	if(pLayout == NULL)
		return;
	pLayout->AddParameter(parameter, value);
}

void CPanesMgr::UpdateLayout(HWND hWnd)
{
	ETSLayoutMgr* pLayout = GetWndLayoutMgr(hWnd);
	if(pLayout == NULL)
		return;
	pLayout->UpdateLayout();
	NotifyUpdateLayout(hWnd);
}

bool CPanesMgr::AddLayer(CWnd *pMainWnd, CWnd *pWnd, CString paneName, CString layerName)
{
	ETSLayoutMgr::CPaneMulti pane;
	GetPaneMulti(paneName,pane);
	CString errMsg;

	if(!pane.IsValid())
	{
		errMsg = "无效的MultiPane:" + paneName;
		//AfxMessageBox(errMsg);
		return false;
	}

	CRect rc = pane->getPaneRect();

	ETSLayoutMgr* pLayout = GetWndLayoutMgr(pMainWnd->m_hWnd);

	if(pLayout == NULL)
	{
		errMsg = "无效的Main Layout";
		//AfxMessageBox(errMsg);
		return false;
	}

	ETSLayoutMgr::CPaneBase paneitem = pLayout->item(pWnd);

	((ETSLayoutMgr::PaneItem *)paneitem.GetPaneBase())->setItemType(ETSLayoutMgr::TYPE_WINDOW);
	paneitem->SetName(layerName);

	pane->addLayer(layerName, paneitem);
	return true;
}

BOOL CPanesMgr::SetBkAttrib(CWnd *pWnd, CString paneName, CString name, CString value, bool bRedraw)
{
	ETSLayoutMgr* pLayout = GetBkLayoutMgr(pWnd->m_hWnd);

	if(pLayout != NULL && pLayout->m_RootPane.IsValid())
	{
		ETSLayoutMgr::CPaneBase base  = pLayout->m_RootPane->GetSubPane(paneName);
		if(base.IsValid())
		{
			if(name == "x_start")
			{
				base.GetPaneBase()->m_start_x = value;
			}
			else if(name == "y_start")
			{
				base.GetPaneBase()->m_start_y = value;		
			}
			else if(name == "x_end")
			{
				base.GetPaneBase()->m_end_x = value;		
			}
			else if(name == "y_end")
			{
				base.GetPaneBase()->m_end_y = value;		
			}
			else if(name == "center_strech")
			{
				if(value == "1")
					base.GetPaneBase()->m_posMode = (ETSLayoutMgr::layPositionMode)(base.GetPaneBase()->m_posMode | ETSLayoutMgr::DRAW_CENTER_STRECH);
				else
					base.GetPaneBase()->m_posMode = (ETSLayoutMgr::layPositionMode)((DWORD)base.GetPaneBase()->m_posMode & (~ETSLayoutMgr::DRAW_CENTER_STRECH));
			}
			else if(name == "tile")
			{
				if(value == "1")
					base.GetPaneBase()->m_posMode = (ETSLayoutMgr::layPositionMode)(base.GetPaneBase()->m_posMode | ETSLayoutMgr::DRAW_TILE);
				else
					base.GetPaneBase()->m_posMode = (ETSLayoutMgr::layPositionMode)((DWORD)base.GetPaneBase()->m_posMode & (~ETSLayoutMgr::DRAW_TILE));
			}

			if(bRedraw)
			{

				CRect rc1 = base->m_paneParent->getPaneRect();
				rc1.top -= 1;
				base->m_paneParent->resizeTo(rc1, false);
				rc1.top += 1;
				base->m_paneParent->resizeTo(rc1, true);
			}
			return TRUE;
		}
	}
	return TRUE;
}

BOOL CPanesMgr::ShowPane(CWnd *pWnd, CString paneName, bool bShow)
{
	ETSLayoutMgr* pLayout = GetWndLayoutMgr(pWnd->m_hWnd);

	if(pLayout != NULL && pLayout->m_RootPane.IsValid())
	{
		ETSLayoutMgr::CPaneBase base  = pLayout->m_RootPane->GetSubPane(paneName);
		if(base.IsValid())
		{
			base->showPane(bShow);
			return true;
		}
	}
	return false;
}

BOOL CPanesMgr::SetPaneSize(CWnd *pWnd, CString paneName, CRect rc, bool bAffactParent)
{
	ETSLayoutMgr* pLayout = GetWndLayoutMgr(pWnd->m_hWnd);
	
	if(pLayout != NULL && pLayout->m_RootPane.IsValid())
	{
		ETSLayoutMgr::CPaneBase base  = pLayout->m_RootPane->GetSubPane(paneName);
		if(base.IsValid())
		{
			//base->m_rcRect = rc;
			DWORD modeSize = base->modeResize();
			base->m_modeResize = ETSLayoutMgr::GREEDY;
			base->resizeTo(rc, true );
			base->m_modeResize = modeSize;

			if(bAffactParent)
			{
				CRect rc1 = base->m_paneParent->getPaneRect();
				rc1.top -= 1;
				base->m_paneParent->resizeTo(rc1, false);
				rc1.top += 1;
				base->m_paneParent->resizeTo(rc1, true);
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPanesMgr::GetPaneItemRect(CWnd *pWnd, CString subName ,CRect &rc)
{
	ETSLayoutMgr* pLayout = GetWndLayoutMgr(pWnd->m_hWnd);

	if(pLayout != NULL && pLayout->m_RootPane.IsValid())
	{
		ETSLayoutMgr::CPaneBase base  = pLayout->m_RootPane->GetSubPane(subName);
		if(base.IsValid())
		{
			rc = base->getPaneRect();
			return true;
		}
	}

	pLayout = GetBkLayoutMgr(pWnd->m_hWnd);

	if(pLayout != NULL && pLayout->m_RootPane.IsValid())
	{
		ETSLayoutMgr::CPaneBase base  = pLayout->m_RootPane->GetSubPane(subName);
		if(base.IsValid())
		{
			rc = base->getPaneRect();
			return true;
		}
	}

	return false;
}

bool CPanesMgr::OnChangeSkinTemplate(CString strPath,CString strSub)
{ 
	static BOOL bIsChangeDisplayModeDlgVisuabl=FALSE;
	if (bIsChangeDisplayModeDlgVisuabl)
	{
		return FALSE;
	}
	bIsChangeDisplayModeDlgVisuabl=TRUE;	

	if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		m_bWaitOutFullScreen = TRUE;
		m_strWaitSkin = strPath;
		return false;
	}
	CWaitCursor cur;
	DWORD count = 0;
	count = ::GetTickCount();
	CString oldPath = m_pSkin->m_strSkinPath;

	// 保存皮肤路径
	CString skinPath = strPath;
	AfxUserConfig* pUserConfig = AfxGetUserConfig();
	skinPath.TrimRight("\\");
	int nPos = skinPath.ReverseFind('\\');
	skinPath = skinPath.Mid(nPos + 1);
	nPos = oldPath.ReverseFind('\\');
	CString oldSkin = oldPath.Mid(nPos + 1);

	bIsChangeDisplayModeDlgVisuabl=FALSE;

	pUserConfig->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX, string(skinPath), true);

	AfxGetUIManager()->UIGetChangeColorMgr()->SetSelectedColor(0);

	CString strCurPath = m_pSkin->m_strSkinPath;
	if(strPath == "")
		strPath = m_pSkin->m_strSkinPath;

	m_pSkin->SetSkinPath(strPath);
	m_pSkin->LoadSkinFromPath(strPath);

	//保存设置
	SavePanes();
	//TRACE("ChangeSkin1: %d ms\n", ::GetTickCount() - count);

	//皮肤发生改变，将原有皮肤的设置应用到新的皮肤中
	CString p = m_xml.GetFilePath();

	m_xml.Close();

	string strMySkinConfPath;
	GlobalFunc::GetSkinConfPath( strMySkinConfPath );
	SetXmlPath( strMySkinConfPath.c_str() );

	LoadColors();
	LoadChangeColorData();

	AfxGetUIManager()->UIGetSkinMgr()->ChangeSkinColor();

	//将更换前的窗口大小设置保存到更换后的皮肤配置文件中
	SavePanes();

	m_mapPaneMulti.RemoveAll();

	CString strWndName, strControlName;
	POSITION pos;
	HWND key;
	ETSLayoutMgr * pLayout;

	pos = this->m_mapBkLayoutMgr.GetStartPosition();
	while (pos != NULL)
	{
		m_mapBkLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );
		if(key == ::AfxGetMainWindow()->m_hWnd)
			continue;
		strWndName = pLayout->m_strWndName;
		strControlName = pLayout->m_strControlName;
		delete pLayout;
		int ret = m_mapBkLayoutMgr.RemoveKey(key);
		pLayout = this->GetBkLayoutMgr(key);
		CreateBmpPane(CWnd::FromHandle(key), strWndName, "");
	}
	//TRACE("ChangeSkin3: %d ms\n", ::GetTickCount() - count);
	pos = m_mapLayoutMgr.GetStartPosition();
	while (pos != NULL)
	{
		m_mapLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );
		if(key == ::AfxGetMainWindow()->m_hWnd)
			continue;

		if(!::IsWindow(key))
		{
			delete pLayout;
			m_mapLayoutMgr.RemoveKey(key);;
			continue;
		}
		strWndName = pLayout->m_strWndName;
		strControlName = pLayout->m_strControlName;
		delete pLayout;
		int ret = m_mapLayoutMgr.RemoveKey(key);

		CreateControlPane(CWnd::FromHandle(key), strWndName, "");
	}
	//TRACE("ChangeSkin4: %d ms\n", ::GetTickCount() - count);

	CreateWndPane(AfxGetMainWindow(), "playbox", false);
	CreateBmpPane(AfxGetMainWindow(), "mainBk", "11");

	::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, 120, true);
	::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, 0, true);
	cur.Restore();

	if(strCurPath.Find(strPath) < 0)
	{
		m_bChangedSkin = TRUE;
	}

	this->NotifySkinChanged(oldPath, strPath);
	
	if( strSub == "" )
	{
		AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject = "";
	}

	string strSkinPath	= m_pSkin->GetSkinPath();
	AfxGetUserConfig()->SetConfigStringValue( CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB_CURRSUB,"",true);

	if( strSub == "" )
	{
		::AfxGetMainWindow()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);	
	}
	else
	{
		AfxGetUIManager()->UIGetChangeColorMgr()->NotifyChangeSubject( strSub );		
	}
	return true;
}

void CPanesMgr::OnchangeColor( COLORREF acolCol, UINT anLightValue,bool bChangeTop )
{
	NotifyColorChanging();
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3");
	LoadColors();
	m_pSkin->LoadSkinFromPath(m_pSkin->m_strSkinPath);
	LoadChangeColorData();
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.1");

	string str = AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject;
	if( str.length()>0 )
		m_pSkin->SubjectToSkin(str);

	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.2");
	//改变颜色
	CString strWndName, strControlName;

	POSITION pos;
	HWND key;
	ETSLayoutMgr * pLayout;

	pos = m_mapLayoutMgr.GetStartPosition();
	while (pos != NULL)
	{
		m_mapLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );

		try
		{
			if( pLayout == NULL || pLayout->m_pWnd == NULL || !::IsWindow(pLayout->m_pWnd->m_hWnd))
			{
				delete pLayout;
				m_mapLayoutMgr.RemoveKey(key);
				m_mapBkLayoutMgr.RemoveKey(key);
				continue;
			}
		}
		catch(...)
		{
			m_mapLayoutMgr.RemoveKey(key);
			m_mapBkLayoutMgr.RemoveKey(key);
			continue;
		}

		strWndName = pLayout->m_strWndName;
		strControlName = pLayout->m_strControlName;

		ChangeWndColor(CWnd::FromHandle(key), strWndName, strControlName);
	}
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.4");

	//改变背景色
	pos = m_mapBkLayoutMgr.GetStartPosition();
	while (pos != NULL)
	{
		m_mapBkLayoutMgr.GetNextAssoc( pos, key, (ETSLayoutMgr*)pLayout );

		if(pLayout == NULL)
			continue;

		COLORREF color = pLayout->getOriBackGroundColor();

		::AfxGetUIManager()->UIGetChangeColorMgr()->ChangeColor(color);

		pLayout->setBackgroundColor(color);
	}

	AfxGetUIManager()->UIGetChangeColorMgr()->BeforeChangeColor();
	AfxGetUIManager()->UIGetSkinMgr()->ChangeSkinColor(bChangeTop);
	AfxGetUIManager()->UIGetChangeColorMgr()->AfterChangeColor();

	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.5");
	::AfxGetMainWindow()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

	::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, anLightValue, true);
	::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, acolCol, true);
 
	double fHue, fLight,fSat;
	gBitMapOperate.RGBtoHSL(acolCol, &fHue, &fSat, &fLight);
	CString value;
	value.Format("%d,%d,%d", (int)(fHue*240), (int)(fSat*100), (int)(fLight*100));


	AfxGetUserConfig()->SetConfigStringValue( CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB_COLOR,value.GetBuffer(),true);

	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.6");
	this->NotifyColorChanged();
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","3.7");
}

void CPanesMgr::OnChangeSubject(CString strSubName)
{	
	string strSkinPath = m_pSkin->GetSkinPath();
	string strBmpFileName = strSkinPath + "\\SbujectSkin\\" + string(strSubName) + "\\TopPanelBKLeft.bmp";

	AfxGetUserConfig()->SetConfigStringValue(CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB_CURRSUB,strSubName.GetBuffer(),true);	
	OneSubject* pSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
	if(pSub != NULL)
	{
		CString strmsg;
		strmsg.Format("NAME:%s|ID:%d",pSub->strShowName.c_str(), pSub->nID);
		LogRealMsg( "ChangeSub",string(strmsg));
	}
	
	CWnd *pWnd  = this->GetWnd("toppanel");
	CDibBitmap* pLeft = m_pSkin->GetDibBmp("TopPanelBKLeft");
	CDibBitmap* pMid = m_pSkin->GetDibBmp("TopPanelBKMid");
	CDibBitmap* pRight = m_pSkin->GetDibBmp("TopPanelBKRight");

	CString strValue;

	if(!YL_FileInfo::IsValid(strBmpFileName))
	{
		SetBkAttrib(pWnd, "TopPanelBKMid", "center_strech", "1", false);
		SetBkAttrib(pWnd, "TopPanelBKMid", "x_start", "0", false);
		SetBkAttrib(pWnd, "TopPanelBKMid", "x_end", "0", false);
		SetBkAttrib(pWnd, "TopPanelBKRight", "x_start", "0", true);	
	}
	else
	{
		strValue.Format("%d",pLeft->GetWidth());
		SetBkAttrib(pWnd, "TopPanelBKMid", "x_start", strValue, false);
		strValue.Format("%d",pRight->GetWidth());
		SetBkAttrib(pWnd, "TopPanelBKMid", "x_end", strValue, false);
		SetBkAttrib(pWnd, "TopPanelBKMid", "center_strech", "0", false);
		string strsubconf = strSkinPath + "\\SbujectSkin\\" + string(strSubName) + "\\conf.ini";

		char szSkinName[10];
		GetPrivateProfileString("CONF","tile","",szSkinName,255,strsubconf.c_str());
		int itile = atoi(szSkinName);
		if(itile == 1)	
			SetBkAttrib(pWnd, "TopPanelBKMid", "tile", "1", false);
		else
			SetBkAttrib(pWnd, "TopPanelBKMid", "tile", "0", false);
		SetBkAttrib(pWnd, "TopPanelBKRight", "x_start", strValue, true);	

	}
}

BOOL CPanesMgr::IsSplitterOpen(CString name)
{
	CWnd *pWnd = this->GetWnd(name);

	if(pWnd == NULL)
		return false;
	if(pWnd->IsKindOf(RUNTIME_CLASS(ETSSplitter)))
	{
		return !((ETSSplitter*) pWnd)->IsClosed();
	}
	return false;
}

void CPanesMgr::SetSplitterStatus(CString name, BOOL bClose)
{
	CWnd *pWnd = this->GetWnd(name);

	if(pWnd->IsKindOf(RUNTIME_CLASS(ETSSplitter)))
	{
		if(bClose != (( (ETSSplitter*) pWnd)->IsClosed() == true)?TRUE:FALSE )
			((ETSSplitter*) pWnd)->CloseSplitter();
	}
}

void CPanesMgr::NotifyLayerChanged(CString paneName, CString oldLayerName, CString newLayerName)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_LayerChanged(string(paneName), string(oldLayerName),string(newLayerName));
	}
}

void CPanesMgr::NotifyColorChanging()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_ColorChanging();
	}
}

void CPanesMgr::NotifyColorChanged()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_ColorChanged();
	}
}

void CPanesMgr::NotifySkinChanged(CString oldPath, CString newPath)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_SkinChanged(string(oldPath), string(newPath));
	}
}

void CPanesMgr::NotifyUpdateLayout(HWND hwnd)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_UpdateLayout(hwnd);
	}
}

void CPanesMgr::IPanelChangeOb_OutFullScreen()
{
	if(m_bWaitOutFullScreen)
	{
		OnChangeSkinTemplate(m_strWaitSkin,"");
		m_bWaitOutFullScreen = FALSE;
		m_strWaitSkin = "";
	}
}
void CPanesMgr::NotifyLayoutInitFinished()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_InitFinished();
	}
}

void CPanesMgr::NotifhyLayoutInitBegin()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_LAYOUTMGR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ILayoutChangeObserver* pOb = dynamic_cast<ILayoutChangeObserver*>(*itOb);
		pOb->ILayoutChangeOb_InitBegin();
	}
}

Displaymode	CPanesMgr::GetDisplayMode()
{
	return m_modeDisplay;
}

CString	CPanesMgr::GetWndPanelMode(CWnd *pWnd)
{
	return GetWndLayoutMgr(pWnd->m_hWnd)->m_strControlName;
}

COLORREF CPanesMgr::GetSkinColor()
{
	return m_skinColor;
}