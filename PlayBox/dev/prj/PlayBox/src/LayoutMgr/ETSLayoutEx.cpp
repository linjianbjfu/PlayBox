#include "stdafx.h"

#include "ETSLayout.h"
#include "skinmgr.h"
#include "ETSSplitter.h"
//
//创建PaneSpl对象
ETSLayoutMgr::CPane ETSLayoutMgr::paneSpl(ETSSplitter * pSpl,  layOrientation orientation, ETSLayoutMgr::layResizeMode modeResize /*=GREEDY*/, 
									   int nClosedPos /*=nDefaultBorder*/, int nClosedOri /*=0*/, 
									   int nSize /*=0*/,
									   int nOpenSize,
									   int nHideCloseBtn,
									   int nSlim
									   )
{
	ETSLayoutMgr::PaneSplitter* pPane = new PaneSplitter ( pSpl, this, orientation);

	//pPane->m_sizeSecondary = (isClosed)? nClosedPos:nSplOpenSize;

	pPane->m_sizeSecondary = nSize;
	//if(orientation == VERTICAL)
	pPane->m_modeResize    = modeResize;
//	pPane->SetClosed(nSize == nClosedPos);

	if(orientation == VERTICAL)
		pSpl->m_bHorizSplitter = true;
	else
		pSpl->m_bHorizSplitter   = false;

	pSpl->m_bHideCloseBtn = (nHideCloseBtn == 1);
	//pSpl->SetSlim(nSlim ==1);

	pPane->m_nClosedOri = nClosedOri;
	pPane->m_nSplClosedPos = nClosedPos;
	pPane->m_nSplOpendPos = nOpenSize;
	
	pSpl->m_pane = (ETSLayoutMgr::PaneSplitter *)pPane;
	pPane->m_nDeflate = -1;
	return CPane(pPane);
}

ETSLayoutMgr::PaneSplitter::PaneSplitter(ETSSplitter * pSpl, ETSLayoutMgr * pMgr,  layOrientation orientation )
: ETSLayoutMgr::Pane(pMgr, orientation, 0, 0) 
{
	ASSERT(pSpl);
	m_pSpl = pSpl;
	m_nDeflate = true;
}

void ETSLayoutMgr::PaneSplitter::showPane(bool bShow) 
{
	this->m_bShow = bShow;
	if(bShow)
		m_pSpl->ShowWindow(SW_SHOW);
	else
		m_pSpl->ShowWindow(SW_HIDE);

	ETSLayoutMgr::Pane::showPane(bShow);
}

int ETSLayoutMgr::PaneSplitter::getMinConstrainHorz()
{
	CRect rc = getPaneRect();

	if(m_Orientation == VERTICAL && rc.Width() <= 0)
	{
		return 0;
	}
	else
		return Pane::getMinConstrainHorz();
}

int ETSLayoutMgr::PaneSplitter::getMinConstrainVert()
{
	CRect rc = this->getPaneRect();

	if(m_Orientation == HORIZONTAL && rc.Height() <= 0)
	{
		return 0;
	}
	else
		return Pane::getMinConstrainVert();
}

//modifier here
bool ETSLayoutMgr::PaneSplitter::resizeTo(CRect& rcNewArea,bool bRedraw,bool bReSizeAlways)
{
	bool ret;

	int modeSize1 , modeSize2 ;
	modeSize1 = modeSize2 = -1;
	if(rcNewArea.left > rcNewArea.right)
		return true;

	if( (rcNewArea.Width() < m_rcRect.Width()	|| rcNewArea.Height() < m_rcRect.Height() || m_nDeflate == -1)
		&& !this->IsClosed()
		)
		m_nDeflate = true;
	else
		m_nDeflate = false;

	CPaneBase pItem3 = this->m_paneItems[0];
	ret = Pane::resizeTo(rcNewArea, bRedraw, bReSizeAlways);
//	this->m_pSpl->ShowWindow(SW_SHOW);

	CPaneBase pItem1 = this->m_paneItems[0];
	CPaneBase pItem2 = this->m_paneItems[2];
	if( m_nDeflate &&
		(pItem1->getPaneRect().Width() == pItem1->getMinConstrainHorz() 
		|| pItem1->getPaneRect().Height() == pItem1->getMinConstrainVert())
		)
	{
		modeSize2 = pItem2->modeResize();
		pItem2->m_modeResize = GREEDY;

	}
	if( m_nDeflate &&
		(pItem2->getPaneRect().Width() == pItem2->getMinConstrainHorz() 
		|| pItem2->getPaneRect().Height() == pItem2->getMinConstrainVert())
		)
	{
		modeSize1 = pItem1->modeResize();
		pItem1->m_modeResize = GREEDY;
	}


	ret = Pane::resizeTo(rcNewArea, bRedraw, bReSizeAlways);
	m_rcRect = rcNewArea;
	m_pSpl->ShowWindow(SW_SHOW);

	if(modeSize1 != -1)
		pItem1->m_modeResize = modeSize1;
	if(modeSize2 != -1)
		pItem2->m_modeResize = modeSize2;

	return ret;
}


void ETSLayoutMgr::PaneSplitter::setOpenConstrain()
{
	CPaneBase pItem;
	if(this->m_nClosedOri == 0)
		pItem = m_paneItems[0];
	else
		pItem = m_paneItems[2];
	CRect rc = pItem->getPaneRect();
	
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
	{
		this->m_nSplOpendPos = rc.Width();
		TRACE("fff%d\n", m_nSplOpendPos);
	}
	else
	{
		this->m_nSplOpendPos = rc.Height();
	}

	//m_sizeSecondary = m_nSplOpendPos;
}

int ETSLayoutMgr::PaneSplitter::getOpenConstrain(int offset)
{
	CPaneBase pItem;
	int orgValue = 0;
	int maxConstrain = this->m_nSplOpendPos;

	if(offset < 0)
		pItem = m_paneItems[2];
	else
		pItem = m_paneItems[0];

	CRect rc = pItem->getPaneRect();

	if(m_Orientation == ETSLayoutMgr::HORIZONTAL)
	{
		orgValue = rc.Width();
		//minConstrain = getCloseConstrainHorz();
	}
	else
	{
		orgValue = rc.Height();
		//minConstrain = pItem->getMinConstrainVert();
	}

	if(offset < 0)
	{
		if(orgValue - offset > maxConstrain)
			offset =  orgValue - maxConstrain;
	}
	else
	{
		if(orgValue + offset > maxConstrain)
			offset = maxConstrain - orgValue;
	}

	return offset;
}

int ETSLayoutMgr::PaneSplitter::getCloseConstrain(int offset)
{
	CPaneBase pItem;
	int orgValue = 0;
	int minConstrain = this->m_nSplClosedPos;

	if(offset < 0)
		pItem = m_paneItems[0];
	else
		pItem = m_paneItems[2];

	CRect rc = pItem->getPaneRect();

	if(m_Orientation == ETSLayoutMgr::HORIZONTAL)
	{
		orgValue = rc.Width();
		//minConstrain = getCloseConstrainHorz();
	}
	else
	{
		orgValue = rc.Height();
		//minConstrain = pItem->getMinConstrainVert();
	}

	if(offset < 0)
	{
		if(orgValue + offset < minConstrain)
			offset =  minConstrain - orgValue;
	}
	else
	{
		if(orgValue - offset < minConstrain)
			offset = orgValue - minConstrain;
	}

	return offset;
}

int  ETSLayoutMgr::PaneSplitter::getOffsetConstrain(int offset)
{
	CPaneBase pItem;
	int orgValue = 0;
	int minConstrain = 0;

	if(offset < 0)
		pItem = m_paneItems[0];
	else
		pItem = m_paneItems[2];

	CRect rc = pItem->getPaneRect();

	if(m_Orientation == ETSLayoutMgr::HORIZONTAL)
	{
		orgValue = rc.Width();
		minConstrain = pItem->getMinConstrainHorz();
	}
	else
	{
		orgValue = rc.Height();
		minConstrain = pItem->getMinConstrainVert();
	}

	if(offset < 0)
	{
		if(orgValue + offset < minConstrain)
			offset =  minConstrain - orgValue;
	}
	else
	{
		if(orgValue - offset < minConstrain)
			offset = orgValue - minConstrain;
	}

	return offset;
}
void ETSLayoutMgr::PaneSplitter::SetSplPosition(int pos)
{
	ASSERT(m_paneItems.GetSize() == 3);

	CPaneBase pItem = m_paneItems[0];
	CRect rc = pItem->getPaneRect();

	//Resize First Pane
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc = CRect(rc.left, rc.top, rc.left + pos, rc.bottom);
	else
		rc = CRect(rc.left, rc.top, rc.right, rc.top + pos);//rc.InflateRect(0, 0, 0, offset);
	ForceResizeTo(pItem, rc);

	//Resize Second Pane
	pItem = m_paneItems[1];
	CRect rcSpl = pItem->getPaneRect();
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc = CRect(rc.right, rcSpl.top, rc.right + rcSpl.Width(), rcSpl.bottom);//rc.InflateRect(-offset,0, 0,  0);
	else
		rc = CRect(rcSpl.left, rc.bottom, rcSpl.right, rc.bottom + rcSpl.Width());//.rc.InflateRect(0, -offset, 0,0);
	ForceResizeTo(pItem, rc);

	//Resize Splitter
	pItem = m_paneItems[2];
	//CRect rcSecond = pItem->getPaneRect();
	CRect rcAll = this->getPaneRect();
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc = CRect(rc.right, rc.top, rc.right + rcAll.Width() - rcSpl.Width() - pos, rc.bottom);//rc.InflateRect(-offset, 0, offset, 0);
	else
		rc = CRect(rc.left, rc.bottom, rc.right, rc.bottom + rcAll.Height() - rcSpl.Height() - pos); //rc.InflateRect(0, -offset,0, offset);
	ForceResizeTo(pItem, rc);

	if(!IsClosed())
		setOpenConstrain();

}

int ETSLayoutMgr::PaneSplitter::GetSplPosition()
{
	ASSERT(m_paneItems.GetSize() == 3);

	CPaneBase pItem = m_paneItems[0];
	CRect rc = pItem->getPaneRect();

	pItem = m_paneItems[1];
	CRect rc2 = pItem->getPaneRect();

	if(rc.Width() == 0 && rc2.Width() == 0)
		return -1;

	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		return rc.Width();
	else
		return rc.Height();
}


int	 ETSLayoutMgr::PaneSplitter::GetOuterPaneMinPosition()
{
	CPaneBase pItem = m_paneItems[2];
	int min =  pItem->getMinConstrainHorz();

	return min;
}

bool ETSLayoutMgr::PaneSplitter::ForceResizeOuterPane(int offset, bool bRedraw)
{
	//Resize Splitter
	CPaneBase pItem = m_paneItems[2];
	CRect rc = pItem->getPaneRect();

	if(rc.Width() - offset < pItem->getMinConstrainHorz())
		offset = rc.Width() - pItem->getMinConstrainHorz();

	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc.InflateRect(0, 0, -offset, 0);
	else
		;
	ForceResizeTo(pItem, rc, bRedraw);

	this->m_pSpl->ShowWindow(SW_SHOW);
	return true;
}

bool ETSLayoutMgr::PaneSplitter::resizeTo(int offset,bool bRedraw,bool bReSizeAlways)
{
	ASSERT(m_paneItems.GetSize() == 3);

	CPaneBase pItem = m_paneItems[0];
	CRect rc = pItem->getPaneRect();

	//Resize First Pane
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc.InflateRect(0,0, offset, 0);
	else
		rc.InflateRect(0, 0, 0, offset);
	ForceResizeTo(pItem, rc, bRedraw);

	//Resize Second Pane
	pItem = m_paneItems[2];
	rc = pItem->getPaneRect();
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc.InflateRect(-offset,0, 0,  0);
	else
		rc.InflateRect(0, -offset, 0,0);
	ForceResizeTo(pItem, rc, bRedraw);

	//Resize Splitter
	pItem = m_paneItems[1];
	rc = pItem->getPaneRect();
	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		rc.InflateRect(-offset, 0, offset, 0);
	else
		rc.InflateRect(0, -offset,0, offset);
	ForceResizeTo(pItem, rc, bRedraw);

	//this->m_nSplClosedPos = this->GetSplPosition();
	//this->m_pSpl->ShowWindow(SW_SHOW);
	return true;
}
int  ETSLayoutMgr::PaneSplitter::getSecondSize()
{
	if(m_paneItems.GetSize() != 3)
		return 0;
	CPaneBase pItem = m_paneItems[2];
	CRect rc = pItem->getPaneRect();
	int value;

	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
		value= rc.Height();
	else
		value = rc.Width();
	return value;
}

int  ETSLayoutMgr::PaneSplitter::IsClosed()
{
	CPaneBase pItem;
	int width;

	if(this->m_nClosedOri == 0)
		pItem = m_paneItems[0];
	else
		pItem = m_paneItems[2];

	CRect rc = pItem->getPaneRect();

	if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
	{
		width = rc.Width();
	}
	else
	{
		width = pItem->getConstrainVert(0);
		width = rc.Height();
	}

	if(width == this->m_nSplClosedPos)
		return true;
	return false;
}

bool ETSLayoutMgr::PaneSplitter::addPane( CPane pSubpane, ETSLayoutMgr::layResizeMode modeResize, int sizeSecondary /* = 0 */) 
{
	if(this->m_paneItems.GetSize() == 1)
	{
		if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
			Pane::addPane(this->m_pMgr->item((CWnd*)this->m_pSpl, ABSOLUTE_HORZ , m_pSpl->GetSplSize(), 0,m_pSpl->GetSplSize()));
		else
			Pane::addPane(this->m_pMgr->item((CWnd*)this->m_pSpl, ABSOLUTE_VERT , 0, m_pSpl->GetSplSize(),0,m_pSpl->GetSplSize()));
	}
	Pane::addPane(pSubpane, modeResize, sizeSecondary);	

	return true;
}

bool ETSLayoutMgr::PaneSplitter::addPane( CPaneBase pItem ) 
{
	if(this->m_paneItems.GetSize() == 1)
	{
		if(this->m_Orientation == ETSLayoutMgr::HORIZONTAL)
			Pane::addPane(this->m_pMgr->item((CWnd*)this->m_pSpl, ABSOLUTE_HORZ , m_pSpl->GetSplSize(), 0,m_pSpl->GetSplSize()));
		else
			Pane::addPane(this->m_pMgr->item((CWnd*)this->m_pSpl, ABSOLUTE_VERT , 0,m_pSpl->GetSplSize(),0,m_pSpl->GetSplSize()));
	}
	Pane::addPane(pItem);

	return true;
}



void ETSLayoutMgr::PaneSplitter::ForceResizeTo(CPaneBase &pPane, CRect &rc,bool bRedraw)
{
	DWORD modeSize = pPane->modeResize();
	pPane->m_modeResize = GREEDY;
	pPane->resizeTo(rc, bRedraw );
	pPane->m_modeResize = modeSize;
};


/*****************************************************************************************************************************************************/

ETSLayoutMgr::PaneMulti* ETSLayoutMgr::CPaneMulti::operator->() const
{
	ASSERT(m_pPaneHolder);

	if(!m_pPaneHolder)
		return 0;

	return reinterpret_cast<PaneMulti*>(m_pPaneHolder->m_pPane);
}

ETSLayoutMgr::CPaneMulti& ETSLayoutMgr::CPaneMulti::operator<< ( const ETSLayoutMgr::CPane pPane )
{
	GetPane()->addPane( pPane, (ETSLayoutMgr::layResizeMode)pPane->m_modeResize, pPane->m_sizeSecondary);
	return (*this);
}

ETSLayoutMgr::CPaneMulti& ETSLayoutMgr::CPaneMulti::operator<< ( const ETSLayoutMgr::CPaneBase pItem )
{
	GetPane()->addPane( pItem );
	return (*this);
}

ETSLayoutMgr::CPaneMulti::CPaneMulti( PaneMulti* pPane ) : ETSLayoutMgr::CPane(pPane)
{
}

ETSLayoutMgr::CPaneMulti::CPaneMulti() : ETSLayoutMgr::CPane()
{
}

ETSLayoutMgr::CPaneMulti ETSLayoutMgr::paneMulti( layOrientation orientation, ETSLayoutMgr::layResizeMode modeResize /*=GREEDY*/, 
									   int sizeBorder /*=nDefaultBorder*/, int sizeExtraBorder /*=0*/, 
									   int sizeSecondary /*=0*/)
{
	PaneMulti* pPane = new PaneMulti ( this, orientation, sizeBorder, sizeExtraBorder );
	pPane->m_sizeSecondary = sizeSecondary;
	pPane->m_modeResize    = modeResize;

	return CPaneMulti(pPane); 
}


void ETSLayoutMgr::PaneMulti::showPane(bool bShow) 
{
	m_bShow = bShow;

	if(m_paneCurrent.IsValid())
	{
		m_paneCurrent.GetPaneBase()->showPane(bShow); 
		if(m_paneCurrent.GetPaneBase()->GetName() == "SearchResult" && bShow)
			bShow = true;
	}
	else if(bShow == false)
	{
		POSITION pos = m_mapLayers.GetStartPosition();
		CString key;
		CPaneBase	pane;
		while (pos != NULL)
		{
			m_mapLayers.GetNextAssoc( pos, key, pane );
			pane.GetPaneBase()->showPane(false);
		}
	}
}

ETSLayoutMgr::CPaneBase ETSLayoutMgr::PaneMulti::GetSubPane(CString name)
{
	POSITION pos = m_mapLayers.GetStartPosition();
	CString key;
	CPaneBase	pane;
	while (pos != NULL)
	{
		m_mapLayers.GetNextAssoc( pos, key, pane );
		CPaneBase pSubPane = pane->GetSubPane(name);
		if(pSubPane.IsValid())
			return pSubPane;
	}
	return NULL;
}


void ETSLayoutMgr::PaneMulti::addLayer(CString name, CPaneBase pane)
{
	pane->showPane(false);
	m_mapLayers.SetAt(name, pane);
}

CString	 ETSLayoutMgr::PaneMulti::getCurrent()
{
	return this->m_strCurrent;
}
bool ETSLayoutMgr::PaneMulti::setCurrent(CString name , bool bRedraw )
{

	CRect rect = CRect(0,0,-1,-1);
	showPane(false);

	if(m_paneCurrent.IsValid())
	{
		rect = m_paneCurrent.GetPaneBase()->getPaneRect();
	}

	if(!m_mapLayers.Lookup(name, m_paneCurrent))
	{
		//如果current没有，择默认设置第一个layer。
		POSITION pos = m_mapLayers.GetStartPosition();
		CString key;
		if (pos != NULL)
		{
			m_mapLayers.GetNextAssoc( pos, key, m_paneCurrent );
		}
		//else
		//showPane(true);
		//return false;
	}
	m_paneItems.RemoveAll();

	if((m_paneCurrent->modeResize() & ABSOLUTE_VERT)
		&& m_paneCurrent->getConstrainVert(0) != rect.Height())
	{
		rect.bottom = rect.top + m_paneCurrent->getConstrainVert(0);
		
		CWnd *pWnd = this->m_pMgr->m_pWnd;//->GetParent();
		::AfxGetUIManager()->UIGetLayoutMgr()->SetPaneSize(pWnd, GetName() ,rect);
	}
	addPane(m_paneCurrent);

	m_strCurrent = name;
	m_paneCurrent->resizeTo(rect,true, true);
	if(bRedraw)
		showPane(true);


	//if(bRedraw)
	//	::AfxGetMainWindow()->RedrawWindow();
	return true;
}

/********************************************************************************************************************************************/

ETSLayoutMgr::PanePosition* ETSLayoutMgr::CPanePosition::operator->() const
{
	ASSERT(m_pPaneHolder);

	if(!m_pPaneHolder)
		return 0;

	return reinterpret_cast<PanePosition*>(m_pPaneHolder->m_pPane);
}

ETSLayoutMgr::CPanePosition& ETSLayoutMgr::CPanePosition::operator<< ( const ETSLayoutMgr::CPane pPane )
{
	GetPane()->addPane( pPane, (ETSLayoutMgr::layResizeMode)pPane->m_modeResize, pPane->m_sizeSecondary);
	return (*this);
}

ETSLayoutMgr::CPanePosition& ETSLayoutMgr::CPanePosition::operator<< ( const ETSLayoutMgr::CPaneBase pItem )
{
	GetPane()->addPane( pItem );
	return (*this);
}

ETSLayoutMgr::CPanePosition::CPanePosition( PanePosition* pPane ) : ETSLayoutMgr::CPane(pPane)
{
}

ETSLayoutMgr::CPanePosition::CPanePosition() : ETSLayoutMgr::CPane()
{
}

ETSLayoutMgr::CPanePosition ETSLayoutMgr::panePosition( layOrientation orientation, ETSLayoutMgr::layResizeMode modeResize /*=GREEDY*/, 
									   int sizeBorder /*=nDefaultBorder*/, int sizeExtraBorder /*=0*/, 
									   int sizeSecondary /*=0*/)
{
	PanePosition* pPane = new PanePosition ( this, orientation, sizeBorder, sizeExtraBorder );
	pPane->m_sizeSecondary = sizeSecondary;
	pPane->m_modeResize    = modeResize;

	return CPanePosition(pPane);
}



ETSLayoutMgr::PanePosition::PanePosition( ETSLayoutMgr* pMgr, layOrientation orientation, int sizeBorder /*= nDefaultBorder*/, int sizeExtraBorder /*= 0*/ )
: ETSLayoutMgr::Pane(pMgr, orientation, sizeBorder, sizeExtraBorder) 
{
}



bool ETSLayoutMgr::PanePosition::resizeTo(CRect& rcNewArea,bool bRedraw,bool bReSizeAlways)
{
	m_rcRect = rcNewArea;
	for(int i = 0 ; i<m_paneItems.GetSize(); i++)
	{
		CPaneBase base = m_paneItems[i];
		
		int sx = atoi(base.GetPaneBase()->m_start_x);
		int sy = atoi(base.GetPaneBase()->m_start_y);
		int ex = atoi(base.GetPaneBase()->m_end_x);
		int ey = atoi(base.GetPaneBase()->m_end_y);

		int percent_start, percent_end;

		percent_start = atoi(base.GetPaneBase()->m_percent_start);
		percent_end = atoi(base.GetPaneBase()->m_percent_end);

		CString value;
		if(m_pMgr->GetValue(base.GetPaneBase()->m_start_x,value))
			sx = atoi(value);
		if(m_pMgr->GetValue(base.GetPaneBase()->m_start_y,value))
			sy = atoi(value);
		if(m_pMgr->GetValue(base.GetPaneBase()->m_end_x,value))
			ex = atoi(value);
		if(m_pMgr->GetValue(base.GetPaneBase()->m_end_y,value))
			ey = atoi(value);

		int percent_rangey = 0;
		int percent_rangex = 0;
		DWORD mode = base.GetPaneBase()->m_posMode;
		if(mode & ETSLayoutMgr::START_X_PERCENT | mode & ETSLayoutMgr::END_X_PERCENT)
		{
			percent_rangex = rcNewArea.Width();
			if(percent_start > 0)
				percent_rangex -= percent_start;
			else
				percent_rangex += percent_start;

			if(percent_end > 0)
				percent_rangex -= rcNewArea.Width() - percent_end;
			else
				percent_rangex += percent_end;
		}
		if(mode & ETSLayoutMgr::START_Y_PERCENT | mode & ETSLayoutMgr::END_Y_PERCENT)
		{
			percent_rangey = rcNewArea.Height();
			if(percent_start > 0)
				percent_rangey -= percent_start;
			else
				percent_rangey += percent_start;

			if(percent_end > 0)
				percent_rangey -= rcNewArea.Height() - percent_end;
			else
				percent_rangey += percent_end;
		}

		CRect r;
		if(mode & ETSLayoutMgr::START_X_FROM_LEFT)
			r.left = rcNewArea.left + sx;
		if(mode & ETSLayoutMgr::START_X_FROM_RIGHT)
			r.left = rcNewArea.right - sx;
		if(mode & ETSLayoutMgr::START_X_PERCENT)
		{
			if(percent_start >= 0)
				r.left = rcNewArea.left + percent_start + percent_rangex * sx / 100 ;
			else
				r.left = rcNewArea.right + percent_start + percent_rangex * sx / 100 ;
		}
		if(mode & ETSLayoutMgr::START_Y_FROM_TOP)
			r.top = rcNewArea.top + sy;
		if(mode & ETSLayoutMgr::START_Y_FROM_BOTTOM)
			r.top = rcNewArea.bottom - sy;
		if(mode & ETSLayoutMgr::START_Y_PERCENT)
		{
			if(percent_start >= 0)
				r.top = rcNewArea.top + percent_start + percent_rangey * sy / 100;
			else
				r.top = rcNewArea.bottom + percent_start + percent_rangey * sy / 100;
		}
		if(mode & ETSLayoutMgr::END_X_FROM_LEFT)
			r.right = rcNewArea.left + ex;
		if(mode & ETSLayoutMgr::END_X_FROM_RIGHT)
			r.right = rcNewArea.right - ex;
		if(mode & ETSLayoutMgr::END_X_PERCENT)
		{
			if(percent_start >= 0)
				r.left = rcNewArea.left + percent_start + percent_rangex * ex / 100 ;
			else
				r.left = rcNewArea.right + percent_start + percent_rangex * ex / 100 ;
		}
		if(mode & ETSLayoutMgr::END_Y_FROM_TOP)
			r.bottom = rcNewArea.top + ey;
		if(mode & ETSLayoutMgr::END_Y_FROM_BOTTOM)
			r.bottom = rcNewArea.bottom - ey;
		if(mode & ETSLayoutMgr::END_Y_PERCENT)
		{
			if(percent_start >= 0)
				r.top = rcNewArea.top + percent_start + rcNewArea.Height() * ey / 100;
			else
				r.top = rcNewArea.bottom + percent_start + rcNewArea.Height() * ey / 100;
		}
		if(mode & ETSLayoutMgr::X_DELTA)
		{
			if(ex == 0){
				CDibBitmap *pBmp = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp(base.GetPaneBase()->GetName(), false);
				if(pBmp != NULL)
					ex = pBmp->GetWidth();
			}
			r.right = r.left + ex;
		}
		if(mode & ETSLayoutMgr::Y_DELTA)
		{
			if(ey== 0){
				CDibBitmap *pBmp = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp(base.GetPaneBase()->GetName(), false);
				if(pBmp != NULL)
					ey = pBmp->GetHeight();
			}
			r.bottom = r.top + ey;

		}
		if(mode & ETSLayoutMgr::DRAW_CENTER_STRECH)
		{
			r = rcNewArea;
		}

		CString name = base->GetName();
		base->resizeTo(r, bRedraw);
	}
	return true;
}

/*******************************************************************************************************************/
void	ETSLayoutMgr::AddParameter(CString name, CString value)
{
	m_mapParams.SetAt(name, value);
}

BOOL	ETSLayoutMgr::GetValue(CString name, CString &value)
{
	return m_mapParams.Lookup(name, value);
}

void ETSLayoutMgr::UpdateBkLayout(CDC *pDC, BOOL bUseMemDC)
{
	CRect rc;
	
	m_pDC = pDC;
	m_pWnd->GetWindowRect(&rc);
	m_pWnd->ScreenToClient(&rc);
	if(bUseMemDC)
	{
		CMemDC memdc(pDC, &rc);
		m_pDC = &memdc;

		if(this->m_clrBackground != 0)
			m_pDC->FillSolidRect(&rc, m_clrBackground);
		Layout(rc);
	}
	else
	{
		if(this->m_clrBackground != 0)
			m_pDC->FillSolidRect(&rc, m_clrBackground);
		Layout(rc);
	}
	m_pDC = NULL;
	//HRGN hrgn = CreateRectRgn(0,0,0,0);
	//int ret = ::GetWindowRgn(m_pWnd->m_hWnd,hrgn);
	//if(ret == COMPLEXREGION)
	//	;//SelectClipRgn(pDC->m_hDC,hrgn);
	//::DeleteObject(hrgn);
//
//	NULLREGION The region is empty. 
//SIMPLEREGION The region is a single rectangle. 
//COMPLEXREGION The region is more than one rectangle. 
//ERROR 


}



bool ETSLayoutMgr::PaneMulti::isLayerExist(CString name)
{
	CPaneBase	panebase;
	if(!m_mapLayers.Lookup(name, panebase))
	{
		return false;
	}

	return true;
}
