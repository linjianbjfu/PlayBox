#include "stdafx.h"
#include "GDataGetter.h"

IPanelChange*	  GDataGetter::s_pPanelChangeData = NULL;


IPanelChange*	GDataGetter::PanelChangeData()
{
	if( s_pPanelChangeData )
	{
		return s_pPanelChangeData;
	}
	s_pPanelChangeData = (IPanelChange*)AfxGetDataManager2()->GetDataObject( KWID_DATA_PANELCHANGE );
	return s_pPanelChangeData;
}