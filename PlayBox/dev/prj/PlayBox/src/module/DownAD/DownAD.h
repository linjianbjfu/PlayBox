#pragma once
#include "src\datainterface\IConfUpdateObserver.h"
#include "..\..\datainterface\IAdData.h"


class CDownAD : public IConfUpdateObserver, public IAdData
{
public:
	static CDownAD*	m_pDownAD;
	static CDownAD*	GetInstance();

	CDownAD(void);
	~CDownAD(void);
	void	DataAppStart();
	void	DataAppExit();

	HANDLE			m_hThreadDown;
	bool			m_bGetAdData;
	void			DownZipFinish(void* pVoid,unsigned int unContentLen);
	void			NotifyZipStart();
	void			NotifyZipSuc();
	void			NotifyZipFailed();
	void			ZipResReady(BOOL bDownSuccess);

	virtual bool	GetLoadingPicAd( AD_STRUCT& ad );
private:
	vector<AD_STRUCT>	m_vecAD;
	int					m_iCurAd;
	void			ReloadAd();
};
