#if !defined(ETS_LAYOUTMGR_EX_INCLUDED_)
#define ETS_LAYOUTMGR_EX_INCLUDED_

#include "ETSLayout.h"

/**
 * Forward class declarations
 */
class ETSSplitter;

class  ETSLayoutExMgr : public ETSLayoutMgr
{
public:
	ETSLayoutExMgr(CWnd* pWnd)	{ m_pWnd = pWnd; m_sizeRootBorders = CSize(5,5); };
	virtual ~ETSLayoutExMgr();


	/**
	 * This class encapsulates a Subpane (and indeed the root Pane too)
	 * it is a container of PaneBase* which it will recursivly resize
	 */

}
#endif 
