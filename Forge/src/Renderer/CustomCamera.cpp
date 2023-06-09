#include "Fpch.h"
#include "CustomCamera.h"

RTTI_IMPLEMENT_TYPE( renderer::CustomCamera );

namespace renderer
{

	CustomCamera::CustomCamera() = default;

	Matrix CustomCamera::GetInvViewMatrix() const
	{
		return m_transform.ToMatrix();
	}

	Matrix CustomCamera::GetViewMatrix() const
	{
		return m_transform.ToMatrix().OrthonormInverted();
	}

	Matrix CustomCamera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * GetProjectionMatrix();
	}

	void CustomCamera::SetPosition( const Vector3& pos )
	{
		m_transform.SetPosition( pos );
	}

	const Vector3& CustomCamera::GetPosition() const
	{
		return m_transform.GetPosition3();
	}

	void CustomCamera::SetOrientation( const Quaternion& rot )
	{
		m_transform.SetOrientation( rot );
	}

	const Quaternion& CustomCamera::GetOrientation() const
	{
		return m_transform.GetOrientation();
	}
}