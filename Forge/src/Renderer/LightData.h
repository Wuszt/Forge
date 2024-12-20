#pragma once

namespace renderer
{
	class IDepthStencilBuffer;

	struct PointLightData
	{
		PointLightData() = default;
		~PointLightData() = default;

		PointLightData( Matrix p, const Vector3& position, Float power, const LinearColor& color )
			: Position( position )
			, Power( power )
			, Color( color.R, color.G, color.B )
			, ProjectionA( p[ 1 ][ 2 ] )
			, ProjectionB( p[ 3 ][ 2 ] )
		{}

		Vector3 Position;
		Float Power;
		Vector3 Color;
		Float ProjectionA;
		Float ProjectionB;
		Float padding[3];
	};

	struct SpotLightData
	{
		SpotLightData() = default;
		~SpotLightData() = default;

		SpotLightData( const Vector3& position, const Vector3& direction,
			Float innerAngle, Float outerAngle, Float power, const LinearColor& color, const Matrix& vp )
			: Position( position )
			, InnerAngle( innerAngle )
			, OuterAngle( outerAngle )
			, Direction( direction )
			, Power( power )
			, Color( color.R, color.G, color.B )
			, VP( vp )
		{}

		Vector3 Position;
		Float InnerAngle;
		Vector3 Direction;
		Float OuterAngle;
		Vector3 Color;
		Float Power;
		Matrix VP;
	};

	struct DirectionalLightData
	{
		Vector3 Direction = Vector3::EY();
		Float padding0;
		Vector3 Color = { 1.0f, 1.0f, 1.0f };
		Float padding1;
		Matrix VP;
	};

	template< class T >
	struct LightData
	{
		T m_shaderData;
		std::shared_ptr< IDepthStencilBuffer > m_shadowMap;
	};
}
