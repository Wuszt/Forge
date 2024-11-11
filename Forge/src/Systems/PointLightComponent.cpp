#include "Fpch.h"
#include "PointLightComponent.h"

RTTI_IMPLEMENT_TYPE( forge::PointLightFragment,
	RTTI_REGISTER_PROPERTY( m_color, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_PROPERTY( m_power, RTTI_ADD_METADATA( "Editable" ) );
	);
RTTI_IMPLEMENT_TYPE( forge::PointLightComponent );