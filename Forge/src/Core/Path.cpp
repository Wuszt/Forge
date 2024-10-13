#include "fpch.h"
#include "Path.h"

RTTI_IMPLEMENT_TYPE( forge::Path,
	RTTI_REGISTER_PROPERTY( m_path );
	RTTI_REGISTER_METHOD( OnPostDeserialize );
);