// dllmain.h : 模块类的声明。
#ifndef __BHO_DEMO_H
#define __BHO_DEMO_H

class CBHO_DemoModule : public CAtlDllModuleT< CBHO_DemoModule >
{
public :
	DECLARE_LIBID(LIBID_BHO_DemoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BHO_DEMO, "{2DA36825-25E1-4DEE-B137-55FB42BB9998}")
	
};
//HINSTANCE g_hInstance ;

extern class CBHO_DemoModule _AtlModule;
#endif