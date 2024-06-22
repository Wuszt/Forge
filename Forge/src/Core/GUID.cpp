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
