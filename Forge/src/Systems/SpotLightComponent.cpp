#include "Fpch.h"
#include "SpotLightComponent.h"

RTTI_IMPLEMENT_TYPE( forge::SpotLightFragment,
	RTTI_REGISTER_PROPERTY( m_color, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_PROPERTY( m_innerAngle, 
		RTTI_ADD_METADATA( "RangeMin", 0 );
		RTTI_ADD_METADATA( "RangeMax", FORGE_PI_HALF );
		RTTI_ADD_METADATA( "IsRadiansAngle" );
		RTTI_ADD_METADATA( "Editable" );
		);
	RTTI_REGISTER_PROPERTY( m_outerAngle,
		RTTI_ADD_METADATA( "RangeMin", 0 );
		RTTI_ADD_METADATA( "RangeMax", FORGE_PI_HALF );
		RTTI_ADD_METADATA( "IsRadiansAngle" );
		RTTI_ADD_METADATA( "Editable" );
	);
	RTTI_REGISTER_PROPERTY( m_power, RTTI_ADD_METADATA( "Editable" ) );
	);

RTTI_IMPLEMENT_TYPE( forge::SpotLightComponent )