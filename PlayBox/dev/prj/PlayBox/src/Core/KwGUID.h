#ifndef _KW_GUID_H
#define _KW_GUID_H

#include "tapi.h"

class KwGUID
{

public:

	KwGUID();
	KwGUID(const KwGUID&);
	KwGUID(const GUID&);
	~KwGUID();

public:

	bool	operator<(const KwGUID&) const;
	void	SetGUID( const GUID& );
	void	GetGUID( GUID& ) const; 
	static bool	IsEqual(const KwGUID&,const KwGUID&);
private:
	GUID	m_GUID;
};

#endif