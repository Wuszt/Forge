#include "fpch.h"
#include "Path.h"

RTTI_IMPLEMENT_TYPE( forge::Path,
	RTTI_REGISTER_PROPERTY( m_path, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_METHOD( PostDeserialize );
);