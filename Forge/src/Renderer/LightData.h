#pragma once

namespace renderer
{
	struct LightData
	{
		LightData() = default;
		~LightData() = default;

		LightData( const Vector3& position, Float power, const Vector3& color )
			: Position( position )
			, Power( power )
			, Color( color )
		{}

		Vector3 Position;
		Float Power;;
		Vector4 Color;
	};
}
