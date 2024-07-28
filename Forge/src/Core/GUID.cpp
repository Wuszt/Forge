#include "fpch.h"
#include "GUID.h"
#include <combaseapi.h>

RTTI_IMPLEMENT_TYPE( forge::GUID,
	RTTI_REGISTER_PROPERTY( m_A );
	RTTI_REGISTER_PROPERTY( m_B );
	RTTI_REGISTER_PROPERTY( m_C );
	RTTI_REGISTER_PROPERTY( m_D );
);

forge::GUID forge::GUID::Generate()
{
	GUID result;
	CoCreateGuid( reinterpret_cast< ::GUID* >( &result ) );
	return result;
}

Uint64 forge::GUID::GetHash() const
{
	return (static_cast< Int64 >( m_A ) << 32 | static_cast< Int64 >( m_B )) ^ (static_cast< Int64 >( m_C ) << 32 | static_cast< Int64 >( m_D ));
}
