#include "stdafx.h"
#include "ETSLayout.h"



ETSLayoutMgr::PaneMulti::PaneMulti( ETSLayoutMgr* pMgr, layOrientation orientation, int sizeBorder /*= nDefaultBorder*/, int sizeExtraBorder /*= 0*/ )
: ETSLayoutMgr::Pane(pMgr, orientation, sizeBorder, sizeExtraBorder) 
{
}

int ETSLayoutMgr::PaneMulti::getConstrainHorz(int sizeParent)
{
	return m_paneCurrent->getConstrainHorz(sizeParent);
}

int ETSLayoutMgr::PaneMulti::getConstrainVert(int sizeParent)
{
	return m_paneCurrent->getConstrainVert(sizeParent);
}

int ETSLayoutMgr::PaneMulti::getMinConstrainHorz()
{
	return m_paneCurrent->getMinConstrainHorz();
}

int ETSLayoutMgr::PaneMulti::getMinConstrainVert()
{
	return m_paneCurrent->getMinConstrainVert();
}

int ETSLayoutMgr::PaneMulti::getMaxConstrainHorz()
{
	return m_paneCurrent->getMaxConstrainHorz();
}

int ETSLayoutMgr::PaneMulti::getMaxConstrainVert()
{
	return m_paneCurrent->getMaxConstrainVert();
}

bool ETSLayoutMgr::PaneMulti::resizeTo(CRect& rcNewArea,bool bRedraw,bool bReSizeAlways)
{
	this->m_rcRect = rcNewArea;
	return m_paneCurrent->resizeTo(rcNewArea,bRedraw, bReSizeAlways);
}
