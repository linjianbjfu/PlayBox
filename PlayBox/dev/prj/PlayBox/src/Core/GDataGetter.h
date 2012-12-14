#ifndef _G_DATA_GETTER_H_20081201
#define _G_DATA_GETTER_H_20081201


#include "IDataManager.h"
#include "../DataInterface/IPanelChange.h"


#define  GLOBAL_PANELCHANGEDATA		((IPanelChange*)GDataGetter::PanelChangeData())

class GDataGetter
{
public:
	
	static IPanelChange*			PanelChangeData();

private:

	static  IPanelChange*		s_pPanelChangeData;	
};

#endif