#include "Fpch.h"
#include "PointLightComponent.h"

RTTI_IMPLEMENT_TYPE( forge::PointLightFragment,
	RTTI_REGISTER_PROPERTY( m_color );
	RTTI_REGISTER_PROPERTY( m_power );
	);
RTTI_IMPLEMENT_TYPE( forge::PointLightComponent );