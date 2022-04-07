#include "Fpch.h"
#include "PerspectiveCamera.h"

namespace renderer
{
	PerspectiveCamera::PerspectiveCamera( Float aspectRatio, Float fov, Float nearPlane, Float farPlane )
		: m_aspectRatio( aspectRatio )
		, m_fov( fov )
		, m_nearPlane( nearPlane )
		, m_farPlane( farPlane )
	{}

	PerspectiveCamera::~PerspectiveCamera() = default;

	Matrix PerspectiveCamera::GetProjectionMatrix() const
	{
		return ConstructProjectionMatrix( m_aspectRatio, m_fov, m_nearPlane, m_farPlane );
	}

	Matrix PerspectiveCamera::GetInvViewMatrix() const
	{
		return m_transform.ToMatrix();
	}

	Matrix PerspectiveCamera::GetViewMatrix() const
	{
		return m_transform.ToMatrix().OrthonormInverted();
	}

	Matrix PerspectiveCamera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * GetProjectionMatrix();
	}

	void PerspectiveCamera::SetPosition( const Vector3& pos )
	{
		m_transform.SetPosition( pos );
	}

	const Vector3& PerspectiveCamera::GetPosition() const
	{
		return m_transform.GetPosition3();
	}

	void PerspectiveCamera::SetOrientation( const Quaternion& rot )
	{
		m_transform.SetOrientation( rot );
	}

	const Quaternion& PerspectiveCamera::GetOrientation() const
	{
		return m_transform.GetOrientation();
	}

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