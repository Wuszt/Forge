#include "Fpch.h"
#include "PerspectiveCamera.h"

IMPLEMENT_TYPE( renderer::PerspectiveCamera );

namespace renderer
{
	PerspectiveCamera::PerspectiveCamera( Float aspectRatio, Float fov, Float nearPlane, Float farPlane )
		: m_aspectRatio( aspectRatio )
		, m_fov( fov )
		, m_nearPlane( nearPlane )
		, m_farPlane( farPlane )
	{}

	PerspectiveCamera::PerspectiveCamera() = default;

	PerspectiveCamera::~PerspectiveCamera() = default;

	Matrix PerspectiveCamera::ConstructProjectionMatrix( Float aspectRatio, Float fov, Float nearPlane, Float farPlane )
	{
		Float d = Math::Ctg( fov * 0.5f );
		Float da = d / aspectRatio;
		Float y0 = farPlane / ( farPlane - nearPlane );
		Float y1 = -nearPlane * farPlane / ( farPlane - nearPlane );

		return Matrix
		(
			da, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, y0, 1.0f,
			0.0f, d, 0.0f, 0.0f,
			0.0f, 0.0f, y1, 0.0f
		);
	}
}