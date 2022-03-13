#pragma once

namespace renderer
{
	struct PointLightData
	{
		PointLightData() = default;
		~PointLightData() = default;

		PointLightData( const Vector3& position, Float power, const Vector3& color )
			: Position( position )
			, Power( power )
			, Color( color )
		{}

		Vector3 Position;
		Float Power;
		Vector4 Color;
	};

	struct SpotLightData
	{
		SpotLightData() = default;
		~SpotLightData() = default;

		SpotLightData( const Vector3& position, const Vector3& direction,
			Float innerAngle, Float outerAngle, Float power, const Vector3& color )
			: Position( position )
			, InnerAngle( innerAngle )
			, OuterAngle( outerAngle )
			, Direction( direction )
			, Power( power )
			, Color( color )
		{}

		Vector3 Position;
		Float InnerAngle;
		Vector3 Direction;
		Float OuterAngle;
		Vector3 Color;
		Float Power;
	};

	struct DirectionalLightData
	{
		Vector3 m_direction;
		Float m_padding0;
		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_padding1;
	};
}
