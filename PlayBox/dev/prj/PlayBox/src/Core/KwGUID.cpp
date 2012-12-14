#include "KwGUID.h"


KwGUID::KwGUID()
{
	memset( &m_GUID,0,sizeof(GUID) );
}
KwGUID::KwGUID( const KwGUID& kwguid )
{	
	GUID guid;
	kwguid.GetGUID( guid );
	memcpy( &m_GUID,&guid,sizeof(GUID) );
}

KwGUID::KwGUID( const GUID& guid )
{
	memcpy( &m_GUID,&guid,sizeof(GUID) );
}

KwGUID::~KwGUID()
{

}

void KwGUID::GetGUID( GUID& guid) const
{
	memcpy(&guid,&m_GUID,sizeof(GUID));
	return ;
}

void KwGUID::SetGUID( const GUID& guid )
{
	memcpy( &m_GUID,&guid,sizeof(GUID) );
}

bool KwGUID::operator<( const KwGUID& kwGUID ) const
{	
	GUID guid;
	kwGUID.GetGUID( guid );
	return m_GUID.Data1 < guid.Data1;
}


bool KwGUID::IsEqual( const KwGUID& kwGUID ,const KwGUID& kwGUID2)
{	
	GUID guid;
	kwGUID.GetGUID( guid );

	GUID guid2;
	kwGUID2.GetGUID( guid2 );

	return guid.Data1 == guid2.Data1 && guid.Data2 == guid2.Data2 && guid.Data3 == guid2.Data3 && memcmp(guid.Data4,guid2.Data4,8*sizeof(unsigned char)) == 0;
}
